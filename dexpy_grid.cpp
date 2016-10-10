#define WXINTL_NO_GETTEXT_MACRO // pybind11 has a _ function, which this macro breaks

#include <wx/wxprec.h>

#include <wx/grid.h>
#include <iostream>

#include "dexpy_grid.h"

#include <pybind11/pybind11.h>

void init_python()
{
    static bool isInitialized = false;

    if (!isInitialized) {
        _putenv_s("PYTHONPATH", "./dexpy_env");
        Py_Initialize();
        isInitialized = true;
    }
}


std::vector<std::vector<std::string>> run_filtrate_example();
void init_python();

IMPLEMENT_APP(GridApp)

bool GridApp::OnInit()
{
    GridFrame *frame = new GridFrame;
    frame->Show(true);

    return true;
}

GridFrame::GridFrame() : wxFrame((wxFrame *)nullptr, wxID_ANY, wxT("dexpy python embedding example"), wxDefaultPosition, wxDefaultSize)
{
    mGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize( 400, 300));

    int gridW = 600, gridH = 300;
    int logW = gridW, logH = 300;

    mLogWin = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint( 0, gridH + 20 ), wxSize( logW, logH ), wxTE_MULTILINE);
    mStartButton = new wxButton(this, wxID_ANY, "Start");
    mStartButton->Bind(wxEVT_BUTTON, &GridFrame::OnStart, this);

    mLogger = new wxLogTextCtrl(mLogWin);
    mLogOld = wxLog::SetActiveTarget(mLogger);
    wxLog::DisableTimestamp();

    mGrid->CreateGrid(0, 0);

    mGrid->AppendRows(100);
    mGrid->AppendCols(100);

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* logSizer = new wxBoxSizer(wxHORIZONTAL);
    logSizer->Add(mStartButton, 0, wxEXPAND);
    logSizer->Add(mLogWin, 0, wxEXPAND);

    topSizer->Add(logSizer, 0, wxEXPAND);
    topSizer->Add(mGrid, 1, wxEXPAND);

    SetSizerAndFit(topSizer);

    Centre();
}

void GridFrame::OnStart(wxCommandEvent& /*inEvent*/)
{
    namespace pb = pybind11;
    init_python();
    try
    {
        std::vector<std::vector<std::string>> results = run_filtrate_example();
        for (size_t r = 0; r < results.size(); ++r)
            for (size_t c = 0; c < results[r].size(); ++c)
                mGrid->SetCellValue(r, c, results[r][c]);
    } catch (pb::error_already_set& e)
    {
        wxLogMessage(wxT("%s"), e.what());
    } catch (std::exception& e) {
        wxLogMessage(wxT("%s"), e.what());
    }
}


GridFrame::~GridFrame()
{
    delete wxLog::SetActiveTarget(mLogOld);
}
