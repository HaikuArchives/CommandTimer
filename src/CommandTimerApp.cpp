#include "CommandTimerApp.h"


CommandTimerApp::CommandTimerApp()
	:
	BApplication("application/x-vnd.jas.CommandTimer")
{
	BRect cTWindowRect(100, 100, 425, 240);
	cTWindow = new CommandTimerWindow(cTWindowRect);
	cTWindow->Show();
}
