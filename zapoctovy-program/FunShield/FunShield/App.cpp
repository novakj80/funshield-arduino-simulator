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
    void DrawSegmDisplay(wxDC& dc, Funshield_& shield);
    void DrawDigits(wxDC& dc, Funshield_& shield);
    void DrawGlyph(wxDC& dc, int pos, byte glyph);
private:
    SimulatorFrame* parent_;

    const wxPoint topLedPosition{ 300, 280 };
    const int ledSize{ 10 };
    const int ledDistance{ 30 };
    const wxPoint leftButtonPostiton{ 200, 200 };
    const wxRect digitSegmentRectangles[7]{ 
        {10,0,60,10},
        {70,10,10,65},
        {70,85,10,65},
        {10,150,60,10},
        {0,85,10,65},
        {0,10,10,65},
        {10,75,60,10},
    };
    /* Make digits bolder
    const wxRect digitSegmentRectangles[7]{ 
        {14,0,52,14},
        {66,14,14,59},
        {66,87,14,59},
        {14,146,52,14},
        {0,87,14,59},
        {0,14,14,59},
        {14,75,52,14},
    };
    */
    const wxPoint segmDisplayTopLeft{30, 30};
    const wxBrush* segmOnBrush = wxRED_BRUSH;
    const wxBrush* segmOffBrush = wxTheBrushList->FindOrCreateBrush({48, 48, 48});
    const int segmDotRadius = 12;
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
        frame = new SimulatorFrame("Funshield simulator", wxDefaultPosition, wxSize{560, 600});
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
    : wxFrame(NULL, wxID_ANY, title, pos, size, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
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
    panel->DrawDigits(dc, Funshield_::getInstance());
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
    DrawSegmDisplay(dc, Funshield_::getInstance());
}

void DrawingPanel::DrawLEDs(wxDC& dc, Funshield_& shield)
{
    for (int i = 0; i < shield.led_count; i++)
    {
        dc.SetBrush(shield.isLedOn(i) ? *wxGREEN_BRUSH : *wxGREY_BRUSH);
        dc.DrawCircle(topLedPosition.x, topLedPosition.y + ((shield.led_count - i - 1) * ledDistance), ledSize);
    }
}

void DrawingPanel::DrawSegmDisplay(wxDC& dc, Funshield_& shield)
{
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawRectangle(segmDisplayTopLeft.x - 10, segmDisplayTopLeft.y - 10, 500, 180);
    DrawDigits(dc, shield);
}
void DrawingPanel::DrawDigits(wxDC& dc, Funshield_& shield)
{
    for (int i = 0; i < 4; i++)
    {
        DrawGlyph(dc, i, shield.getGlyph(i));
    }
}

void DrawingPanel::DrawGlyph(wxDC& dc, int pos, byte glyph)
{

    for (int i = 0; i < 7; i++)
    {
        dc.SetBrush(glyph & (1 << i) ? *segmOffBrush : *segmOnBrush);
        dc.DrawRectangle(
            digitSegmentRectangles[i].x + pos * (80 + 44) + segmDisplayTopLeft.x,
            digitSegmentRectangles[i].y + segmDisplayTopLeft.y,
            digitSegmentRectangles[i].width,
            digitSegmentRectangles[i].height);
    }
    dc.SetBrush(glyph & 0b1000'0000 ? *segmOffBrush : *segmOnBrush);
    dc.DrawCircle(segmDisplayTopLeft.x + 80 + pos *(80 + 44) + 15, segmDisplayTopLeft.y + 151, segmDotRadius);
}