#ifndef COMMANDTIMERWINDOW_H
#define COMMANDTIMERWINDOW_H

#include "CommandTimerView.h"
#include <Application.h>
#include <InterfaceKit.h>
#include <Window.h>

class CommandTimerWindow : public BWindow
{
public:
	CommandTimerWindow(BRect cTWindowRect);

	virtual void MessageReceived(BMessage* cTMessage);
	virtual bool QuitRequested();

private:
	CommandTimerView* cTView;
};

#endif
