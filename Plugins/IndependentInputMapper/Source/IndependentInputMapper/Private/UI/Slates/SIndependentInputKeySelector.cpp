// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Slates/SIndependentInputKeySelector.h"

#include "IndependentCaptureInputProcessor.h"


void SIndependentInputKeySelector::Construct(const FArguments& InArgs)
{
	SelectedKey = InArgs._SelectedKey;
	KeySelectionText = InArgs._KeySelectionText;
	NoKeySpecifiedText = InArgs._NoKeySpecifiedText;
	OnKeySelected = InArgs._OnKeySelected;
	OnSelectingKeyChanged = InArgs._OnSelectingKeyChanged;
	OnKeySelectionCanceled = InArgs._OnKeySelectionCanceled;
	InputKeyCaptureInfo = InArgs._InputKeyCaptureInfo;
	bEscapeCancelsSelection = InArgs._EscapeCancelsSelection;
	bIsFocusable = InArgs._IsFocusable;

	bIsSelectingKey = false;

	ChildSlot
		[
			SAssignNew(Button, SButton)
				.ButtonStyle(InArgs._ButtonStyle)
				.IsFocusable(bIsFocusable)
				.OnClicked(this, &SIndependentInputKeySelector::OnClicked)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				[
					SAssignNew(TextBlock, STextBlock)
						.Text(this, &SIndependentInputKeySelector::GetSelectedKeyText)
						.TextStyle(InArgs._TextStyle)
						.Margin(Margin)
						.Justification(ETextJustify::Center)
				]
		];
}

SIndependentInputKeySelector::~SIndependentInputKeySelector()
{
	CancelCapture();
}

FInputChord SIndependentInputKeySelector::GetSelectedKey() const
{
	return SelectedKey.IsSet() ? SelectedKey.Get() : EKeys::Invalid;
}

void SIndependentInputKeySelector::SetSelectedKey(TAttribute<FInputChord> InSelectedKey)
{
	if (SelectedKey.IdenticalTo(InSelectedKey) == false)
	{
		SelectedKey = InSelectedKey;
		OnKeySelected.ExecuteIfBound(SelectedKey.IsSet() ? SelectedKey.Get() : FInputChord(EKeys::Invalid));
	}
}

void SIndependentInputKeySelector::SetMargin(TAttribute<FMargin> InMargin)
{
	Margin = InMargin;
}

void SIndependentInputKeySelector::SetButtonStyle(const FButtonStyle* InButtonStyle)
{
	ButtonStyle = InButtonStyle ? *InButtonStyle : FButtonStyle();

	if (Button.IsValid())
		Button->SetButtonStyle(&ButtonStyle);
}

void SIndependentInputKeySelector::SetTextStyle(const FTextBlockStyle* InTextStyle)
{
	TextStyle = InTextStyle ? *InTextStyle : FTextBlockStyle();
	if (TextBlock.IsValid())
		TextBlock->SetTextStyle(&TextStyle);
}

void SIndependentInputKeySelector::SetTextJustification(ETextJustify::Type Justification)
{
	if (TextBlock.IsValid())
		TextBlock->SetJustification(Justification);
}

void SIndependentInputKeySelector::SetTextVerticalAlignment(EVerticalAlignment VerticalAlignment)
{
	if (Button.IsValid())
		Button->SetVAlign(VerticalAlignment);
}

void SIndependentInputKeySelector::SetTextHorizontalAlignment(EHorizontalAlignment HorizontalAlignment)
{
	if (Button.IsValid())
		Button->SetHAlign(HorizontalAlignment);
}

void SIndependentInputKeySelector::SetInputKeyCaptureInfo(const FIndependentInputCaptureInfo& InInputKeyCaptureInfo)
{
	InputKeyCaptureInfo = InInputKeyCaptureInfo;
}

void SIndependentInputKeySelector::OnFocusLost(const FFocusEvent& InFocusEvent)
{
	CancelCapture();
}

FText SIndependentInputKeySelector::GetSelectedKeyText() const
{
	if (bIsSelectingKey)
	{
		return KeySelectionText;
	}
	else if (SelectedKey.IsSet())
	{
		if (SelectedKey.Get().Key.IsValid())
		{
			// If the key in the chord is a modifier key, print it's display name directly since the FInputChord
					// displays these as empty text.
			return SelectedKey.Get().Key.IsModifierKey()
				? SelectedKey.Get().Key.GetDisplayName()
				: SelectedKey.Get().GetInputText();
		}
	}
	return NoKeySpecifiedText;
}

FReply SIndependentInputKeySelector::OnClicked()
{
	if (bIsSelectingKey)
		return FReply::Handled();

	TouchInputDataMap.Empty();
	SetIsSelectingKey(true);
	InputProcessor = MakeShared<FIndependentCaptureInputProcessor>(this);

	if (!FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0))
	{
		SetIsSelectingKey(false);
		InputProcessor.Reset();
	}

	return FReply::Handled()
		.SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

