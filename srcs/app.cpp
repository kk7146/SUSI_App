#include <wx/wx.h>

#include "app.h"
#include "main_frame.h"
#include <Susi4.h>

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    uint32_t st = SusiLibInitialize();
    if (st != SUSI_STATUS_SUCCESS) {
        wxMessageBox(wxString::Format("SusiLibInitialize() failed: %08X", st),
            "SUSI init error", wxOK | wxICON_ERROR);
        return false;
    }
    auto* f = new MainFrame(nullptr);
    f->Show();
    return true;
}
int MyApp::OnExit() {
    SusiLibUninitialize();
    return wxApp::OnExit();
}