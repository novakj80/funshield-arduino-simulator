// app.cpp: Definuje vstupní bod pro aplikaci.

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

};

// Main app class
class FunshieldSimulatorApp : public wxApp
{
    wxFrame* frame = new wxFrame(NULL, wxID_ANY, "Funshield");
    virtual bool OnInit()
    {
        frame->Show();
        return false;
    }

};

wxIMPLEMENT_APP(FunshieldSimulatorApp);
