#ifndef MAIN_FRAME_H
#define MAIN_FRAME_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/treectrl.h>
#include <unordered_map>

wxDECLARE_EVENT(EVT_MF_NEW_FRAME, wxCommandEvent);

class MainFrame : public wxFrame {
public:
    MainFrame();
    ~MainFrame() override;

private:
    wxAuiManager aui_{ this };

    wxAuiNotebook* centerBook_ = nullptr;
    wxTreeCtrl* nav_ = nullptr;
    wxTextCtrl* log_ = nullptr;

    using PageFactoryFn = wxWindow * (*)(wxWindow* parent);

    std::unordered_map<wxString, wxWindow*> openPages_;
    std::unordered_map<wxString, PageFactoryFn> pageFactories_;

    void CreateMenuBar();
    void CreateLayout();
    void InitNav();
    void BindEvent();
    void RegistPage();

    wxWindow* MakePlaceholderPage(const wxString& title, const wxString& desc);
    
    void AddPage(const wxString& key, wxWindow* page);
    int  FindPageIndex(wxWindow* page) const;
    void Log(const wxString& msg);

    void RegisterPageFactory(const wxString& key, PageFactoryFn fn);
    void OpenByKey(const wxString& key);

    void OnQuit(wxCommandEvent&);
    void OnAbout(wxCommandEvent&);
    void OnTreeActivate(wxTreeEvent&);
    void OnCloseCurrent(wxCommandEvent&);
    void OnToggleNav(wxCommandEvent&);
    void OnToggleLog(wxCommandEvent&);
    void OnTabClose(wxAuiNotebookEvent& e);

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