#define WXINTL_NO_GETTEXT_MACRO // pybind11 has a _ function, which this macro breaks

#include <wx/wxprec.h>

#include <wx/grid.h>
#include <iostream>

#include "dexpy_grid.h"

#include <pybind11/pybind11.h>
#include <pybind11/eval.h> // starting in pybind 1.9

void run_filtrate_example();
void init_python();

IMPLEMENT_APP(GridApp)

bool GridApp::OnInit()
{
    GridFrame *frame = new GridFrame;
    frame->Show(true);

    return true;
}

void OnStart(wxCommandEvent& /*inEvent*/)
{
    namespace pb = pybind11;
    init_python();
    try {
        run_filtrate_example();
    } catch (pb::error_already_set& e)
    {
        wxLogMessage(wxT("%s"), e.what());
    } catch (std::exception& e) {
        wxLogMessage(wxT("%s"), e.what());
    }
}


GridFrame::GridFrame() : wxFrame((wxFrame *)nullptr, wxID_ANY, wxT("dexpy python embedding example"), wxDefaultPosition, wxDefaultSize)
{
    grid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize( 400, 300));

    int gridW = 600, gridH = 300;
    int logW = gridW, logH = 300;

    logWin = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint( 0, gridH + 20 ), wxSize( logW, logH ), wxTE_MULTILINE);
    startButton = new wxButton(this, wxID_ANY, "Start");
    startButton->Bind(wxEVT_BUTTON, &OnStart);

    logger = new wxLogTextCtrl(logWin);
    m_logOld = wxLog::SetActiveTarget(logger);
    wxLog::DisableTimestamp();

    grid->CreateGrid(0, 0);

    grid->AppendRows(100);
    grid->AppendCols(100);

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* logSizer = new wxBoxSizer(wxHORIZONTAL);
    logSizer->Add(startButton, 0, wxEXPAND);
    logSizer->Add(logWin, 0, wxEXPAND);

    topSizer->Add(logSizer, 0, wxEXPAND);
    topSizer->Add(grid, 1, wxEXPAND);

    SetSizerAndFit(topSizer);

    Centre();
}

GridFrame::~GridFrame()
{
    delete wxLog::SetActiveTarget(m_logOld);
}
