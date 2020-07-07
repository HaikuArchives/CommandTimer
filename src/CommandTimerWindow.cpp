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
		B_TRANSLATE("Command:"), NULL, NULL);
	commandTextControl->SetModificationMessage(new BMessage('CMND'));
	commandTextControl->SetDivider(60);
	commandTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);

	pathTextControl = new BTextControl("pathTextControl",
		B_TRANSLATE("Execute in:"), "/boot/home", NULL);
	pathTextControl->SetModificationMessage(new BMessage('CMND'));
	pathTextControl->SetDivider(60);
	pathTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	pathTextControl->SetEnabled(false);

	hoursSpinner = new BSpinner("hoursSpinner", B_TRANSLATE("Hours:"), NULL);
	hoursSpinner->SetRange(0, 10000);

	minsSpinner = new BSpinner("minsSpinner", B_TRANSLATE("Minutes:"), NULL);
	minsSpinner->SetRange(0, 59);

	secsSpinner = new BSpinner("minsSpinner", B_TRANSLATE("Seconds:"), NULL);
	secsSpinner->SetRange(0, 59);

	startStopButton = new BButton("startStopButton", B_TRANSLATE("Start"),
		new BMessage('CLOK'));
	startStopButton->SetEnabled(false);
	startStopButton->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNLIMITED));

	repeatCheckBox = new BCheckBox("repeatCheckBox", B_TRANSLATE("Repeat"),
		new BMessage('REPT'));
	repeatCheckBox->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	pathCheckBox = new BCheckBox("pathCheckBox", NULL, new BMessage('PATH'));

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
			.AddGlue()
			.AddGrid(B_USE_DEFAULT_SPACING, B_USE_HALF_ITEM_SPACING)
				.Add(hoursSpinner->CreateLabelLayoutItem(), 0, 0)
				.Add(hoursSpinner->CreateTextViewLayoutItem(), 1, 0)
				.Add(minsSpinner->CreateLabelLayoutItem(), 0, 1)
				.Add(minsSpinner->CreateTextViewLayoutItem(), 1, 1)
				.Add(secsSpinner->CreateLabelLayoutItem(), 0, 2)
				.Add(secsSpinner->CreateTextViewLayoutItem(), 1, 2)
				.Add(startStopButton, 3, 0, 1, 3)
			.End()
			.AddGlue()
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
		case 'CMND':
			startStopButton->SetEnabled(commandTextControl->TextLength() == 0
				? false : true);
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
	hoursSpinner->SetValue(seconds / 3600);
	minsSpinner->SetValue((seconds / 60) % 60);
	secsSpinner->SetValue(seconds % 60);
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
	hoursSpinner->SetEnabled(permission);
	minsSpinner->SetEnabled(permission);
	secsSpinner->SetEnabled(permission);
}


void
CommandTimerWindow::getTime()
{
	seconds = hoursSpinner->Value() * 3600
		+ minsSpinner->Value() * 60
		+ secsSpinner->Value();
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