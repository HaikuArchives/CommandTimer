/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#include "CommandTimerApp.h"


CommandTimerApp::CommandTimerApp()
	:
	BApplication("application/x-vnd.jas.CommandTimer")
{
	BRect cTWindowRect(100, 100, 525, 240);
	cTWindow = new CommandTimerWindow(cTWindowRect);
	cTWindow->Show();
}
