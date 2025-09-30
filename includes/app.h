#ifndef APP_H
#define APP_H

#include <wx/wx.h>
#include <Susi4.h>

class MainApp : public wxApp {
public:
    MainApp() {
        SusiLibInitialize();
    }
    ~MainApp() {
        SusiLibUninitialize();
    }
    bool OnInit() override;
};

#endif