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
//#include "login.h"

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




class RecipeApp : public wxFrame {
public:
    RecipeApp(const wxString& title);

private:
    wxTextCtrl* searchCtrl;
    wxChoice* actionChoice;
    wxListBox* recipeList;
    wxTextCtrl* recipeDetail;
    Connection* conn;

    void OnSearch(wxCommandEvent& event);
    void OnRecipeSelect(wxCommandEvent& event);
};

RecipeApp::RecipeApp(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 800)) {
	conn=connect_database();
    wxPanel* panel = new wxPanel(this);
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

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
	wxFont detailFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	recipeList->SetFont(listFont);
	recipeDetail->SetFont(detailFont);
	
    mainSizer->Add(recipeList, 2, wxEXPAND | wxALL, 5);
    mainSizer->Add(recipeDetail, 3, wxEXPAND | wxALL, 5);

    topSizer->Add(mainSizer, 1, wxEXPAND);
    panel->SetSizer(topSizer);

    searchCtrl->Bind(wxEVT_TEXT_ENTER, &RecipeApp::OnSearch, this);
    recipeList->Bind(wxEVT_LISTBOX, &RecipeApp::OnRecipeSelect, this);
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




class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

bool MyApp::OnInit() {
    
    

    RecipeApp* frame = new RecipeApp("Recipe Manager");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);

