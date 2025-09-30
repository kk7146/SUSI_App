#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/toolbar.h>
#include <vector>
#include <string>
#include <cstdio>

#include "main_frame.h"
#include "gpio_page.h"
#include "util.h"

using namespace SUSI;

static wxWindow* CreateGPIOPage_Factory(wxWindow* parent, const void* args) {
    auto* bank = static_cast<const SUSI::GPIO::Bank*>(args);
    return new GpioPage(parent, *bank);
}

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Control Hub", wxDefaultPosition, wxSize(1080, 700)) {
    SetMinSize(wxSize(900, 600));
    CreateMenuBar();
    CreateStatusBar();
    SetStatusText("Ready");

    CreateLayout();
    InitNav();

    BindEvent();

    RegisterPageFactory_("GPIO", &CreateGPIOPage_Factory);

    aui_.Update();
}

MainFrame::~MainFrame() {
    aui_.UnInit();
}

void MainFrame::CreateMenuBar() {
    auto* menubar = new wxMenuBar();

    auto* fileMenu = new wxMenu();
    fileMenu->Append(ID_Quit, "&Exit\tAlt-F4", "Close the application");

    auto* viewMenu = new wxMenu();
    viewMenu->AppendCheckItem(ID_View_ToggleNav, "Show &Navigation\tCtrl+1", "Toggle left navigation");
    viewMenu->AppendCheckItem(ID_View_ToggleLog, "Show &Log\tCtrl+2", "Toggle bottom log");
    viewMenu->Check(ID_View_ToggleNav, true);
    viewMenu->Check(ID_View_ToggleLog, true);

    auto* ctrlMenu = new wxMenu();
    ctrlMenu->Append(ID_CloseCurrentPage, "&Close Current Tab\tCtrl+W");

    auto* helpMenu = new wxMenu();
    helpMenu->Append(ID_About, "&About");

    menubar->Append(fileMenu, "&File");
    menubar->Append(viewMenu, "&View");
    menubar->Append(ctrlMenu, "&Controllers");
    menubar->Append(helpMenu, "&Help");

    SetMenuBar(menubar);
}

void MainFrame::CreateLayout() {
    centerBook_ = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxAUI_NB_CLOSE_ON_ACTIVE_TAB | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_SPLIT | wxAUI_NB_SCROLL_BUTTONS);

    nav_ = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);

    log_ = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);

    aui_.AddPane(nav_, wxAuiPaneInfo()
        .Name("nav").Caption("Controllers")
        .Left().Layer(1).BestSize(240, -1).MinSize(200, -1)
        .CloseButton(false).MaximizeButton(true));

    aui_.AddPane(centerBook_, wxAuiPaneInfo()
        .Name("center").Layer(1).CenterPane());

    aui_.AddPane(log_, wxAuiPaneInfo()
        .Name("log").Caption("Log")
        .Bottom().Layer(1).BestSize(-1, 180).MinSize(-1, 120)
        .CloseButton(false).MaximizeButton(true));
}

void MainFrame::InitNav() {
    const wxTreeItemId root = nav_->AddRoot("ROOT");
    const wxTreeItemId gpio = nav_->AppendItem(root, "GPIO",
        -1, -1, new NodeData(NodeData::Category, "GPIO"));

    auto banks = SUSI::GPIO::FindAvailableBanks(SUSI::GPIO::PinFilter::Any);
    for (const auto& b : banks) {
        const wxString label = wxString::Format("Bank %d", b.index);
        nav_->AppendItem(gpio, label, -1, -1, new NodeData(NodeData::Bank, "GPIO", b));
    }
    nav_->ExpandAll();
}

void MainFrame::BindEvent() {
    Bind(wxEVT_MENU, &MainFrame::OnQuit, this, ID_Quit);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, ID_About);
    Bind(wxEVT_MENU, &MainFrame::OnCloseCurrent, this, ID_CloseCurrentPage);
    Bind(wxEVT_MENU, &MainFrame::OnToggleNav, this, ID_View_ToggleNav);
    Bind(wxEVT_MENU, &MainFrame::OnToggleLog, this, ID_View_ToggleLog);
    centerBook_->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &MainFrame::OnTabClose, this);
    if (nav_) {
        nav_->Bind(wxEVT_TREE_ITEM_ACTIVATED,
            &MainFrame::OnTreeActivate, this);
    }
}

