// app.cpp: Definuje vstupní bod pro aplikaci.
// Rename constant INPUT because INPUT is used by wxWidgets on Windows (defined in winuser.h)
#define INPUT pin_mode_INPUT
#include "FunShield.h"
#undef INPUT
// Include wxWidgets library headers
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class SimulatorFrame : public wxFrame
{
public:
    SimulatorFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    bool renderLoopRunning = false;
    void simulationLoop(wxIdleEvent& evt);
};

// Main app class
class FunshieldSimulatorApp : public wxApp
{
    SimulatorFrame* frame;   
    virtual bool OnInit()
    {
        // Main window
        // Must be allocated on heap, library manages its destruction
        frame = new SimulatorFrame("Funshield simulator", wxDefaultPosition, wxDefaultSize);
        frame->Show();
        return true;
    }

};

// implements "main" function
wxIMPLEMENT_APP(FunshieldSimulatorApp);

SimulatorFrame::SimulatorFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    CreateStatusBar();
    Bind(wxEVT_IDLE, &SimulatorFrame::simulationLoop, this);
    setup();
}

void SimulatorFrame::simulationLoop(wxIdleEvent& evt)
{
    loop();
    std::string led_str{};
    for (int i = 0; i < Funshield_::getInstance().led_count; i++)
    {
        if (Funshield_::getInstance().isLedOn(i)) led_str += 'O'; else led_str += '.';
    }
    SetStatusText(led_str);
    evt.RequestMore();

}