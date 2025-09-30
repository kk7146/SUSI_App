#ifndef GPIO_PAGE_H
#define GPIO_PAGE_H

#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <array>

#include "control_page.h"

class GpioPage : public ControlPage {
public:
    explicit GpioPage(wxWindow* parent);

    void SetBank(int bank);
    int  GetBank() const { return bank_; }
    void SelectPin(int absolutePin);
private:
    int bank_ = 0;

    wxStaticText* lblBank_ = nullptr;
    wxCheckBox* chkAuto_ = nullptr;
    wxButton* btnRefresh_ = nullptr;

    wxStaticText* lblSupIn_ = nullptr;
    wxStaticText* lblSupOut_ = nullptr;
    wxStaticText* lblDir_ = nullptr;
    wxStaticText* lblVal_ = nullptr;

    wxFlexGridSizer* grid_ = nullptr;
    std::array<wxStaticText*, 32> pinLabel_{};
    std::array<wxCheckBox*, 32> dirOut_{};
    std::array<wxToggleButton*, 32> valBtn_{};

    wxButton* btnAllIn_ = nullptr;
    wxButton* btnAllOut_ = nullptr;
    wxButton* btnAllLow_ = nullptr;
    wxButton* btnAllHigh_ = nullptr;

    wxTimer pollTimer_;

    void BuildUI_();
    void RebuildPins_();
    void RefreshMasksAndRows_();
    void RefreshRow_(int bit);

    void OnToggleAuto_(wxCommandEvent&);
    void OnClickRefresh_(wxCommandEvent&);
    void OnToggleDir_(int bit);
    void OnToggleVal_(int bit);
    void OnAllIn_(wxCommandEvent&);
    void OnAllOut_(wxCommandEvent&);
    void OnAllLow_(wxCommandEvent&);
    void OnAllHigh_(wxCommandEvent&);
    void OnPollTimer_(wxTimerEvent&);
};

#endif
