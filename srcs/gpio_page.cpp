#include "gpio_page.h"
#include "util.h"

using namespace SUSI::GPIO;

namespace {
    constexpr std::uint32_t PINS_PER_BANK_V = PINS_PER_BANK;
}

GpioPage::GpioPage(wxWindow* parent,
    Bank bank,
    const wxString& key,
    const wxString& title,
    const wxString& desc)
    : ControlPage(parent, key, title, desc)
{
    currentBankIndex_ = bank.index;
    BuildUI();
    PopulateGridForCurrentBank();
}

void GpioPage::SetBank(Bank bank)
{
    if (!bank.valid()) {
        wxMessageBox("Invalid bank.", "GPIO", wxOK | wxICON_WARNING, this);
        return;
    }
    currentBankIndex_ = bank.index;
    if (bankLabel_) {
        bankLabel_->SetLabel(wxString::Format("Bank %u", currentBankIndex_));
        bankLabel_->Wrap(-1);
    }
    PopulateGridForCurrentBank();
}

void GpioPage::BuildUI()
{
    auto* root = new wxBoxSizer(wxVERTICAL);

  
    {
        auto* top = new wxBoxSizer(wxHORIZONTAL);

        bankLabel_ = new wxStaticText(content_, wxID_ANY,
            wxString::Format("Bank %u", currentBankIndex_));
        bankLabel_->SetFont(bankLabel_->GetFont().Bold());
        top->Add(bankLabel_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

        btnRefresh_ = new wxButton(content_, wxID_ANY, "Refresh");
        btnRefresh_->Bind(wxEVT_BUTTON, &GpioPage::OnRefresh, this);
        top->Add(btnRefresh_, 0);

        root->Add(top, 0, wxBOTTOM, 6);
    }

  
    {
        grid_ = new wxDataViewListCtrl(content_, wxID_ANY,
            wxDefaultPosition, wxDefaultSize,
            wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_MULTIPLE);

        grid_->AppendTextColumn("Bit", wxDATAVIEW_CELL_INERT, 60, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE);
        grid_->AppendTextColumn("Direction", wxDATAVIEW_CELL_INERT, 110, wxALIGN_LEFT);
        grid_->AppendTextColumn("Level", wxDATAVIEW_CELL_INERT, 80, wxALIGN_LEFT);

        root->Add(grid_, 1, wxEXPAND | wxBOTTOM, 6);
    }

  
    {
        auto* bottom = new wxBoxSizer(wxHORIZONTAL);
        btnSetIn_ = new wxButton(content_, wxID_ANY, "Set In");
        btnSetOut_ = new wxButton(content_, wxID_ANY, "Set Out");
        btnWriteLow_ = new wxButton(content_, wxID_ANY, "Write Low");
        btnWriteHigh_ = new wxButton(content_, wxID_ANY, "Write High");
        btnRefreshSelected_ = new wxButton(content_, wxID_ANY, "Refresh Selected");

        bottom->Add(btnSetIn_, 0, wxRIGHT, 4);
        bottom->Add(btnSetOut_, 0, wxRIGHT, 12);
        bottom->Add(btnWriteLow_, 0, wxRIGHT, 4);
        bottom->Add(btnWriteHigh_, 0, wxRIGHT, 12);
        bottom->AddStretchSpacer(1);
        bottom->Add(btnRefreshSelected_, 0);

        btnSetIn_->Bind(wxEVT_BUTTON, &GpioPage::OnSetIn, this);
        btnSetOut_->Bind(wxEVT_BUTTON, &GpioPage::OnSetOut, this);
        btnWriteLow_->Bind(wxEVT_BUTTON, &GpioPage::OnWriteLow, this);
        btnWriteHigh_->Bind(wxEVT_BUTTON, &GpioPage::OnWriteHigh, this);
        btnRefreshSelected_->Bind(wxEVT_BUTTON, &GpioPage::OnRefreshSelected, this);

        root->Add(bottom, 0);
    }

    content_->SetSizer(root);
}

void GpioPage::PopulateGridForCurrentBank()
{
    grid_->DeleteAllItems();

    if (currentBankIndex_ == static_cast<std::uint32_t>(-1)) {
        return;
    }

    const std::uint32_t bankIndex = currentBankIndex_;

    for (std::uint32_t bit = 0; bit < PINS_PER_BANK_V; ++bit) {
        const std::uint32_t abs = bankIndex * PINS_PER_BANK_V + bit;
        const Pin pin{ Bank{ bankIndex }, bit };

        Dir d = Dir::Unknown;
        Level lv = Level::Unknown;
        (void)GetDir(pin, d);
        (void)GetLevel(pin, lv);

        wxVector<wxVariant> row;
        row.push_back(wxVariant(wxString::Format("%u", bit)));
        row.push_back(wxVariant(wxString::Format("%u", abs)));
        row.push_back(wxVariant(DirToString(d)));
        row.push_back(wxVariant(LevelToString(lv)));

        grid_->AppendItem(row);
    }
}

void GpioPage::RefreshAllRows()
{
    const unsigned count = grid_->GetItemCount();
    for (unsigned r = 0; r < count; ++r) {
        auto item = grid_->RowToItem(r);

        std::uint32_t abs = 0;
        if (!GetAbsFromItem(item, abs)) continue;

        const auto bank = abs / PINS_PER_BANK_V;
        const auto bit = abs % PINS_PER_BANK_V;
        const Pin pin{ Bank{ bank }, bit };

        Dir d = Dir::Unknown;
        Level lv = Level::Unknown;
        (void)GetDir(pin, d);
        (void)GetLevel(pin, lv);

        grid_->SetValue(wxVariant(DirToString(d)), r, COL_DIR);
        grid_->SetValue(wxVariant(LevelToString(lv)), r, COL_LEVEL);
    }
}

void GpioPage::RefreshSelectedRows()
{
    wxDataViewItemArray sel;
    grid_->GetSelections(sel);
    for (auto& item : sel) {
        int r = grid_->ItemToRow(item);
        if (r < 0) continue;

        std::uint32_t abs = 0;
        if (!GetAbsFromItem(item, abs)) continue;

        const auto bank = abs / PINS_PER_BANK_V;
        const auto bit = abs % PINS_PER_BANK_V;
        const Pin pin{ Bank{ bank }, bit };

        Dir d = Dir::Unknown;
        Level lv = Level::Unknown;
        (void)GetDir(pin, d);
        (void)GetLevel(pin, lv);

        grid_->SetValue(wxVariant(DirToString(d)), r, COL_DIR);
        grid_->SetValue(wxVariant(LevelToString(lv)), r, COL_LEVEL);
    }
}

void GpioPage::ApplyDir(Dir dir)
{
    wxDataViewItemArray sel;
    grid_->GetSelections(sel);
    if (sel.empty()) {
        wxMessageBox("Select one or more pins first.", "No Selection",
            wxOK | wxICON_INFORMATION, this);
        return;
    }

    for (auto& item : sel) {
        std::uint32_t abs = 0;
        if (!GetAbsFromItem(item, abs)) continue;

        const auto bank = abs / PINS_PER_BANK_V;
        const auto bit = abs % PINS_PER_BANK_V;
        const Pin pin{ Bank{ bank }, bit };

        SusiStatus_t st = SUSI_STATUS_SUCCESS;
        if (dir == Dir::Input)       st = SetIn(pin);
        else if (dir == Dir::Output) st = SetOut(pin);
        else continue;

        ShowIfError(st, dir == Dir::Input ? "SetIn" : "SetOut");
    }
    RefreshSelectedRows();
}

void GpioPage::ApplyWrite(Level level)
{
    wxDataViewItemArray sel;
    grid_->GetSelections(sel);
    if (sel.empty()) {
        wxMessageBox("Select one or more pins first.", "No Selection",
            wxOK | wxICON_INFORMATION, this);
        return;
    }

    for (auto& item : sel) {
        std::uint32_t abs = 0;
        if (!GetAbsFromItem(item, abs)) continue;

        const auto bank = abs / PINS_PER_BANK_V;
        const auto bit = abs % PINS_PER_BANK_V;
        const Pin pin{ Bank{ bank }, bit };

        SusiStatus_t st = SUSI_STATUS_SUCCESS;
        if (level == Level::Low)       st = WriteLow(pin);
        else if (level == Level::High) st = WriteHigh(pin);
        else continue;

        ShowIfError(st, level == Level::Low ? "WriteLow" : "WriteHigh");
    }
    RefreshSelectedRows();
}

void GpioPage::OnRefresh(wxCommandEvent&)
{
    PopulateGridForCurrentBank();
}

void GpioPage::OnSetIn(wxCommandEvent&) { ApplyDir(Dir::Input); }
void GpioPage::OnSetOut(wxCommandEvent&) { ApplyDir(Dir::Output); }
void GpioPage::OnWriteLow(wxCommandEvent&) { ApplyWrite(Level::Low); }
void GpioPage::OnWriteHigh(wxCommandEvent&) { ApplyWrite(Level::High); }
void GpioPage::OnRefreshSelected(wxCommandEvent&) { RefreshSelectedRows(); }

wxString GpioPage::DirToString(Dir d)
{
    switch (d) {
    case Dir::Input:  return "Input";
    case Dir::Output: return "Output";
    default:          return "Unknown";
    }
}

wxString GpioPage::LevelToString(Level l)
{
    switch (l) {
    case Level::Low:  return "Low";
    case Level::High: return "High";
    default:          return "Unknown";
    }
}

void GpioPage::ShowIfError(SusiStatus_t st, const wxString& what)
{
    if (st != SUSI_STATUS_SUCCESS) {
        wxMessageBox(wxString::Format("%s failed (status=%d)", what, static_cast<int>(st)),
            "SUSI Error", wxOK | wxICON_ERROR);
    }
}

bool GpioPage::GetAbsFromItem(const wxDataViewItem& item, std::uint32_t& outAbs) const
{
    if (!item.IsOk()) return false;
    int row = grid_->ItemToRow(item);
    if (row < 0) return false;

    wxVariant v;
    grid_->GetValue(v, row, COL_ABS);
    long l = 0;
    if (!v.GetString().ToLong(&l)) return false;
    outAbs = static_cast<std::uint32_t>(l);
    return true;
}
