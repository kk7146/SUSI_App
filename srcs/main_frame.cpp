#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <cstdint>
#include <string>

#include "Susi4.h"
#include "main_frame.h"

static inline wxString Hex32(uint32_t v) {
    return wxString::Format("0x%08X", v);
}
static inline void AppendLine(wxTextCtrl* log, const wxString& s) {
    if (!log) return;
    log->AppendText(s + "\n");
    log->ShowPosition(log->GetLastPosition());
}

MainFrame::MainFrame(wxWindow* parent, wxWindowID id) : wxFrame(nullptr, wxID_ANY, "SUSI GPIO (wxWidgets Sample)", wxDefaultPosition, wxSize(520, 420)) {
    auto* panel = new wxPanel(this);
    auto* vbox = new wxBoxSizer(wxVERTICAL);

    auto* h0 = new wxBoxSizer(wxHORIZONTAL);
    h0->Add(new wxStaticText(panel, wxID_ANY, "GPIO Pin (absolute index):"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    spinPin_ = new wxSpinCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 127, 0);
    h0->Add(spinPin_, 0);
    btnFindFirst_ = new wxButton(panel, wxID_ANY, "Find First Available");
    h0->AddSpacer(8);
    h0->Add(btnFindFirst_, 0);
    vbox->Add(h0, 0, wxALL, 8);

    auto* g1 = new wxStaticBoxSizer(wxVERTICAL, panel, "Direction");
    auto* h1 = new wxBoxSizer(wxHORIZONTAL);
    btnSetOut_ = new wxButton(panel, wxID_ANY, "Set OUTPUT (0)");
    btnSetIn_ = new wxButton(panel, wxID_ANY, "Set INPUT (1)");
    btnGetDir_ = new wxButton(panel, wxID_ANY, "Get Direction");
    h1->Add(btnSetOut_, 0, wxRIGHT, 6);
    h1->Add(btnSetIn_, 0, wxRIGHT, 6);
    h1->Add(btnGetDir_, 0);
    g1->Add(h1, 0, wxALL, 6);
    vbox->Add(g1, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    auto* g2 = new wxStaticBoxSizer(wxVERTICAL, panel, "Level");
    auto* h2 = new wxBoxSizer(wxHORIZONTAL);
    btnWriteLow_ = new wxButton(panel, wxID_ANY, "Write LOW (0)");
    btnWriteHigh_ = new wxButton(panel, wxID_ANY, "Write HIGH (1)");
    btnGetLvl_ = new wxButton(panel, wxID_ANY, "Get Level");
    lblLevel_ = new wxStaticText(panel, wxID_ANY, "Current Level: ?");
    h2->Add(btnWriteLow_, 0, wxRIGHT, 6);
    h2->Add(btnWriteHigh_, 0, wxRIGHT, 6);
    h2->Add(btnGetLvl_, 0, wxRIGHT, 12);
    h2->Add(lblLevel_, 0, wxALIGN_CENTER_VERTICAL);
    g2->Add(h2, 0, wxALL, 6);
    vbox->Add(g2, 0, wxLEFT | wxRIGHT | wxBOTTOM, 8);

    log_ = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    vbox->Add(log_, 1, wxEXPAND | wxALL, 8);

    panel->SetSizer(vbox);

    btnFindFirst_->Bind(wxEVT_BUTTON, &MainFrame::OnFindFirst, this);
    btnSetOut_->Bind(wxEVT_BUTTON, &MainFrame::OnSetOut, this);
    btnSetIn_->Bind(wxEVT_BUTTON, &MainFrame::OnSetIn, this);
    btnGetDir_->Bind(wxEVT_BUTTON, &MainFrame::OnGetDir, this);
    btnWriteLow_->Bind(wxEVT_BUTTON, &MainFrame::OnWriteLow, this);
    btnWriteHigh_->Bind(wxEVT_BUTTON, &MainFrame::OnWriteHigh, this);
    btnGetLvl_->Bind(wxEVT_BUTTON, &MainFrame::OnGetLevel, this);

    int first = FindFirstAvailablePin();
    if (first >= 0) {
        spinPin_->SetValue(first);
        AppendLine(log_, wxString::Format("First available pin: %d", first));
    }
    else {
        AppendLine(log_, "No available GPIO pin found.");
    }
}

SusiId_t MainFrame::PinToSingleId(int pin) const {
    return SUSI_ID_GPIO(static_cast<uint8_t>(pin));
}

int MainFrame::FindFirstAvailablePin() {
    for (uint8_t bank = 0; bank < 16; ++bank) {
        uint32_t idBank = SUSI_ID_GPIO_BANK(bank);
        uint32_t supIn = 0, supOut = 0;

        auto st1 = SusiGPIOGetCaps(idBank, SUSI_ID_GPIO_INPUT_SUPPORT, &supIn);
        auto st2 = SusiGPIOGetCaps(idBank, SUSI_ID_GPIO_OUTPUT_SUPPORT, &supOut);
        if (st1 != SUSI_STATUS_SUCCESS || st2 != SUSI_STATUS_SUCCESS)
            continue;

        uint32_t mask = supIn | supOut;
        if (mask == 0) continue;

        for (int bit = 0; bit < 32; ++bit) {
            if (mask & (1u << bit)) {
                return bank * 32 + bit;
            }
        }
    }
    return -1;
}

void MainFrame::OnFindFirst(wxCommandEvent&) {
    int first = FindFirstAvailablePin();
    if (first >= 0) {
        spinPin_->SetValue(first);
        AppendLine(log_, wxString::Format("First available pin: %d", first));
    }
    else {
        AppendLine(log_, "No available GPIO pin found.");
    }
}

void MainFrame::OnSetOut(wxCommandEvent&) {
    int pin = spinPin_->GetValue();
    SusiId_t id = PinToSingleId(pin);
    uint32_t st = SusiGPIOSetDirection(id, /*mask*/1u, /*Direction*/0u);
    AppendLine(log_, wxString::Format("SetDirection OUTPUT -> %s (pin %d, st=%s)",
        st == SUSI_STATUS_SUCCESS ? "OK" : "FAIL", pin, Hex32(st)));
}

void MainFrame::OnSetIn(wxCommandEvent&) {
    int pin = spinPin_->GetValue();
    SusiId_t id = PinToSingleId(pin);
    uint32_t st = SusiGPIOSetDirection(id, /*mask*/1u, /*Direction*/1u);
    AppendLine(log_, wxString::Format("SetDirection INPUT -> %s (pin %d, st=%s)",
        st == SUSI_STATUS_SUCCESS ? "OK" : "FAIL", pin, Hex32(st)));
}

void MainFrame::OnGetDir(wxCommandEvent&) {
    int pin = spinPin_->GetValue();
    SusiId_t id = PinToSingleId(pin);
    uint32_t dir = 0;
    uint32_t st = SusiGPIOGetDirection(id, /*mask*/1u, &dir);
    if (st == SUSI_STATUS_SUCCESS) {
        bool isInput = (dir & 1u) != 0;
        AppendLine(log_, wxString::Format("GetDirection: %s (raw=%s)", isInput ? "INPUT(1)" : "OUTPUT(0)", Hex32(dir)));
    }
    else {
        AppendLine(log_, wxString::Format("GetDirection FAIL (st=%s)", Hex32(st)));
    }
}

void MainFrame::OnWriteLow(wxCommandEvent&) {
    int pin = spinPin_->GetValue();
    SusiId_t id = PinToSingleId(pin);
    uint32_t st = SusiGPIOSetLevel(id, /*mask*/1u, /*Level*/0u);
    AppendLine(log_, wxString::Format("SetLevel LOW -> %s (pin %d, st=%s)",
        st == SUSI_STATUS_SUCCESS ? "OK" : "FAIL", pin, Hex32(st)));
    if (st == SUSI_STATUS_SUCCESS) OnGetLevelStub(pin);
}

void MainFrame::OnWriteHigh(wxCommandEvent&) {
    int pin = spinPin_->GetValue();
    SusiId_t id = PinToSingleId(pin);
    uint32_t st = SusiGPIOSetLevel(id, /*mask*/1u, /*Level*/1u);
    AppendLine(log_, wxString::Format("SetLevel HIGH -> %s (pin %d, st=%s)",
        st == SUSI_STATUS_SUCCESS ? "OK" : "FAIL", pin, Hex32(st)));
    if (st == SUSI_STATUS_SUCCESS) OnGetLevelStub(pin);
}

void MainFrame::OnGetLevelStub(int pin) {
    SusiId_t id = PinToSingleId(pin);
    uint32_t level = 0;
    uint32_t st = SusiGPIOGetLevel(id, /*mask*/1u, &level);
    if (st == SUSI_STATUS_SUCCESS) {
        int v = (level & 1u) ? 1 : 0;
        lblLevel_->SetLabel(wxString::Format("Current Level: %d", v));
        AppendLine(log_, wxString::Format("GetLevel: %d (raw=%s)", v, Hex32(level)));
    }
    else {
        AppendLine(log_, wxString::Format("GetLevel FAIL (st=%s)", Hex32(st)));
    }
}

void MainFrame::OnGetLevel(wxCommandEvent&) {
    int pin = spinPin_->GetValue();
    OnGetLevelStub(pin);
}