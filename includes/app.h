#ifndef APP_H
#define APP_H

#include <wx/app.h>
#include <Susi4.h>

#include "main_frame.h"

class MyApp : public wxApp {
public:
    bool OnInit() override;
    int OnExit() override;
};

wxDECLARE_APP(MyApp);

#endif
