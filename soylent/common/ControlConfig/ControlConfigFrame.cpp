// For compilers that supports precompilation , includes "wx/wx.h"
#include "ControlConfigFrame.h"
#include <string>
#include <fstream>
#include <wx\treectrl.h>
#include <wx\listctrl.h>
#include "..\NVThing.h"

void ExpandAll(wxTreeCtrl *aWtc, const wxTreeItemId &aId);

using namespace std;

BEGIN_EVENT_TABLE(ControlConfigFrame, wxFrame)
	EVT_MENU(MENU_FILE_OPEN, ControlConfigFrame::OnMenuFileOpen)
	EVT_MENU(MENU_FILE_SAVE, ControlConfigFrame::OnMenuFileSave)
	EVT_MENU(MENU_FILE_QUIT, ControlConfigFrame::OnMenuFileQuit)
	EVT_MENU(MENU_INFO_ABOUT, ControlConfigFrame::OnMenuInfoAbout)
	EVT_MENU(MENU_EDIT_DELETE, ControlConfigFrame::OnMenuEditDelete)
	EVT_MENU(MENU_EDIT_ADD, ControlConfigFrame::OnMenuEditAdd)
	EVT_LISTBOX_DCLICK(INPUTSOURCES_DBLCLICK, ControlConfigFrame::OnMenuEditAdd)
END_EVENT_TABLE()


ControlConfigFrame::ControlConfigFrame(int xpos, int ypos, int width, int height)
: wxFrame((wxFrame *) NULL, -1, "Control Config", wxPoint(xpos, ypos), wxSize(width, height))
{
	// Create text control.
	/*
	mWtcTextCtrl = new wxTextCtrl(this, -1, wxString("Type some text..."),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
		*/

	// Create tree control.
	/*
    wxTreeCtrl(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxTreeCtrlNameStr)
			   */
	mWtcCommands = new wxTreeCtrl(this);

	/*
    wxListView( wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxT("listctrl") )
	*/

	mWlcInputSources = // new wxListCtrl(this);
		new wxListView(this,
                INPUTSOURCES_DBLCLICK,
                wxDefaultPosition,
                wxDefaultSize,
                wxLC_LIST);

	//mWlcInputSources->SetColumnCount(1);

	// wxLC_REPORT

	// Create menu bar and contents.
	m_pMenuBar = new wxMenuBar();
	// File Menu
	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(MENU_FILE_OPEN, "&Open");
	m_pFileMenu->Append(MENU_FILE_SAVE, "&Save");
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(MENU_FILE_QUIT, "&Quit");
	m_pMenuBar->Append(m_pFileMenu, "&File");
	// Edit menu
	m_pEditMenu = new wxMenu();
	m_pEditMenu->Append(MENU_EDIT_ADD, "&Add\tEnter");
	m_pEditMenu->Append(MENU_EDIT_DELETE, "&Delete\tDEL");
	m_pMenuBar->Append(m_pEditMenu, "&Edit");
	// About menu
	m_pInfoMenu = new wxMenu();
	m_pInfoMenu->Append(MENU_INFO_ABOUT, "&About");
	m_pMenuBar->Append(m_pInfoMenu, "&Info");
	SetMenuBar(m_pMenuBar);

	// Setup status bar.
	CreateStatusBar(3);
	SetStatusText("Ready", 0);
	SetStatusText("Or", 1);
	SetStatusText("Not", 2);

	// Do the sizer thing.
	wxBoxSizer *lWsMain = new wxBoxSizer(wxHORIZONTAL);
//	wxBoxSizer *textSizer = new wxBoxSizer(wxHORIZONTAL);

	lWsMain->Add(mWtcCommands, 1, wxEXPAND);
	lWsMain->Add(mWlcInputSources, 1, wxEXPAND);

	SetSizer(lWsMain);
	SetAutoLayout(TRUE);

	mWsFilename = "";
}

ControlConfigFrame::~ControlConfigFrame()
{
}

void ControlConfigFrame::OnMenuFileOpen(wxCommandEvent &event)
{
	wxFileDialog lWfd(this, "Choose control settings file",
		"", "", "*.txt", 0);
	if (lWfd.ShowModal() == wxID_OK)
	{
		OpenFile(lWfd.GetPath());
	}
}

void ControlConfigFrame::OpenFile(const char *aC)
{
    ifstream lIfs(aC);

	if (lIfs.is_open())
	{
		lIfs >> mNvt;
		lIfs.close();

		SetupTree(mNvt);

		mWsFilename = aC;
	}
}

void ControlConfigFrame::OnMenuFileSave(wxCommandEvent &event)
{
	UpdateNvt(mNvt);
	ofstream lOfs(mWsFilename.c_str());
	lOfs << mNvt;
	lOfs.close();
}
void ControlConfigFrame::OnMenuFileQuit(wxCommandEvent &event)
{
	Close(FALSE);
}
void ControlConfigFrame::OnMenuInfoAbout(wxCommandEvent &event)
{
	wxLogMessage("About Menu Selected");
}

