// just using the wxWidgets grid example code for now
class wxGrid;

class GridApp : public wxApp
{
public:
    bool OnInit();
};

class GridFrame : public wxFrame
{
public:
    GridFrame();
    ~GridFrame();

    wxLog* mLogOld;

private:
    void OnStart(wxCommandEvent& /*inEvent*/);

    wxGrid* mGrid;
    wxTextCtrl* mLogWin;
    wxButton* mStartButton;
    wxLogTextCtrl* mLogger;
};
