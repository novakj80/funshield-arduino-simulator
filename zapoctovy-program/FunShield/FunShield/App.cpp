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
class SimulatorFrame;
class DrawingPanel : public wxPanel
{
public:
    DrawingPanel(SimulatorFrame* parent);
    void OnPaint(wxPaintEvent& evt);
    void DrawLEDs(wxDC& dc, Funshield_& shield);
    //void DrawButtons(wxDC& dc);
private:
    SimulatorFrame* parent_;

    const wxPoint topLedPosition{ 50, 50 };
    const int ledSize{ 10 };
    const int ledDistance{ 50 };
    const wxPoint leftButtonPostiton{ 200, 200 };
};

class SimulatorFrame : public wxFrame
{
public:
    SimulatorFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    bool isButtonPressed(int button);
private:
    void simulationLoop(wxIdleEvent& evt);
    void OnKeyDown(wxKeyEvent& evt);
    void OnKeyUp(wxKeyEvent& evt);
    void OnMinimize(wxIconizeEvent& evt);
    DrawingPanel* panel;

    bool renderLoopRunning = true;
    bool minimized = false;    
    bool buttonPressed[Funshield_::button_count];
    // Mapping of keyboard keys to arduino buttons
    const wxKeyCode buttonKeys[Funshield_::button_count] { wxKeyCode::WXK_NUMPAD1, wxKeyCode::WXK_NUMPAD2, wxKeyCode::WXK_NUMPAD3 };
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

DrawingPanel::DrawingPanel(SimulatorFrame* parent) : wxPanel(parent), parent_(parent)
{
}

SimulatorFrame::SimulatorFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    CreateStatusBar();
    // Bind event handlers
    Bind(wxEVT_IDLE, &SimulatorFrame::simulationLoop, this);
    Bind(wxEVT_ICONIZE, &SimulatorFrame::OnMinimize, this);
    setup();

    panel = new DrawingPanel(this);
    panel->Bind(wxEVT_PAINT, &DrawingPanel::OnPaint, panel);
    panel->Bind(wxEVT_KEY_DOWN, &SimulatorFrame::OnKeyDown, this);
    panel->Bind(wxEVT_KEY_UP, &SimulatorFrame::OnKeyUp, this);
}

void SimulatorFrame::simulationLoop(wxIdleEvent& evt)
{
    // No need to draw funshield state if the window is minimized
    if (minimized) return;
    loop();
    std::string led_str{};
    for (int i = 0; i < Funshield_::getInstance().led_count; i++)
    {
        if (Funshield_::getInstance().isLedOn(i)) led_str += 'O'; else led_str += '.';
    }
    SetStatusText(led_str);

    wxClientDC dc(panel);
    panel->DrawLEDs(dc, Funshield_::getInstance());
    evt.RequestMore();

}

void SimulatorFrame::OnKeyDown(wxKeyEvent& evt)
{
    int key = evt.GetKeyCode();
    // Check if keycode matches any button
    for (int i = 0; i < Funshield_::button_count; i++)
    {
        if (key == buttonKeys[i]) Funshield_::getInstance().setButton(i, true);
    }
}

void SimulatorFrame::OnKeyUp(wxKeyEvent& evt)
{
    int key = evt.GetKeyCode();
    // Check if keycode matches any button
    for (int i = 0; i < Funshield_::button_count; i++)
    {
        if (key == buttonKeys[i]) Funshield_::getInstance().setButton(i, false);
    }
}

void SimulatorFrame::OnMinimize(wxIconizeEvent& evt)
{
    minimized = evt.IsIconized();
}

void DrawingPanel::OnPaint(wxPaintEvent& evt)
{
    wxPaintDC dc(this); 
    //dc.SetBrush(*wxGREEN_BRUSH);
    //dc.DrawCircle({ 100,100 }, 30);
    DrawLEDs(dc, Funshield_::getInstance());
}

void DrawingPanel::DrawLEDs(wxDC& dc, Funshield_& shield)
{
    for (int i = 0; i < shield.led_count; i++)
    {
        dc.SetBrush(shield.isLedOn(i) ? *wxGREEN_BRUSH : *wxGREY_BRUSH);
        dc.DrawCircle(topLedPosition.x, topLedPosition.y + ((shield.led_count - i - 1) * ledDistance), ledSize);
    }
}

