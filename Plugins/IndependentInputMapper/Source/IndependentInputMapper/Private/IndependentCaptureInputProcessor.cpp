// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentCaptureInputProcessor.h"

#include "UI/Slates/SIndependentInputKeySelector.h"


void FIndependentCaptureInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor)
{
	
}

bool FIndependentCaptureInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	return IInputProcessor::HandleKeyDownEvent(SlateApp, InKeyEvent);
}

bool FIndependentCaptureInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (InputKeySelector)
		InputKeySelector->ProcessKeyUp(InKeyEvent);

	return IInputProcessor::HandleKeyUpEvent(SlateApp, InKeyEvent);
}

bool FIndependentCaptureInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	if (InputKeySelector)
		InputKeySelector->ProcessAnalogInput(InAnalogInputEvent);

	return IInputProcessor::HandleAnalogInputEvent(SlateApp, InAnalogInputEvent);
}

bool FIndependentCaptureInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	if (InputKeySelector)
		InputKeySelector->ProcessMouseMove(InPointerEvent);

	return IInputProcessor::HandleMouseMoveEvent(SlateApp, InPointerEvent);
}

bool FIndependentCaptureInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	return IInputProcessor::HandleMouseButtonDownEvent(SlateApp, MouseEvent);
}

bool FIndependentCaptureInputProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	if (InputKeySelector)
		InputKeySelector->ProcessKeyUp(
			FKeyEvent(
				MouseEvent.GetEffectingButton(),
				MouseEvent.GetModifierKeys(),
				MouseEvent.GetInputDeviceId(),
				MouseEvent.IsRepeat(),
				0,
				0));

	return IInputProcessor::HandleMouseButtonUpEvent(SlateApp, MouseEvent);
}

bool FIndependentCaptureInputProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& InPointerEvent)
{
	return IInputProcessor::HandleMouseButtonDoubleClickEvent(SlateApp, InPointerEvent);
}

bool FIndependentCaptureInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	// Ignore Gesture events.
	if (InGestureEvent != nullptr)
		return IInputProcessor::HandleMouseWheelOrGestureEvent(SlateApp, InWheelEvent, InGestureEvent);

	if (InputKeySelector)
	{
		// Create Wheel Key manually since "InWheelEvent.GetEffectingButton()" is none.
		const FKey MouseWheelKey = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;

		InputKeySelector->ProcessKeyUp(
			FKeyEvent(
				MouseWheelKey,
				InWheelEvent.GetModifierKeys(),
				InWheelEvent.GetInputDeviceId(),
				InWheelEvent.IsRepeat(),
				0,
				0));
	}

	return IInputProcessor::HandleMouseWheelOrGestureEvent(SlateApp, InWheelEvent, InGestureEvent);
}
