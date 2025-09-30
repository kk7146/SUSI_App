#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/treectrl.h>
#include <unordered_map>

#include <util.h>

wxDECLARE_EVENT(EVT_MF_NEW_FRAME, wxCommandEvent);

struct NodeData : public wxTreeItemData {
    enum Type { GPIO_BANK, FAN_ROOT } type;
    SUSI::GPIO::Bank bank{};
    explicit NodeData(Type t, const SUSI::GPIO::Bank& b = {}) : type(t), bank(b) {}
};

class MainFrame : public wxFrame {
public:
    MainFrame();
    ~MainFrame() override;

private:
    wxAuiManager aui_{ this };

    wxAuiNotebook* centerBook_ = nullptr;
    wxTreeCtrl* nav_ = nullptr;
    wxTextCtrl* log_ = nullptr;

    std::unordered_map<wxString, wxWindow*> openPages_;

    void CreateMenuBar();
    void CreateLayout();
    void InitNav();
    void BindEvent();

    wxWindow* MakePlaceholderPage(const wxString& title, const wxString& desc);
    
    void AddPage(const wxString& key, wxWindow* page);
    int  FindPageIndex(wxWindow* page) const;
    void Log(const wxString& msg);

    void OnQuit(wxCommandEvent&);
    void OnAbout(wxCommandEvent&);
    void OnTreeActivate(wxTreeEvent&);
    void OnCloseCurrent(wxCommandEvent&);
    void OnToggleNav(wxCommandEvent&);
    void OnToggleLog(wxCommandEvent&);
    void OnTabClose(wxAuiNotebookEvent& e);

    void RegisterPage(const wxString& key, wxWindow* page);
    void ShowOrCreatePage(const wxString& key, const wxString& tabLabel, std::function<wxWindow* (wxWindow*)> factory);
    bool ClosePageByKey(const wxString& key);

    void OpenGPIO();
    void OpenFan();
};

enum {
    ID_View_ToggleNav = wxID_HIGHEST + 1,
    ID_View_ToggleLog,
    ID_CloseCurrentPage,
    ID_Quit,
    ID_About
};

#endif