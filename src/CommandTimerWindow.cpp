/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

 #include "CommandTimerWindow.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TimerWindow"

CommandTimerWindow::CommandTimerWindow(BRect cTWindowRect)
	:
	BWindow(cTWindowRect, "CommandTimer", B_TITLED_WINDOW,
		B_AUTO_UPDATE_SIZE_LIMITS | B_NOT_V_RESIZABLE | B_NOT_ZOOMABLE)
{
	commandTextControl = new BTextControl("commandTextControl",
		B_TRANSLATE("Command:"), NULL, new BMessage('CMND'));
	commandTextControl->SetDivider(60);
	commandTextControl->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	commandTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);

	pathTextControl = new BTextControl("pathTextControl",
		B_TRANSLATE("Use path:"), "/boot/home", new BMessage('CMND'));
	pathTextControl->SetDivider(60);
	pathTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	pathTextControl->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	pathTextControl->SetEnabled(false);

	hoursTextControl = new BTextControl("hoursTextControl", NULL, "00", NULL);
	hoursTextControl->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	hoursTextControl->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

	minsTextControl = new BTextControl("minsTextControl", NULL, "00", NULL);
	minsTextControl->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	minsTextControl->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

	secsTextControl = new BTextControl("minsTextControl", NULL, "00", NULL);
	secsTextControl->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	secsTextControl->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);

	startStopButton = new BButton("startStopButton", B_TRANSLATE("Start"),
		new BMessage('CLOK'));
	startStopButton->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	repeatCheckBox = new BCheckBox("repeatCheckBox", B_TRANSLATE("Repeat"),
		new BMessage('REPT'));
	repeatCheckBox->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	pathCheckBox = new BCheckBox("pathCheckBox", NULL, new BMessage('PATH'));
	pathCheckBox->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	alarmCheckBox = new BCheckBox("alarmCheckBox", B_TRANSLATE("Alarm"),
		new BMessage('BEEP'));
	alarmCheckBox->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_WINDOW_INSETS)
		.AddGrid(B_USE_DEFAULT_SPACING, 0)
			.Add(commandTextControl->CreateLabelLayoutItem(), 0, 0)
			.Add(commandTextControl->CreateTextViewLayoutItem(), 1, 0, 2, 1)
			.Add(pathTextControl->CreateLabelLayoutItem(), 0, 1)
			.Add(pathTextControl->CreateTextViewLayoutItem(), 1, 1)
			.Add(pathCheckBox, 2, 1)
			.Add(repeatCheckBox, 1, 2)
			.Add(alarmCheckBox, 1, 3)
			.SetColumnWeight(1, 10.f)
		.End()
		.Add(new BSeparatorView(B_HORIZONTAL))
		.AddGroup(B_HORIZONTAL, B_USE_HALF_ITEM_SPACING)
			.AddGlue(100)
			.Add(hoursTextControl)
			.Add(minsTextControl)
			.Add(secsTextControl)
			.AddStrut(B_USE_BIG_SPACING)
			.Add(startStopButton)
			.AddGlue(100)
		.End()
	.End();

	isRunning = false;
	alarm = false;
	repeat = false;
	path = false;

	runner
		= new BMessageRunner(BMessenger(this), new BMessage('PULS'), 1000000);
}


bool
CommandTimerWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);

	return true;
}


void
CommandTimerWindow::MessageReceived(BMessage* cTMessage)
{
	switch (cTMessage->what) {
		case 'PULS':
			doPulse();
			break;
		case 'CLOK':
			startStopClock();
			break;
		case 'REPT':
			toggleRepeat();
			break;
		case 'BEEP':
			toggleAlarm();
			break;
		case 'PATH':
			togglePath();
			break;

		default:
			BWindow::MessageReceived(cTMessage);
			break;
	}
}


void
CommandTimerWindow::doPulse()
{
	if (isRunning) {
		if (seconds)
			seconds--;
		else {
			if (isRunning) {
				seconds = secondsBackup;
				executeCommand();
				if (alarm)
					beep();
				if (!repeat) {
					setPermissions(true);
					isRunning = false;
					startStopButton->SetLabel(B_TRANSLATE("Start"));
				}
			}
		}
		updateTime();
	}
}


void
CommandTimerWindow::updateTime()
{
	char digits[3];
	sprintf(digits, "%02ld", seconds / 3600);
	hoursTextControl->SetText(digits);
	sprintf(digits, "%02ld", (seconds / 60) % 60);
	minsTextControl->SetText(digits);
	sprintf(digits, "%02ld", seconds % 60);
	secsTextControl->SetText(digits);
}


void
CommandTimerWindow::startStopClock()
{
	if (isRunning) {
		setPermissions(true);
		isRunning = false;
		startStopButton->SetLabel(B_TRANSLATE("Start"));
	} else {
		setPermissions(false);
		isRunning = true;
		getTime();
		startStopButton->SetLabel(B_TRANSLATE("Stop"));
	}
}


void
CommandTimerWindow::toggleRepeat()
{
	repeat = !repeat;
}


void
CommandTimerWindow::toggleAlarm()
{
	alarm = !alarm;
}


void
CommandTimerWindow::togglePath()
{
	path = !path;
	pathTextControl->SetEnabled(path);
}


void
CommandTimerWindow::setPermissions(bool permission)
{
	commandTextControl->SetEnabled(permission);
	pathCheckBox->SetEnabled(permission);
	pathTextControl->SetEnabled(permission ? path : false);
	hoursTextControl->SetEnabled(permission);
	minsTextControl->SetEnabled(permission);
	secsTextControl->SetEnabled(permission);
}


void
CommandTimerWindow::getTime()
{
	seconds = atoi(hoursTextControl->Text()) * 3600
		+ atoi(minsTextControl->Text()) * 60
		+ atoi(secsTextControl->Text());
	secondsBackup = seconds;
}


void
CommandTimerWindow::executeCommand()
{
	BString cmd = "%command% &";
	if (path) {
		cmd = "cd %path% ; %command% &";
		cmd.ReplaceFirst("%path%", pathTextControl->Text());
	}
	cmd.ReplaceFirst("%command%", commandTextControl->Text());

	system(cmd);
}