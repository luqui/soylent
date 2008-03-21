// For compilers that supports precompilation , includes "wx/wx.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "ControlConfigApp.h"
#include "ControlConfigFrame.h"
IMPLEMENT_APP(ControlConfigApp)
bool ControlConfigApp::OnInit()
{
	ControlConfigFrame *frame = new ControlConfigFrame(100, 100, 400, 300);
	frame->Show(TRUE);
	SetTopWindow(frame);

	if (argc > 1)
		frame->OpenFile(argv[1]);
	return true;
}