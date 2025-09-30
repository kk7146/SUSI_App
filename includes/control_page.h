#ifndef CONTROL_PAGE_H
#define CONTROL_PAGE_H

#include <wx/wx.h>

class ControlPage : public wxPanel {
public:
    ControlPage(wxWindow* parent,
        const wxString& key,
        const wxString& title,
        const wxString& desc = "");
    virtual ~ControlPage() = default;

    const wxString& Key() const { return key_; }
protected:
    wxPanel* content_ = nullptr;
    wxBoxSizer* contentSizer_ = nullptr;

private:
    wxString key_;
};

#endif
