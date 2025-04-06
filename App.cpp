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



const char* username="admin";
const char* password="MYrecipe123";
const char* endpoint="tcp://recipe-db.ch66yyc8q7e9.eu-north-1.rds.amazonaws.com";


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

    searchSizer->Add(actionChoice, 0, wxALL, 5);
    searchSizer->Add(searchCtrl, 1, wxEXPAND | wxALL, 5);
    topSizer->Add(searchSizer, 0, wxEXPAND);

    recipeList = new wxListBox(panel, wxID_ANY);
    recipeDetail = new wxTextCtrl(panel, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize,
                                  wxTE_MULTILINE | wxTE_READONLY);

    mainSizer->Add(recipeList, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(recipeDetail, 2, wxEXPAND | wxALL, 5);

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
	

    vector<pair<string, string>> results = runRecipeQuery(conn,action.ToStdString(), params);
    recipeList->Clear();
    for (const auto& r : results) {
        recipeList->Append(wxString::Format("%s - %s", r.first, r.second));
    }
}

void RecipeApp::OnRecipeSelect(wxCommandEvent& event) {
    wxString selected = recipeList->GetStringSelection();
    wxString nameOnly = selected.BeforeFirst('-').Trim();
    string detail = fetchRecipeDetail(conn,nameOnly.ToStdString());
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

