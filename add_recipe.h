#ifndef ADD_RECIPE_DIALOG_H
#define ADD_RECIPE_DIALOG_H

#include <wx/wx.h>
#include <string>
#include <iostream>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <vector>
using namespace std;



class AddRecipeDialog : public wxDialog {
public:
    AddRecipeDialog(wxWindow* parent);
    ~AddRecipeDialog();

    wxString GetTitle() const { return titleCtrl->GetValue(); }
    wxString GetDescription() const { return descCtrl->GetValue(); }
    std::vector<wxString> GetIngredients() const;
    std::vector<wxString> GetSteps() const;

private:
    wxTextCtrl* titleCtrl;
    wxTextCtrl* descCtrl;
    wxListBox* ingredientsList;
    wxTextCtrl* ingredientInput;
    wxListBox* stepsList;
    wxTextCtrl* stepInput;

    void OnAddIngredient(wxCommandEvent& event);
    void OnAddStep(wxCommandEvent& event);


};

AddRecipeDialog::~AddRecipeDialog(){
}


void AddRecipeDialog::OnAddIngredient(wxCommandEvent&) {
    wxString ing = ingredientInput->GetValue().Trim();
    if (!ing.IsEmpty()) {
        ingredientsList->Append(ing);
        ingredientInput->Clear();
    }
}

void AddRecipeDialog::OnAddStep(wxCommandEvent&) {
    wxString step = stepInput->GetValue().Trim();
    if (!step.IsEmpty()) {
        stepsList->Append(step);
        stepInput->Clear();
    }
}

std::vector<wxString> AddRecipeDialog::GetIngredients() const {
    std::vector<wxString> list;
    for (size_t i = 0; i < ingredientsList->GetCount(); ++i)
        list.push_back(ingredientsList->GetString(i));
    return list;
}

std::vector<wxString> AddRecipeDialog::GetSteps() const {
    std::vector<wxString> list;
    for (size_t i = 0; i < stepsList->GetCount(); ++i)
        list.push_back(stepsList->GetString(i));
    return list;
}
AddRecipeDialog::AddRecipeDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "Add Recipe", wxDefaultPosition, wxSize(800, 700)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    titleCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    descCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 100), wxTE_MULTILINE);

    ingredientInput = new wxTextCtrl(this, wxID_ANY);
    wxButton* addIngBtn = new wxButton(this, wxID_HIGHEST + 1, "Add Ingredient");
    ingredientsList = new wxListBox(this, wxID_ANY);

    stepInput = new wxTextCtrl(this, wxID_ANY);
    wxButton* addStepBtn = new wxButton(this, wxID_HIGHEST + 2, "Add Step");
    stepsList = new wxListBox(this, wxID_ANY);

    wxButton* submitBtn = new wxButton(this, wxID_OK, "Submit");

    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Title"), 0, wxTOP | wxLEFT, 10);
    mainSizer->Add(titleCtrl, 0, wxEXPAND | wxALL, 5);

    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Description"), 0, wxTOP | wxLEFT, 10);
    mainSizer->Add(descCtrl, 0, wxEXPAND | wxALL, 5);

    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Ingredients"), 0, wxTOP | wxLEFT, 10);
    mainSizer->Add(ingredientInput, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(addIngBtn, 0, wxALL, 5);
    mainSizer->Add(ingredientsList, 1, wxEXPAND | wxALL, 5);

    mainSizer->Add(new wxStaticText(this, wxID_ANY, "Steps"), 0, wxTOP | wxLEFT, 10);
    mainSizer->Add(stepInput, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(addStepBtn, 0, wxALL, 5);
    mainSizer->Add(stepsList, 1, wxEXPAND | wxALL, 5);

    mainSizer->Add(submitBtn, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizer(mainSizer);

    addIngBtn->Bind(wxEVT_BUTTON, &AddRecipeDialog::OnAddIngredient, this);
    addStepBtn->Bind(wxEVT_BUTTON, &AddRecipeDialog::OnAddStep, this);
}



#endif

