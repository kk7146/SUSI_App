#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <cstdint>
#include <string>

#include "Susi4.h"

class MainFrame : public wxFrame {
public:
    explicit MainFrame(wxWindow* parent, wxWindowID id = wxID_ANY);

private:
    int FindFirstAvailablePin();
    SusiId_t PinToSingleId(int pin) const;
    void OnFindFirst(wxCommandEvent&);
    void OnSetOut(wxCommandEvent&);
    void OnSetIn(wxCommandEvent&);
    void OnGetDir(wxCommandEvent&);
    void OnWriteLow(wxCommandEvent&);
    void OnWriteHigh(wxCommandEvent&);
    void OnGetLevel(wxCommandEvent&);
    void OnGetLevelStub(int pin);

private:
    wxSpinCtrl* spinPin_{};
    wxButton* btnFindFirst_{};
    wxButton* btnSetOut_{};
    wxButton* btnSetIn_{};
    wxButton* btnGetDir_{};
    wxButton* btnWriteLow_{};
    wxButton* btnWriteHigh_{};
    wxButton* btnGetLvl_{};
    wxStaticText* lblLevel_{};
    wxTextCtrl* log_{};
};

#endif