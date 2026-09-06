// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentCaptureInputProcessor.h"

#include "UI/Slates/SIndependentInputKeySelector.h"


void FIndependentCaptureInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{

}

bool FIndependentCaptureInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	InKeyEvent.GetInputDeviceId();
	return InputKeySelector != nullptr;
}

bool FIndependentCaptureInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (InputKeySelector)
		return InputKeySelector->ProcessKeyUp(InKeyEvent);
	
	return false;
}

bool FIndependentCaptureInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	if (InputKeySelector)
		return InputKeySelector->ProcessAnalogInput(InAnalogInputEvent);

	return false;
}

bool FIndependentCaptureInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	if (InputKeySelector)
		return InputKeySelector->ProcessMouseMove(InPointerEvent);

	return false;
}

bool FIndependentCaptureInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return InputKeySelector != nullptr;
}

bool FIndependentCaptureInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (InputKeySelector)
		return InputKeySelector->ProcessKeyUp(
			FKeyEvent(
				MouseEvent.GetEffectingButton(), 
				MouseEvent.GetModifierKeys(), 
				MouseEvent.GetInputDeviceId(), 
				MouseEvent.IsRepeat(), 
				0, 
				0));

	return false;
}

bool FIndependentCaptureInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	return HandleMouseButtonDownEvent(SlateApp, InPointerEvent);
}

bool FIndependentCaptureInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	// Ignore Gesture events.
	if (InGestureEvent != nullptr)
		return false;

	if (InputKeySelector)
	{
		// Create Wheel Key manually since "InWheelEvent.GetEffectingButton()" is none.
		const FKey MouseWheelKey = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;
		
		return InputKeySelector->ProcessKeyUp(
			FKeyEvent(
				MouseWheelKey,
				InWheelEvent.GetModifierKeys(),
				InWheelEvent.GetInputDeviceId(),
				InWheelEvent.IsRepeat(),
				0,
				0));
	}

	return false;
}
