/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#include "CommandTimerApp.h"


int
main()
{
	CommandTimerApp* cTApp;

	cTApp = new CommandTimerApp();
	cTApp->Run();
	cTApp->PostMessage(B_QUIT_REQUESTED);
	delete cTApp;

	return 0;
}
