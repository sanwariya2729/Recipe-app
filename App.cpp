#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "login.h"
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <memory>
#include <vector>
#include <string>
#include "fetch_data.h"

using namespace std;
using namespace sql;



const char* username="root";
const char* password="1234";
const char* endpoint="tcp://127.0.0.1:3306";


Connection* connect_database(){
	try{
		mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
		Connection* conn = driver->connect(endpoint,username,password);
		conn->setSchema("recipe_db");
		return conn;
	
	}catch(SQLException& e){
		cerr<< "Connection failed:"<<e.what()<<endl;
		exit(1);
	}
	
}



//helper function
int getUserId(const std::string& username,shared_ptr<Connection> conn) {
    try {
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT user_id FROM users WHERE username = ?"
        );
        pstmt->setString(1, username);
        sql::ResultSet* res = pstmt->executeQuery();

        int uid = -1;
        if (res->next()) {
            uid = res->getInt("user_id");
        }

        delete res;
        delete pstmt;
        return uid;
    } catch (sql::SQLException& e) {
        std::cerr << "SQL error in getUserId: " << e.what() << std::endl;
        return -1;
    }
}




class LoginDialog : public wxDialog {
public:
    wxTextCtrl* usernameCtrl;
    wxTextCtrl* passwordCtrl;
    wxTextCtrl* emailCtrl;
    wxString username;
    

    LoginDialog(wxWindow* parent)
        : wxDialog(parent, wxID_ANY, "Login", wxDefaultPosition, wxSize(600, 400)) {

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        usernameCtrl = new wxTextCtrl(this, wxID_ANY);
        passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
        emailCtrl = new wxTextCtrl(this,wxID_ANY);

        sizer->Add(new wxStaticText(this, wxID_ANY, "Username:"), 0, wxALL, 5);
        sizer->Add(usernameCtrl, 0, wxEXPAND | wxALL, 5);
        sizer->Add(new wxStaticText(this, wxID_ANY, "Password:"), 0, wxALL, 5);
        sizer->Add(passwordCtrl, 0, wxEXPAND | wxALL, 5);
        sizer->Add(new wxStaticText(this, wxID_ANY, "email:"), 0, wxALL, 5);
        sizer->Add(emailCtrl, 0, wxEXPAND | wxALL, 5);

        wxButton* loginButton = new wxButton(this, wxID_OK, "Login");
        sizer->Add(loginButton, 0, wxALIGN_CENTER | wxALL, 10);

        SetSizer(sizer);
    }

    wxString GetUsername() const {
        return usernameCtrl->GetValue();
    }

    wxString GetPassword() const {
        return passwordCtrl->GetValue();
    }
    wxString Getemail() const {
        return emailCtrl->GetValue();
    }
};



class RecipeApp : public wxFrame {
public:
    RecipeApp(const wxString& title,shared_ptr<Connection> conn,int user_id);

private:
    wxTextCtrl* searchCtrl;
    wxChoice* actionChoice;
    wxListBox* recipeList;
    wxTextCtrl* recipeDetail;
    shared_ptr<Connection> conn;
    int user_id;

    void OnSearch(wxCommandEvent& event);
    void OnRecipeSelect(wxCommandEvent& event);
    //void OnAddRecipe(wxCommandEvent& event);
	void OnAddFavorite(wxCommandEvent& event);
	void OnAddReview(wxCommandEvent& event);
	int getRecipeIdByTitle(const std::string& title);

};

RecipeApp::RecipeApp(const wxString& title,shared_ptr<Connection> conn_,int user_id)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 800)),conn(conn_),user_id(user_id) {
    wxPanel* panel = new wxPanel(this);
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* bottomSizer= new wxBoxSizer(wxHORIZONTAL);
    
    wxButton* addRecipeBtn = new wxButton(panel, wxID_ANY, "Add Recipe");
	wxButton* addFavoriteBtn = new wxButton(panel, wxID_ANY, "Add to Favorites");
	wxButton* addReviewBtn = new wxButton(panel, wxID_ANY, "Add Review");

	bottomSizer->Add(addRecipeBtn, 0, wxALL, 5);
	bottomSizer->Add(addFavoriteBtn, 0, wxALL, 5);
	bottomSizer->Add(addReviewBtn, 0, wxALL, 5);	
	
	
    actionChoice = new wxChoice(panel, wxID_ANY);
    for (const auto& entry : queryMap) {
        actionChoice->Append(entry.first);
    }
    actionChoice->SetSelection(0);

    searchCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    searchSizer->Add(actionChoice, 0, wxALL, 10);
    searchSizer->Add(searchCtrl, 1, wxEXPAND | wxALL, 10);
    topSizer->Add(searchSizer, 0, wxEXPAND);

    recipeList = new wxListBox(panel, wxID_ANY);
    recipeDetail = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize,
                                  wxTE_MULTILINE | wxTE_READONLY);
	
	wxFont listFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont detailFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	recipeList->SetFont(listFont);
	recipeDetail->SetFont(detailFont);
	
    mainSizer->Add(recipeList, 2, wxEXPAND | wxALL, 5);
    mainSizer->Add(recipeDetail, 3, wxEXPAND | wxALL, 5);

    topSizer->Add(mainSizer, 1, wxEXPAND);
    topSizer->Add(bottomSizer, 0, wxALIGN_CENTER);
    panel->SetSizer(topSizer);

    searchCtrl->Bind(wxEVT_TEXT_ENTER, &RecipeApp::OnSearch, this);
    recipeList->Bind(wxEVT_LISTBOX, &RecipeApp::OnRecipeSelect, this);
    //addRecipeBtn->Bind(wxEVT_BUTTON, &RecipeApp::OnAddRecipe, this);
	addFavoriteBtn->Bind(wxEVT_BUTTON, &RecipeApp::OnAddFavorite, this);
	addReviewBtn->Bind(wxEVT_BUTTON, &RecipeApp::OnAddReview, this);

}

