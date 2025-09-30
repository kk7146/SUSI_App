#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/toolbar.h>

#include "app.h"
#include "main_frame.h"

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit() {
    if (!wxApp::OnInit()) return false;
    auto* frame = new MainFrame();
    frame->Show(true);
    return true;
}
