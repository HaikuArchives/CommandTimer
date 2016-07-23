#include "CommandTimerApp.h"

int main()
{
	CommandTimerApp *cTApp;
	
	cTApp = new CommandTimerApp();
	cTApp->Run();
	cTApp->PostMessage(B_QUIT_REQUESTED);
	delete cTApp;
	
	return 0;
}