//helper function
int RecipeApp::getRecipeIdByTitle(const std::string& title) {
    shared_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
        "SELECT recipe_id FROM recipes WHERE title = ?"
    ));
    stmt->setString(1, title);
    shared_ptr<sql::ResultSet> res(stmt->executeQuery());

    if (res->next()) {
        return res->getInt("recipe_id");
    }
    return -1; // Not found
}
void RecipeApp::OnSearch(wxCommandEvent& event) {
    wxString action = actionChoice->GetStringSelection();
    wxString term = searchCtrl->GetValue();
    vector<string> params;
    if (!term.IsEmpty()) params.push_back(term.ToStdString());

    vector<pair<string, string>> results = runRecipeQuery(conn, action.ToStdString(), params);
    recipeList->Clear();

    for (const auto& r : results) {
        recipeList->Append(wxString::Format("%s - %s", r.first, r.second));
    }
}


void RecipeApp::OnRecipeSelect(wxCommandEvent& event) {
    wxString selected = recipeList->GetStringSelection();

    
    int sepIndex = selected.Find(" - ");
    wxString titleOnly = (sepIndex != wxNOT_FOUND) ? selected.Left(sepIndex).Trim() : selected.Trim();

    std::string recipeTitle = titleOnly.ToStdString();
    std::cout << "Fetching details for: " << recipeTitle << std::endl;

    string detail = fetchRecipeDetail(conn, recipeTitle);
    recipeDetail->SetValue(detail);
}




void RecipeApp::OnAddFavorite(wxCommandEvent& event) {
    wxString selected = recipeList->GetStringSelection();
    if (selected.IsEmpty()) {
        wxMessageBox("Select a recipe first.", "Error");
        return;
    }

    wxString title = selected.BeforeFirst('-').Trim();
    int recipeId = getRecipeIdByTitle(title.ToStdString()); // Implement this helper

    try {
        shared_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO favorites (user_id, recipe_id) VALUES (?, ?)"
        ));
        stmt->setInt(1, user_id);
        stmt->setInt(2, recipeId);
        stmt->executeUpdate();

        wxMessageBox("Recipe added to favorites!", "Success");
    } catch (sql::SQLException& e) {
        wxMessageBox("Already in favorites or error occurred.", "Error");
    }
}

void RecipeApp::OnAddReview(wxCommandEvent& event) {
    wxString selected = recipeList->GetStringSelection();
    if (selected.IsEmpty()) {
        wxMessageBox("Select a recipe first.", "Error");
        return;
    }

    wxString title = selected.BeforeFirst('-').Trim();
    int recipeId = getRecipeIdByTitle(title.ToStdString());

    wxTextEntryDialog commentDialog(this, "Enter your review:", "Add Review");
    if (commentDialog.ShowModal() != wxID_OK) return;
    wxString comment = commentDialog.GetValue();

    wxTextEntryDialog ratingDialog(this, "Enter rating (1-5):", "Add Review");
    if (ratingDialog.ShowModal() != wxID_OK) return;
    int rating = wxAtoi(ratingDialog.GetValue());

    try {
        shared_ptr<sql::PreparedStatement> stmt(conn->prepareStatement(
            "INSERT INTO reviews (recipe_id, user_id, rating, comment) VALUES (?, ?, ?, ?)"
        ));
        stmt->setInt(1, recipeId);
        stmt->setInt(2, user_id);
        stmt->setInt(3, rating);
        stmt->setString(4, comment.ToStdString());
        stmt->executeUpdate();

        wxMessageBox("Review submitted!", "Success");
    } catch (sql::SQLException& e) {
        wxMessageBox("Failed to submit review.", "Error");
    }
}





class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

bool MyApp::OnInit() {
	shared_ptr<Connection> conn(connect_database());

    LoginDialog loginDialog(nullptr);
    if (loginDialog.ShowModal() == wxID_OK) {
        wxString username = loginDialog.GetUsername();
        wxString password = loginDialog.GetPassword();
        wxString email = loginDialog.Getemail();

        std::string uname = username.ToStdString();
        std::string pword = password.ToStdString();
        std::string emal = email.ToStdString();

        if (validateUser(conn,uname, pword)) {
            int userId = getUserId(uname,conn);
            RecipeApp* mainWin = new RecipeApp("Recipe App",conn, userId);
            mainWin->Show(true);
            return true;
        } else {
            int answer = wxMessageBox("User not found. Create new user?", "Create Account", wxYES_NO | wxICON_QUESTION);
            if (answer == wxYES) {
                if ((!uname.empty() && !pword.empty() ) && createUser(conn,uname, pword,emal)) {
                    wxMessageBox("User created successfully!", "Success", wxOK | wxICON_INFORMATION);
                    int userId = getUserId(uname,conn); 
                    RecipeApp* mainWin = new RecipeApp("Recipe App",conn, userId);
                    mainWin->Show(true);
                    return true;
                } else {
                    wxMessageBox("Failed to create user.", "Error", wxOK | wxICON_ERROR);
                }
            }
        }
    }
    return false;
}

wxIMPLEMENT_APP(MyApp);

