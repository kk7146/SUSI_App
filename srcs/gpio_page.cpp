#include <wx/wx.h>
#include "gpio_page.h"
#include "util.h"

static inline wxString Hex32(uint32_t v) {
    return wxString::Format("0x%08X", v);
}

GpioPage::GpioPage(wxWindow* parent, GPIO::Bank bank)
    : ControlPage(parent, "GPIO", "GPIO Control",
        "control page")
{
    bank_ = bank;
    BuildUI_();
}

void GpioPage::BuildUI_() {
    auto* vbox = contentSizer_;
    auto* panel = content_;

    {
        auto* row = new wxBoxSizer(wxHORIZONTAL);

        lblBank_ = new wxStaticText(panel, wxID_ANY, "Bank ?");
        lblBank_->SetFont(lblBank_->GetFont().Bold());
        row->Add(lblBank_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

        chkAuto_ = new wxCheckBox(panel, wxID_ANY, "Auto-Refresh");
        chkAuto_->SetValue(true);
        row->Add(chkAuto_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

        btnRefresh_ = new wxButton(panel, wxID_ANY, "Refresh");
        row->Add(btnRefresh_, 0, wxALIGN_CENTER_VERTICAL);

        vbox->Add(row, 0, wxEXPAND | wxBOTTOM, 6);
    }

    {
        auto* g = new wxFlexGridSizer(2, 4, 4, 12);
        g->AddGrowableCol(0);
        g->AddGrowableCol(1);
        g->AddGrowableCol(2);
        g->AddGrowableCol(3);

        lblSupIn_ = new wxStaticText(panel, wxID_ANY, "IN : -");
        lblSupOut_ = new wxStaticText(panel, wxID_ANY, "OUT: -");
        lblDir_ = new wxStaticText(panel, wxID_ANY, "DIR: -");
        lblVal_ = new wxStaticText(panel, wxID_ANY, "VAL: -");

        g->Add(new wxStaticText(panel, wxID_ANY, "Support (Input) :"));
        g->Add(lblSupIn_, 0, wxEXPAND);
        g->Add(new wxStaticText(panel, wxID_ANY, "Support (Output):"));
        g->Add(lblSupOut_, 0, wxEXPAND);
        g->Add(new wxStaticText(panel, wxID_ANY, "Direction (1=Out):"));
        g->Add(lblDir_, 0, wxEXPAND);
        g->Add(new wxStaticText(panel, wxID_ANY, "Value (1=High)  :"));
        g->Add(lblVal_, 0, wxEXPAND);

        vbox->Add(g, 0, wxEXPAND | wxBOTTOM, 8);
    }

    {
        auto* hdr = new wxFlexGridSizer(1, 3, 0, 12);
        hdr->Add(new wxStaticText(panel, wxID_ANY, "Pin"));
        hdr->Add(new wxStaticText(panel, wxID_ANY, "Direction (Output)"));
        hdr->Add(new wxStaticText(panel, wxID_ANY, "Value"));
        vbox->Add(hdr, 0, wxBOTTOM, 2);
    }

    grid_ = new wxFlexGridSizer(32, 3, 4, 12);
    grid_->AddGrowableCol(2);

    for (int bit = 0; bit < 32; ++bit) {
        pinLabel_[bit] = new wxStaticText(panel, wxID_ANY, wxString::Format("%2d", bit));
        dirOut_[bit] = new wxCheckBox(panel, wxID_ANY, "");
        valBtn_[bit] = new wxToggleButton(panel, wxID_ANY, "LOW");
        dirOut_[bit]->Bind(wxEVT_CHECKBOX, [this, bit](wxCommandEvent&) { OnToggleDir_(bit); });
        valBtn_[bit]->Bind(wxEVT_TOGGLEBUTTON, [this, bit](wxCommandEvent&) { OnToggleVal_(bit); });

        grid_->Add(pinLabel_[bit], 0, wxALIGN_CENTER_VERTICAL);
        grid_->Add(dirOut_[bit], 0, wxALIGN_CENTER_VERTICAL);
        grid_->Add(valBtn_[bit], 0, wxEXPAND);
    }
    vbox->Add(grid_, 0, wxEXPAND | wxBOTTOM, 8);

    {
        auto* row = new wxBoxSizer(wxHORIZONTAL);
        btnAllIn_ = new wxButton(panel, wxID_ANY, "All Input");
        btnAllOut_ = new wxButton(panel, wxID_ANY, "All Output");
        btnAllLow_ = new wxButton(panel, wxID_ANY, "All Low");
        btnAllHigh_ = new wxButton(panel, wxID_ANY, "All High");

        row->Add(btnAllIn_, 0, wxRIGHT, 6);
        row->Add(btnAllOut_, 0, wxRIGHT, 12);
        row->Add(btnAllLow_, 0, wxRIGHT, 6);
        row->Add(btnAllHigh_, 0);

        vbox->Add(row, 0, wxALIGN_RIGHT);
    }

    content_->Layout();

    chkAuto_->Bind(wxEVT_CHECKBOX, &GpioPage::OnToggleAuto_, this);
    btnRefresh_->Bind(wxEVT_BUTTON, &GpioPage::OnClickRefresh_, this);
    btnAllIn_->Bind(wxEVT_BUTTON, &GpioPage::OnAllIn_, this);
    btnAllOut_->Bind(wxEVT_BUTTON, &GpioPage::OnAllOut_, this);
    btnAllLow_->Bind(wxEVT_BUTTON, &GpioPage::OnAllLow_, this);
    btnAllHigh_->Bind(wxEVT_BUTTON, &GpioPage::OnAllHigh_, this);

    pollTimer_.Start(200);
}

void GpioPage::RefreshMasksAndRows_() {
}

void GpioPage::RefreshRow_(int bit) {
}

void GpioPage::OnToggleAuto_(wxCommandEvent&) {
}

void GpioPage::OnClickRefresh_(wxCommandEvent&) {
}

void GpioPage::OnToggleDir_(int bit) {
}

void GpioPage::OnToggleVal_(int bit) {
}

void GpioPage::OnAllIn_(wxCommandEvent&) {
}

void GpioPage::OnAllOut_(wxCommandEvent&) {
}

void GpioPage::OnAllLow_(wxCommandEvent&) {;
}

void GpioPage::OnAllHigh_(wxCommandEvent&) {
}

void GpioPage::OnPollTimer_(wxTimerEvent&) {
}