#ifndef GPIO_BANK_PAGE_H
#define GPIO_BANK_PAGE_H

#include <wx/wx.h>
#include <wx/dataview.h>
#include <cstdint>
#include <vector>

#include "control_page.h"
#include "gpio.h"
#include "util.h"

using namespace SUSI::GPIO;

class GpioPage : public ControlPage {
public:
    GpioPage(wxWindow* parent,
        Bank bank,
        const wxString& key = "gpio.bank",
        const wxString& title = "",
        const wxString& desc = "");
    ~GpioPage() override = default;

    void SetBank(Bank bank);

private:
    std::uint32_t currentBankIndex_{ static_cast<std::uint32_t>(-1) };

    wxStaticText* bankLabel_ = nullptr;
    wxButton* btnRefresh_ = nullptr;
    wxDataViewListCtrl* grid_ = nullptr;

    wxButton* btnSetIn_ = nullptr;
    wxButton* btnSetOut_ = nullptr;
    wxButton* btnWriteLow_ = nullptr;
    wxButton* btnWriteHigh_ = nullptr;
    wxButton* btnRefreshSelected_ = nullptr;

    void BuildUI();
    void PopulateGridForCurrentBank();

    void RefreshAllRows();
    void RefreshSelectedRows();

    void ApplyDir(Dir dir);
    void ApplyWrite(Level level);

    void OnRefresh(wxCommandEvent& e);
    void OnSetIn(wxCommandEvent& e);
    void OnSetOut(wxCommandEvent& e);
    void OnWriteLow(wxCommandEvent& e);
    void OnWriteHigh(wxCommandEvent& e);
    void OnRefreshSelected(wxCommandEvent& e);

    static wxString DirToString(Dir d);
    static wxString LevelToString(Level l);
    static void ShowIfError(SusiStatus_t st, const wxString& what);

    bool GetAbsFromItem(const wxDataViewItem& item, std::uint32_t& outAbs) const;

    enum {
        COL_BIT = 0,
        COL_ABS = 1,
        COL_DIR = 2,
        COL_LEVEL = 3
    };
};

#endif
