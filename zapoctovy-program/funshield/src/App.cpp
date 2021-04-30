// app.cpp: Definuje vstupní bod pro aplikaci.

#include "funshield.h"
using Funshield_ = fs::Funshield_;
#include <sstream>
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
    void DrawButtons(wxDC& dc, Funshield_& shield);
    void DrawSegmDisplay(wxDC& dc, Funshield_& shield);
    void DrawDigits(wxDC& dc, Funshield_& shield);
    void DrawGlyph(wxDC& dc, int pos, byte glyph);
    void DrawButtonBackground(wxDC& dc);
private:
    SimulatorFrame* parent_;

    const wxPoint topLedPosition{ 300, 280 };
    const int ledRadius{ 10 };
    const int ledDistance{ 30 };
    const wxPoint leftButtonPosition{ 30, 280 };
    const int buttonRadius{15};
    const int buttonDistance{ 50 };
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
    // Number of loop cycles for which digit will be visible
    // after it is turned off. This eliminates flickering when
    // using multiplexing.
    const int displayDuration = Funshield_::digit_count;
    int loopsToWait[Funshield_::digit_count]{ 0 };
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
    bool buttonPressed[Funshield_::button_count]{ false };
    // Mapping of keyboard keys to arduino buttons
    const wxKeyCode buttonKeys[Funshield_::button_count] { 'A',  'S', 'D' };
    unsigned long lastTime;
    int loopsInSecond;
};

// Main app class
class FunshieldSimulatorApp : public wxApp
{
    SimulatorFrame* frame;   
    virtual bool OnInit()
    {
        // Main window
        // Must be allocated on heap, library manages its destruction
        frame = new SimulatorFrame("Funshield simulator", wxDefaultPosition, wxSize{560, 500});
        frame->Show();
        return true;
    }

};

// implements "main" function
wxIMPLEMENT_APP(FunshieldSimulatorApp);

DrawingPanel::DrawingPanel(SimulatorFrame* parent) : wxPanel(parent), parent_(parent)
{
    SetBackgroundColour(wxColour{ 90, 120, 223 });
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
    Bind(wxEVT_KEY_DOWN, &SimulatorFrame::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &SimulatorFrame::OnKeyUp, this);

    SetStatusText("Loop is running");
}

bool SimulatorFrame::isButtonPressed(int button)
{
    return buttonPressed[button];
}

void SimulatorFrame::simulationLoop(wxIdleEvent& evt)
{
    // No need to draw funshield state if the window is minimized
    if (minimized) return;
    loop();

    wxClientDC dc(panel);
    panel->DrawLEDs(dc, Funshield_::getInstance());
    panel->DrawDigits(dc, Funshield_::getInstance());
    panel->DrawButtons(dc, Funshield_::getInstance());
    if (lastTime + 1000 > Funshield_::getInstance().millis()) loopsInSecond++;
    else {
        lastTime = Funshield_::getInstance().millis();
        std::stringstream statusText{};
        statusText << loopsInSecond;
        statusText << " loops in second";
        SetStatusText(statusText.str());
        loopsInSecond = 0;
    }
    evt.RequestMore();

}

void SimulatorFrame::OnKeyDown(wxKeyEvent& evt)
{
    int key = evt.GetKeyCode();
    // Check if keycode matches any button
    for (int i = 0; i < Funshield_::button_count; i++)
    {
        if (key == buttonKeys[i]) 
        { 
            buttonPressed[i] = true;
            Funshield_::getInstance().setButton(i, true);

        }
    }
}

void SimulatorFrame::OnKeyUp(wxKeyEvent& evt)
{
    int key = evt.GetKeyCode();
    // Check if keycode matches any button
    for (int i = 0; i < Funshield_::button_count; i++)
    {
        if (key == buttonKeys[i])
        {
            buttonPressed[i] = false;
            Funshield_::getInstance().setButton(i, false);
        }
    }

}

void SimulatorFrame::OnMinimize(wxIconizeEvent& evt)
{
    minimized = evt.IsIconized();
}

void DrawingPanel::OnPaint(wxPaintEvent& evt)
{
    wxPaintDC dc(this); 
    DrawLEDs(dc, Funshield_::getInstance());
    DrawButtonBackground(dc);
    DrawButtons(dc, Funshield_::getInstance());
    DrawSegmDisplay(dc, Funshield_::getInstance());
}

void DrawingPanel::DrawLEDs(wxDC& dc, Funshield_& shield)
{
    for (int i = 0; i < shield.led_count; i++)
    {
        dc.SetBrush(shield.isLedOn(i) ? *wxRED_BRUSH : *wxGREY_BRUSH);
        dc.DrawCircle(topLedPosition.x, topLedPosition.y + ((shield.led_count - i - 1) * ledDistance), ledRadius);
    }
}

void DrawingPanel::DrawButtons(wxDC& dc, Funshield_& shield)
{
    for (int i = 0; i < shield.button_count; i++)
    {
        dc.SetBrush(parent_->isButtonPressed(i) ? *wxGREEN_BRUSH : *wxBLACK_BRUSH);
        dc.DrawCircle(leftButtonPosition.x + ((shield.button_count - i - 1) * buttonDistance), leftButtonPosition.y, buttonRadius);
    }
}

void DrawingPanel::DrawButtonBackground(wxDC& dc)
{
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    for (int i = 0; i < Funshield_::button_count; i++)
    {
        dc.DrawRectangle(leftButtonPosition.x + (i * buttonDistance) - buttonRadius - 5, leftButtonPosition.y - buttonRadius - 5, 2 * buttonRadius + 10, 2 * buttonRadius + 10);
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
    byte posBitmask = shield.getPositionBitmask();
    for (int i = 0; i < 4; i++)
    {
        bool digitIsOn = posBitmask & (1 << i);
        if (digitIsOn) loopsToWait[i] = displayDuration;
        if (digitIsOn || loopsToWait[i] == 0)
        {
            DrawGlyph(dc, i, shield.getGlyph(i));
        }
        else loopsToWait[i]--;
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