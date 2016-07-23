#include "CommandTimerWindow.h"

CommandTimerWindow::CommandTimerWindow(BRect cTWindowRect)
	: BWindow(cTWindowRect, "CommandTimer", B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	cTView = new CommandTimerView(Bounds());
	cTView->SetViewColor(219, 219, 219, 255);
	AddChild(cTView);
}

bool CommandTimerWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);

	return true;
}

void CommandTimerWindow::MessageReceived(BMessage *cTMessage)
{
	switch(cTMessage->what)
	{
		case 'PULS':
			cTView->doPulse();
			break;
		case 'CLOK':
			cTView->startStopClock();
			break;
		case 'REPT':
			cTView->toggleRepeat();
			break;
		case 'BEEP':
			cTView->toggleAlarm();
			break;
		case 'TRAK':
			cTView->toggleTracker();
			break;
		case 'PATH':
			cTView->togglePath();
			break;
		default:
			BWindow::MessageReceived(cTMessage);
			break;
	}
}
