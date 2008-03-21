#ifndef ControlConfigFrame_h
#define ControlConfigFrame_h

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <iostream>

class wxTreeCtrl;
class wxListCtrl;
#include "..\NVThing.h"

class ControlConfigFrame : public wxFrame
{
public:
	/**
	* Constructor. Creates a new ControlConfigFrame
	*/
	ControlConfigFrame(int xpos, int ypos, int width, int height);
	/**
	* Destructor
	*/
	~ControlConfigFrame();
	/**
	* Processes menu File|Open
	*/
	void OnMenuFileOpen(wxCommandEvent &event);
	/**
	* Processes menu File|Save
	*/
	void OnMenuFileSave(wxCommandEvent &event);
	/**
	* Processes menu File|Quit
	*/
	void OnMenuFileQuit(wxCommandEvent &event);
	/**
	* Processes menu About|Info
	*/
	void OnMenuInfoAbout(wxCommandEvent &event);
	void OnMenuEditDelete(wxCommandEvent &event);
	void OnMenuEditAdd(wxCommandEvent &event);

	//void OnInputSourcesDblClick(wxCommandEvent &event);

	void OpenFile(const char *C);

	void SetupTree(NVThing &aNvtControls);
	void UpdateNvt(NVThing &aNvt);

protected:
	DECLARE_EVENT_TABLE()
private:
	wxTextCtrl *mWtcTextCtrl;
	wxTreeCtrl *mWtcCommands;
	wxListCtrl *mWlcInputSources;
	wxMenuBar *m_pMenuBar;
	wxMenu *m_pFileMenu;
	wxMenu *m_pEditMenu;
	wxMenu *m_pInfoMenu;
	wxString mWsFilename;
	NVThing mNvt;
	enum
	{
		MENU_FILE_OPEN,
		MENU_FILE_SAVE,
		MENU_FILE_QUIT,
		MENU_INFO_ABOUT,
		MENU_EDIT_DELETE,
		MENU_EDIT_ADD,
		INPUTSOURCES_DBLCLICK
	};
};

#endif ControlConfigFrame_h