void SIndependentInputKeySelector::SelectKey(FKey Key, bool bShiftDown, bool bControllDown, bool bAltDown, bool bCommandDown)
{
	FInputChord NewSelectedKey = InputKeyCaptureInfo.bAllowModifierKeys
		? FInputChord(Key, bShiftDown, bControllDown, bAltDown, bCommandDown)
		: FInputChord(Key);

	if (SelectedKey.IsBound() == false)
		SelectedKey.Set(NewSelectedKey);

	OnKeySelected.ExecuteIfBound(NewSelectedKey);
}

void SIndependentInputKeySelector::SetIsSelectingKey(bool bInIsSelectingKey)
{
	if (bIsSelectingKey != bInIsSelectingKey)
	{
		bIsSelectingKey = bInIsSelectingKey;

		// Prevents certain inputs from being consumed by the button
		if (Button.IsValid())
			Button->SetEnabled(!bIsSelectingKey);

		OnSelectingKeyChanged.ExecuteIfBound();
	}
}

bool SIndependentInputKeySelector::ProcessKeyUp(const FKeyEvent& InKeyEvent)
{
	if (!bIsSelectingKey)
		return false;

	if (bEscapeCancelsSelection && InputKeyCaptureInfo.IsEscapeKey(InKeyEvent.GetKey()))
	{
		CancelCapture();
		return true;
	}

	if (!InKeyEvent.IsRepeat() && InputKeyCaptureInfo.IsKeyCompatibleForCapture(InKeyEvent.GetKey()))
	{
		CompletedCapture(InKeyEvent);
		return true;
	}

	return false;
}

bool SIndependentInputKeySelector::ProcessAnalogInput(const FAnalogInputEvent& InAnalogInputEvent)
{
	if (!bIsSelectingKey)
		return false;

	FKey AnalogKey = InAnalogInputEvent.GetKey();
	if (bEscapeCancelsSelection && InputKeyCaptureInfo.IsEscapeKey(AnalogKey))
	{
		CancelCapture();
		return true;
	}
	
	switch (InputKeyCaptureInfo.BindingCaptureType)
	{
		case EIndependentInputBindingCaptureType::Axis1D:
		{
			if (InputKeyCaptureInfo.IsKeyCompatibleForCapture(AnalogKey))
			{
				if (AnalogKey.IsTouch())
				{
					if (ShouldCaptureTouchInput(InAnalogInputEvent))
					{
						FModifierKeysState ModifierState;
						CompletedCapture(FKeyEvent(AnalogKey, FModifierKeysState(), InAnalogInputEvent.GetInputDeviceId(), false, 0, 0));
						return true;
					}
				}
				else
				{
					if (FMath::Abs(InAnalogInputEvent.GetAnalogValue()) >= InputKeyCaptureInfo.AxisThreshold)
					{
						FModifierKeysState ModifierState;
						CompletedCapture(FKeyEvent(AnalogKey, FModifierKeysState(), InAnalogInputEvent.GetInputDeviceId(), false, 0, 0));
						return true;
					}
				}
			}
			break;
		}

		case EIndependentInputBindingCaptureType::Axis2D:
		{
			FKey CapturingKey;
			if (AnalogKey.IsAxis2D())
			{
				CapturingKey = AnalogKey;
			}
			else if (AnalogKey.IsAxis1D())
			{
				switch (AnalogKey.GetPairedAxis())
				{
				case EPairedAxis::X:
				case EPairedAxis::Y:
					CapturingKey = AnalogKey.GetPairedAxisKey();
					break;

				case EPairedAxis::Unpaired:
				case EPairedAxis::Z:
				default:
					return false;
				}
			}

			if (InputKeyCaptureInfo.IsKeyCompatibleForCapture(CapturingKey))
			{
				if (AnalogKey.IsTouch())
				{
					if (ShouldCaptureTouchInput(InAnalogInputEvent))
					{
						FModifierKeysState ModifierState;
						CompletedCapture(FKeyEvent(CapturingKey, FModifierKeysState(), InAnalogInputEvent.GetInputDeviceId(), false, 0, 0));
						return true;
					}
				}
				else
				{
					if (FMath::Abs(InAnalogInputEvent.GetAnalogValue()) >= InputKeyCaptureInfo.AxisThreshold)
					{
						FModifierKeysState ModifierState;
						CompletedCapture(FKeyEvent(CapturingKey, FModifierKeysState(), InAnalogInputEvent.GetInputDeviceId(), false, 0, 0));
						return true;
					}
				}
			}
			break;
		}
	}

	return false;
}

