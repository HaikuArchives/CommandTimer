/*
 * Copyright 2020. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Jason Scaroni, jscaroni@calpoly.edu
 *	Humdinger, humdingerb@gmail.com
 */

#include "CommandTimerWindow.h"


#include <Bitmap.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <IconUtils.h>
#include <LayoutBuilder.h>
#include <Path.h>
#include <Resources.h>
#include <SeparatorView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "TimerWindow"


CommandTimerWindow::CommandTimerWindow(BRect cTWindowRect)
	:
	BWindow(cTWindowRect, B_TRANSLATE_SYSTEM_NAME("CommandTimer"), B_TITLED_WINDOW,
		B_AUTO_UPDATE_SIZE_LIMITS | B_NOT_V_RESIZABLE | B_NOT_ZOOMABLE)
{
	// TextControls
	commandTextControl = new BTextControl(
		"commandTextControl", B_TRANSLATE("Command:"), NULL, NULL);
	commandTextControl->SetModificationMessage(new BMessage('CMND'));
	commandTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);

	pathTextControl = new BTextControl(
		"pathTextControl", B_TRANSLATE("Execute in:"), "/boot/home", NULL);
	pathTextControl->SetModificationMessage(new BMessage('CMND'));
	pathTextControl->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	pathTextControl->SetEnabled(false);

	// Spinners
	hoursSpinner = new BSpinner("hoursSpinner", B_TRANSLATE("Hours:"), NULL);
	hoursSpinner->SetRange(0, 10000);

	minsSpinner = new BSpinner("minsSpinner", B_TRANSLATE("Minutes:"), NULL);
	minsSpinner->SetRange(0, 59);

	secsSpinner = new BSpinner("secSpinner", B_TRANSLATE("Seconds:"), NULL);
	secsSpinner->SetRange(0, 59);

	// Buttons
	startStopButton = new BButton(
		"startStopButton", B_TRANSLATE("Start"), new BMessage('CLOK'));
	startStopButton->SetEnabled(false);
	startStopButton->SetExplicitMaxSize(
		BSize(B_SIZE_UNLIMITED, B_SIZE_UNLIMITED));

	chooseCommandButton = new BButton(NULL, new BMessage('CHCB'));
	choosePathButton = new BButton(NULL, new BMessage('CHPB'));
	size_t size;
	BResources* resources = BApplication::AppResources();
	const uint8* data = static_cast<const uint8*>(resources->LoadResource(
		B_VECTOR_ICON_TYPE, 2, &size));
	font_height	fontHeight;
	be_plain_font->GetHeight(&fontHeight);
	float height = fontHeight.ascent + fontHeight.descent + fontHeight.leading
		- 4;
	BBitmap* icon = new BBitmap(BRect(0, 0, height, height), 0, B_RGBA32);
	if (icon != NULL) {
		if (BIconUtils::GetVectorIcon(data, size, icon) == B_OK) {
			chooseCommandButton->SetIcon(icon);
			choosePathButton->SetIcon(icon);
		}
	}

	// Checkboxes
	repeatCheckBox = new BCheckBox(
		"repeatCheckBox", B_TRANSLATE("Repeat"), new BMessage('REPT'));
	repeatCheckBox->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	pathCheckBox = new BCheckBox("pathCheckBox", NULL, new BMessage('PATH'));

	alarmCheckBox = new BCheckBox(
		"alarmCheckBox", B_TRANSLATE("Alarm"), new BMessage('BEEP'));
	alarmCheckBox->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));

	// Do the layout
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_WINDOW_INSETS)
		.AddGrid(B_USE_HALF_ITEM_SPACING, 0)
			.Add(commandTextControl->CreateLabelLayoutItem(), 0, 0)
			.Add(commandTextControl->CreateTextViewLayoutItem(), 1, 0, 2, 1)
			.Add(chooseCommandButton, 3, 0)
			.Add(repeatCheckBox, 1, 1)
			.Add(alarmCheckBox, 1, 2)
			.Add(pathTextControl->CreateLabelLayoutItem(), 0, 3)
			.Add(pathTextControl->CreateTextViewLayoutItem(), 1, 3)
			.Add(choosePathButton, 2, 3)
			.Add(pathCheckBox, 3, 3)
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

	choosePanel = new BFilePanel(B_OPEN_PANEL, NULL, NULL,
		B_FILE_NODE | B_DIRECTORY_NODE, false);
	choosePanel->SetTarget(this);
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
			startStopButton->SetEnabled(
				commandTextControl->TextLength() == 0 ? false : true);
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
		case B_SIMPLE_DATA:
		{
			if (cTMessage->WasDropped()) {
				BRect commandRect = commandTextControl->ConvertToScreen(
					commandTextControl->Bounds());
				BRect pathRect = pathTextControl->ConvertToScreen(
					pathTextControl->Bounds());
				BPoint dropPoint = cTMessage->DropPoint();

				if (commandRect.Contains(dropPoint))
					commandTextControl->SetText(getPath(cTMessage));

				if (pathRect.Contains(dropPoint)) {
					pathTextControl->SetText(getFolder(cTMessage));
					path = true;
					pathTextControl->SetEnabled(path);
					pathCheckBox->SetValue(path);
				}
			}
			break;
		}
		case 'CHCB':
		{
			mode = COMMAND_MODE;
			choosePanel->Show();
			break;
		}
		case 'CHPB':
		{
			mode = PATH_MODE;
			choosePanel->Show();
			break;
		}
		case B_REFS_RECEIVED:
		{
			if (mode == COMMAND_MODE)
				commandTextControl->SetText(getPath(cTMessage));
			else if (mode == PATH_MODE) {
				pathTextControl->SetText(getFolder(cTMessage));
				path = true;
				pathTextControl->SetEnabled(path);
				pathCheckBox->SetValue(path);
			}
		}
		default:
			BWindow::MessageReceived(cTMessage);
			break;
	}
}


BString
CommandTimerWindow::getPath(BMessage* message)
{
	int32 ref_num;
	entry_ref ref;
	BString text = B_TRANSLATE("Error");

	if (message->FindRef("refs", &ref) == B_OK) {
		BPath path;
		BEntry* entry = new BEntry(&ref);
		entry->GetPath(&path);
		text = path.Path();
	}
	return (text);
}


BString
CommandTimerWindow::getFolder(BMessage* message)
{
	entry_ref ref;
	BString text = "Error";

	if (message->FindRef("refs", &ref) == B_OK) {
		BPath path;
		BEntry* entry = new BEntry(&ref);
		entry->GetPath(&path);

		if (entry->IsSymLink()) { // Resolve symlinked folders only
			entry = new BEntry(&ref, true);
			if (entry->IsDirectory())
				entry->GetPath(&path);
		}
		if (!entry->IsDirectory())
			path.GetParent(&path);

		text = path.Path();
	}
	return (text);
}


void
CommandTimerWindow::doPulse()
{
	if (isRunning) {
		if (seconds > 1)
			seconds--;
		else if (isRunning) {
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
		seconds = secondsBackup;
		updateTime();
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
