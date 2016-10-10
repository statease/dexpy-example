// just using the wxWidgets grid example code for now
class wxGrid;

class GridApp : public wxApp
{
public:
    bool OnInit();
};

class GridFrame : public wxFrame
{
    wxGrid* grid;
    wxTextCtrl* logWin;
    wxButton* startButton;
    wxLogTextCtrl* logger;

public:
    GridFrame();
    ~GridFrame();

    wxLog *m_logOld;
};
