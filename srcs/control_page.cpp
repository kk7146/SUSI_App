// control_page.cpp
#include "control_page.h"

ControlPage::ControlPage(wxWindow* parent,
    const wxString& key,
    const wxString& title,
    const wxString& desc)
    : wxPanel(parent),
    key_(key)
{
    auto* root = new wxBoxSizer(wxVERTICAL);

    const bool hasHeader = !title.IsEmpty() || !desc.IsEmpty();
    if (hasHeader) {
        auto* header = new wxBoxSizer(wxVERTICAL);

        if (!title.IsEmpty()) {
            auto* t = new wxStaticText(this, wxID_ANY, title);
            t->SetFont(t->GetFont().Bold().Larger());
            header->Add(t, 0);
        }
        if (!desc.IsEmpty()) {
            header->AddSpacer(2);
            header->Add(new wxStaticText(this, wxID_ANY, desc), 0);
        }

        root->Add(header, 0, wxALL, 8);

        auto* body = new wxPanel(this);
        auto* bodySizer = new wxBoxSizer(wxVERTICAL);
        body->SetSizer(bodySizer);

        root->Add(body, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);

        content_ = body;
        contentSizer_ = bodySizer;
    }
    else {
        content_ = this;
        contentSizer_ = root;
    }

    SetSizer(root);
}