wxWindow* MainFrame::MakePlaceholderPage(const wxString& title, const wxString& desc) {
    auto* panel = new wxPanel(centerBook_);
    auto* vbox = new wxBoxSizer(wxVERTICAL);

    auto* t = new wxStaticText(panel, wxID_ANY, title);
    t->SetFont(t->GetFont().Bold().Larger());
    auto* d = new wxStaticText(panel, wxID_ANY, desc);

    vbox->AddSpacer(12);
    vbox->Add(t, 0, wxLEFT | wxRIGHT, 12);
    vbox->AddSpacer(6);
    vbox->Add(d, 0, wxLEFT | wxRIGHT | wxBOTTOM, 12);
    vbox->AddStretchSpacer(1);

    panel->SetSizer(vbox);
    return panel;
}

void MainFrame::AddOrFocusPageId_(const wxString& pageId, const wxString& tabTitle, wxWindow* page) {
    auto it = openPages_.find(pageId);
    if (it != openPages_.end()) {
        for (size_t i = 0; i < centerBook_->GetPageCount(); ++i)
            if (centerBook_->GetPage(i) == it->second) { centerBook_->SetSelection(i); return; }
        return;
    }
    centerBook_->AddPage(page, tabTitle, true);
    openPages_.emplace(pageId, page);
}

int MainFrame::FindPageIndex(wxWindow* page) const {
    const size_t cnt = centerBook_->GetPageCount();
    for (size_t i = 0; i < cnt; ++i)
        if (centerBook_->GetPage(i) == page)
            return static_cast<int>(i);
    return -1;
}

void MainFrame::Log(const wxString& msg) {
    if (!log_) return;
    log_->AppendText(msg);
    log_->ShowPosition(log_->GetLastPosition());
}

void MainFrame::OnQuit(wxCommandEvent&) { Close(true); }

void MainFrame::OnAbout(wxCommandEvent&) {}

void MainFrame::OnTreeActivate(wxTreeEvent& e) {
    auto* nd = dynamic_cast<NodeData*>(nav_->GetItemData(e.GetItem()));
    if (!nd) return;

    if (nd->kind == NodeData::Category && nd->key == "GPIO") {
        return;
    }

    if (nd->kind == NodeData::Bank && nd->key == "GPIO") {
        const wxString pageId = wxString::Format(" %d", nd->bank.index);
        const wxString tabTitle = wxString::Format("Bank %d", nd->bank.index);
        auto it = pageFactories_.find("GPIO");
        if (it == pageFactories_.end() || !it->second)
            return;
        wxWindow* page = it->second(centerBook_, &nd->bank);
        AddOrFocusPageId_(pageId, tabTitle, page);
        Log(wxString::Format("[INFO] Opened page: %s\n", pageId));
    }
}

void MainFrame::OnTabClose(wxAuiNotebookEvent& e) {
    int idx = e.GetSelection();
    if (idx != wxNOT_FOUND) {
        const wxString pageId = centerBook_->GetPageText(idx);
        openPages_.erase(pageId);
        Log(wxString::Format("[INFO] Closed page: %s\n", pageId));
    }
}


void MainFrame::OnCloseCurrent(wxCommandEvent&) {
    const int sel = centerBook_->GetSelection();
    if (sel == wxNOT_FOUND)
        return;

    const wxString key = centerBook_->GetPageText(sel);
    auto it = openPages_.find(key);
    if (it != openPages_.end())
        openPages_.erase(it);

    centerBook_->DeletePage(sel);
    Log(wxString::Format("[INFO] Closed page: %s\n", key));
}

void MainFrame::OnToggleNav(wxCommandEvent&) {
    auto& pane = aui_.GetPane("nav");
    pane.Show(!pane.IsShown());
    aui_.Update();
}

void MainFrame::OnToggleLog(wxCommandEvent&) {
    auto& pane = aui_.GetPane("log");
    pane.Show(!pane.IsShown());
    aui_.Update();
}

wxDEFINE_EVENT(EVT_MF_NEW_FRAME, wxCommandEvent);
