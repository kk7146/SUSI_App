#ifndef CONTROL_PAGE_H
#define CONTROL_PAGE_H

#include <wx/wx.h>

class ControlPage : public wxPanel {
public:
    ControlPage(wxWindow* parent,
        const wxString& key,
        const wxString& title,
        const wxString& desc = "");
    ~ControlPage() override = default;

    const wxString& Key() const { return key_; }

    wxBoxSizer* GetContentSizer() const { return contentSizer_; }
    wxWindow* GetContentWindow() const { return content_; }

private:
    wxString   key_;
protected:
    wxWindow* content_ = nullptr;
    wxBoxSizer* contentSizer_ = nullptr;

};

#endif