void ControlConfigFrame::OnMenuEditDelete(wxCommandEvent &event)
{
	wxTreeItemId lWtii = mWtcCommands->GetSelection();	
	if (lWtii == mWtcCommands->GetRootItem() || 
		mWtcCommands->GetItemParent(lWtii) == mWtcCommands->GetRootItem())
		return;
	
	mWtcCommands->Delete(lWtii);
}

void ControlConfigFrame::OnMenuEditAdd(wxCommandEvent &event)
{
	wxTreeItemId lWtii = mWtcCommands->GetSelection();

	if (mWtcCommands->GetItemParent(lWtii) != mWtcCommands->GetRootItem())
	{
		wxLogMessage("Current selection in the command tree is not a command!");
	}

    long item = -1;
    while (true)
    {
        item = mWlcInputSources->GetNextItem(item,
                                     wxLIST_NEXT_ALL,
                                     wxLIST_STATE_SELECTED);
        if ( item == -1 )
            break;

		mWtcCommands->AppendItem(lWtii, mWlcInputSources->GetItemText(item));
    }

	mWtcCommands->Refresh();
}

void ControlConfigFrame::SetupTree(NVThing &aNvtControls)
{
	NVThing *lNvt;
	mWtcCommands->DeleteAllItems();
	if (aNvtControls.Find("AvailableCommands", lNvt))
	{
		wxTreeItemId lWtiiRoot = mWtcCommands->AddRoot("Available Commands");
		int lICount;
		if (lNvt->Find("Count", lICount))
		{
			for (int lI = 0; lI < lICount; ++lI)
			{
				const char *lC;
				if (lNvt->Find("Command", lI, lC))
					mWtcCommands->AppendItem(lWtiiRoot, lC);
			}
		}
	}

	// Set the inputsources that are already assigned to commands.
	if (aNvtControls.Find("ActiveCommands", lNvt))
	{
		int lICount;
		if (lNvt->Find("Count", lICount))
		{
			for (int lI = 0; lI < lICount; ++lI)
			{
				const char *lC;
				const char *lCIs;
				if (lNvt->Find("Command", lI, lC) &&
					lNvt->Find("InputSource", lI, lCIs))
				{
					// Find the item in the tree with the same name as lC.
					wxTreeItemId lWtiiRoot = mWtcCommands->GetRootItem();
					long lLCookie;
					wxTreeItemId lWtii = mWtcCommands->GetFirstChild(lWtiiRoot, lLCookie);
					while (lWtii > 0)
					{
						if (mWtcCommands->GetItemText(lWtii) == lC)
						{
							mWtcCommands->AppendItem(lWtii, lCIs);
							break;
						}

						lWtii = mWtcCommands->GetNextChild(lWtiiRoot, lLCookie);
					}

					if (lWtii == 0)
					{
						// Not found.  Add an extra item for this.
						lWtii = mWtcCommands->AppendItem(lWtiiRoot, lC);
						mWtcCommands->AppendItem(lWtii, lCIs);
					}
				}
			}
		}
	}


	// List the Input Sources.
	mWlcInputSources->ClearAll();
	mWlcInputSources->SetColumnWidth(-1, -1);
	//mWlcInputSources->SetColumnCount(1);
	if (aNvtControls.Find("InputSources", lNvt))
	{
		int lICount;
		if (lNvt->Find("Count", lICount))
		{
			for (int lI = 0; lI < lICount; ++lI)
			{
				const char *lC;
				if (lNvt->Find("InputSource", lI, lC))
					mWlcInputSources->InsertItem(lI, lC);
			}
		}
	}

	// Expand the tree.
	ExpandAll(mWtcCommands, mWtcCommands->GetRootItem());

}

void ExpandAll(wxTreeCtrl *aWtc, const wxTreeItemId &aId)
{
	aWtc->Expand(aId);

	long lLCookie;
	wxTreeItemId lWtii = aWtc->GetFirstChild(aId, lLCookie);
	while (lWtii > 0)
	{
		ExpandAll(aWtc, lWtii);

		lWtii = aWtc->GetNextChild(aId, lLCookie);
	}
}

void ControlConfigFrame::UpdateNvt(NVThing &aNvt)
{
	NVThing *lNvt = new NVThing;

	wxTreeItemId lWtiiRoot = mWtcCommands->GetRootItem();
    long cookie;

	int lI = 0;

	wxTreeItemId lWtiiCmd = mWtcCommands->GetFirstChild(lWtiiRoot,cookie);
	while(lWtiiCmd.IsOk())
    {
		long lLSubCookie;
		wxTreeItemId lWtiiIs = mWtcCommands->GetFirstChild(lWtiiCmd,lLSubCookie);
		while (lWtiiIs.IsOk())
		{
			lNvt->Add("Command", lI, mWtcCommands->GetItemText(lWtiiCmd));
			lNvt->Add("InputSource", lI, mWtcCommands->GetItemText(lWtiiIs));
			
			lWtiiIs = mWtcCommands->GetNextChild(lWtiiCmd,lLSubCookie);

			++lI;
		}

		lWtiiCmd = mWtcCommands->GetNextChild(lWtiiRoot,cookie);
    }

	lNvt->Add("Count", lI);
	
	aNvt.Delete("ActiveCommands");
	aNvt.Add("ActiveCommands", lNvt);
}
