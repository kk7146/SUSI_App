#include <wx/wx.h>
#include "control_page.h"

ControlPage::ControlPage(wxWindow* parent,
    const wxString& key,
    const wxString& title,
    const wxString& desc)
    : wxPanel(parent), key_(key) {

    auto* vbox = new wxBoxSizer(wxVERTICAL);
    auto* t = new wxStaticText(this, wxID_ANY, title);
    t->SetFont(t->GetFont().Bold().Larger());
    vbox->AddSpacer(10);
    vbox->Add(t, 0, wxLEFT | wxRIGHT, 12);
    if (!desc.IsEmpty()) {
        vbox->AddSpacer(4);
        vbox->Add(new wxStaticText(this, wxID_ANY, desc), 0, wxLEFT | wxRIGHT | wxBOTTOM, 12);
    }
    content_ = new wxPanel(this);
    contentSizer_ = new wxBoxSizer(wxVERTICAL);
    content_->SetSizer(contentSizer_);
    vbox->Add(content_, 1, wxEXPAND | wxALL, 8);

    SetSizer(vbox);
}