bool SIndependentInputKeySelector::ProcessMouseMove(const FPointerEvent& InPointerEvent)
{
	if (!bIsSelectingKey)
		return false;

	const FVector2D MouseDelta = InPointerEvent.GetCursorDelta();
	FKey CapturingKey;
	switch (InputKeyCaptureInfo.BindingCaptureType)
	{
		case EIndependentInputBindingCaptureType::Axis1D:
		{
			CapturingKey = FKey(FMath::Abs(MouseDelta.X) >= FMath::Abs(MouseDelta.Y) ? EKeys::MouseX : EKeys::MouseY);
			break;
		}

		case EIndependentInputBindingCaptureType::Axis2D:
		{
			CapturingKey = FKey(EKeys::Mouse2D);
			break;
		}

	default:
		return false;
	}

	if (bEscapeCancelsSelection && InputKeyCaptureInfo.IsEscapeKey(CapturingKey))
	{
		CancelCapture();
		return true;
	}

	if (InputKeyCaptureInfo.IsKeyCompatibleForCapture(CapturingKey))
	{
		if (FMath::Abs(MouseDelta.X) >= InputKeyCaptureInfo.MouseAxisDeltaThreshold
			|| FMath::Abs(MouseDelta.Y) >= InputKeyCaptureInfo.MouseAxisDeltaThreshold)
		{
			CompletedCapture(FKeyEvent(CapturingKey, FModifierKeysState(), InPointerEvent.GetInputDeviceId(), false, 0, 0));
			return true;
		}
	}

	return false;
	
}

void SIndependentInputKeySelector::CompletedCapture(const FKeyEvent& InKeyEvent)
{
	if (!bIsSelectingKey && !InputProcessor.IsValid())
		return;

	FKey KeyUp = InKeyEvent.GetKey();
	EModifierKey::Type ModifierKey = EModifierKey::FromBools(
		InKeyEvent.IsControlDown() && KeyUp != EKeys::LeftControl && KeyUp != EKeys::RightControl,
		InKeyEvent.IsAltDown() && KeyUp != EKeys::LeftAlt && KeyUp != EKeys::RightAlt,
		InKeyEvent.IsShiftDown() && KeyUp != EKeys::LeftShift && KeyUp != EKeys::RightShift,
		InKeyEvent.IsCommandDown() && KeyUp != EKeys::LeftCommand && KeyUp != EKeys::RightCommand);

	// Ignore Modifier keys for gamepad buttons.
	if (KeyUp.IsGamepadKey())
		ModifierKey = EModifierKey::None;

	SetIsSelectingKey(false);

	SelectKey(
		KeyUp,
		(ModifierKey & EModifierKey::Shift) != EModifierKey::None,
		(ModifierKey & EModifierKey::Control) != EModifierKey::None,
		(ModifierKey & EModifierKey::Alt) != EModifierKey::None,
		(ModifierKey & EModifierKey::Command) != EModifierKey::None);

	if (InputProcessor.IsValid() && FSlateApplication::IsInitialized())
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);

	InputProcessor.Reset();
}

void SIndependentInputKeySelector::CancelCapture()
{
	if (!bIsSelectingKey && !InputProcessor.IsValid())
		return;

	SetIsSelectingKey(false);
	if (InputProcessor.IsValid() && FSlateApplication::IsInitialized())
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);

	InputProcessor.Reset();
	OnKeySelectionCanceled.ExecuteIfBound();
}

bool SIndependentInputKeySelector::ShouldCaptureTouchInput(const FAnalogInputEvent& InAnalogInputEvent)
{
	FKey AnalogKey = InAnalogInputEvent.GetKey();
	const double CurrentTime = FPlatformTime::Seconds();
	FTouchInputData CurrentTouchData(AnalogKey, CurrentTime, InAnalogInputEvent.GetAnalogValue());

	if (FTouchInputData* FoundTouchData = TouchInputDataMap.Find(AnalogKey))
	{
		double DeltaTime = CurrentTime - FoundTouchData->Timestamp;
		if (DeltaTime > 0.1f)
		{
			// if delta time is greater than 0.1 seconds, update the timestamp and value to the current touch data since it is fresh touch start.
			FoundTouchData->Timestamp = CurrentTouchData.Timestamp;
			FoundTouchData->Value = CurrentTouchData.Value;
			return false;
		}

		const float DeltaValue = FMath::Abs(InAnalogInputEvent.GetAnalogValue() - FoundTouchData->Value);
		if (DeltaValue >= InputKeyCaptureInfo.TouchDeltaThreshold)
			return true;

		FoundTouchData->Timestamp = CurrentTouchData.Timestamp;
		FoundTouchData->Value = CurrentTouchData.Value;
	}
	else
	{
		TouchInputDataMap.Add(AnalogKey, CurrentTouchData);
	}

	return false;
}
