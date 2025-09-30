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

static wxWindow* CreateGPIOPage(wxWindow* parent, GPIO::Bank bank) { return new GpioPage(parent, bank); }

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Control Hub", wxDefaultPosition, wxSize(1080, 700)) {
    SetMinSize(wxSize(900, 600));
    CreateMenuBar();
    CreateStatusBar();
    SetStatusText("Ready");

    CreateLayout();
    InitNav();

    BindEvent();

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
    const wxTreeItemId GPIO = nav_->AppendItem(root, "GPIO");

    auto banks = SUSI::GPIO::FindAvailableBanks(SUSI::GPIO::PinFilter::Any);
    for (auto& b : banks) {
        auto label = wxString::Format("Bank %d", b.index);
        auto id = nav_->AppendItem(GPIO, label);
        nav_->SetItemData(id, new NodeData(NodeData::GPIO_BANK, b));
    }

    auto fanId = nav_->AppendItem(root, "Fan");
    nav_->SetItemData(fanId, new NodeData(NodeData::FAN_ROOT));
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

void MainFrame::AddPage(const wxString& key, wxWindow* page) {
    auto it = openPages_.find(key);
    if (it != openPages_.end()) {
        const int idx = FindPageIndex(it->second);
        if (idx >= 0)
            centerBook_->SetSelection(idx);
        if (page && page != it->second)
            page->Destroy();
        return;
    }

    if (!page)
        page = MakePlaceholderPage(key, key + " control page");
    centerBook_->AddPage(page, key, true);
    openPages_.emplace(key, page);

    Log(wxString::Format("[INFO] Opened page: %s\n", key));
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

    switch (nd->type) {
    case NodeData::GPIO_BANK: {
        const wxString key = wxString::Format("gpio:bank:%d", nd->bank.index);
        const wxString tab = wxString::Format("GPIO %d", nd->bank.index);
        ShowOrCreatePage(key, tab, [&, bank = nd->bank](wxWindow* parent) {
            return new GpioPage(parent, bank);
            });
        break;
    }
    case NodeData::FAN_ROOT: {
        ShowOrCreatePage("fan", "Fan", [&](wxWindow* parent) {
            return MakePlaceholderPage("Fan", "Fan control page");
            });
        break;
    }
    }
}

void MainFrame::OnTabClose(wxAuiNotebookEvent& e) {
    const int idx = e.GetSelection();
    if (idx == wxNOT_FOUND) return;
    const wxString key = centerBook_->GetPageText(idx);
    centerBook_->DeletePage(idx);
    Log(wxString::Format("[INFO] Closed page request: %s\n", key));
}

void MainFrame::OnCloseCurrent(wxCommandEvent&) {
    const int sel = centerBook_->GetSelection();
    if (sel == wxNOT_FOUND) return;
    const wxString key = centerBook_->GetPageText(sel);
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

void MainFrame::RegisterPage(const wxString& key, wxWindow* page) {
    openPages_[key] = page;
    page->Bind(wxEVT_DESTROY, [this, key](wxWindowDestroyEvent&) {
        openPages_.erase(key);
        Log(wxString::Format("[INFO] Page destroyed: %s\n", key));
        });
}

void MainFrame::ShowOrCreatePage(const wxString& key,
    const wxString& tabLabel,
    std::function<wxWindow* (wxWindow*)> factory) {
    if (auto it = openPages_.find(key); it != openPages_.end()) {
        const int idx = FindPageIndex(it->second);
        if (idx >= 0) centerBook_->SetSelection(idx);
        return;
    }
    wxWindow* page = factory(centerBook_);
    centerBook_->AddPage(page, tabLabel, true);
    RegisterPage(key, page);
    Log(wxString::Format("[INFO] Opened page: %s\n", key));
}

bool MainFrame::ClosePageByKey(const wxString& key) {
    auto it = openPages_.find(key);
    if (it == openPages_.end()) return false;
    const int idx = FindPageIndex(it->second);
    if (idx < 0) return false;
    const bool ok = centerBook_->DeletePage(idx);
    return ok;
}


wxDEFINE_EVENT(EVT_MF_NEW_FRAME, wxCommandEvent);
