// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"
#include "Framework/Commands/InputChord.h"

#include "IndependentInputMappingTypes.h"

/**
 *
 */
class INDEPENDENTINPUTMAPPER_API SIndependentInputKeySelector : public SCompoundWidget
{
public:

	friend class FIndependentCaptureInputProcessor;

	DECLARE_DELEGATE_OneParam(FOnKeySelected, const FInputChord&)
	DECLARE_DELEGATE(FOnSelectingKeyChanged)
	DECLARE_DELEGATE(FOnKeySelectionCanceled)

	SLATE_BEGIN_ARGS(SIndependentInputKeySelector)
		: _SelectedKey(FInputChord(EKeys::Invalid))
		, _ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
		, _TextStyle(&FCoreStyle::Get().GetWidgetStyle< FTextBlockStyle >("NormalText"))
		, _KeySelectionText(NSLOCTEXT("InputKeySelector", "ModifingKeySelectionText", "Press Any Key..."))
		, _NoKeySpecifiedText(NSLOCTEXT("InputKeySelector", "UnAssignedKeySelectionText", "UnAssigned"))
		, _InputKeyCaptureInfo(FIndependentInputCaptureInfo())
		, _EscapeCancelsSelection(true)
		, _IsFocusable(true)
		{
		}

		/** The currently selected key */
		SLATE_ATTRIBUTE(FInputChord, SelectedKey)

		/** The font used to display the currently selected key. */
		SLATE_ATTRIBUTE(FSlateFontInfo, Font)

		/** The margin around the selected key text. */
		SLATE_ATTRIBUTE(FMargin, Margin)

		/** The style of the button used to enable key selection. */
		SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)

		/** The text style of the button text */
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)

		/** The text to display while selecting a new key. */
		SLATE_ARGUMENT(FText, KeySelectionText)

		/** The text to display while no key text is available or not selecting a key. */
		SLATE_ARGUMENT(FText, NoKeySpecifiedText)

		/** The information about the key capture. */
		SLATE_ARGUMENT(FIndependentInputCaptureInfo, InputKeyCaptureInfo)

		/** When true, pressing escape will cancel the key selection, when false, pressing escape will select the escape key. */
		SLATE_ARGUMENT(bool, EscapeCancelsSelection)

		/** Occurs whenever a new key is selected. */
		SLATE_EVENT(FOnKeySelected, OnKeySelected)

		/** Occurs whenever key selection mode starts and stops. */
		SLATE_EVENT(FOnSelectingKeyChanged, OnSelectingKeyChanged)

		/** Occurs whenever key selection mode canceled by pressing any escape key(s). */
		SLATE_EVENT(FOnKeySelectionCanceled, OnKeySelectionCanceled)

		/** Sometimes a button should only be mouse-clickable and never keyboard focusable. */
		SLATE_ARGUMENT(bool, IsFocusable)
	SLATE_END_ARGS()

	virtual ~SIndependentInputKeySelector() override;

	void Construct(const FArguments& InArgs);

public:

	/** Gets the currently selected key chord. */
	FInputChord GetSelectedKey() const;

	/** Sets the currently selected key chord. */
	void SetSelectedKey(TAttribute<FInputChord> InSelectedKey);

	/** Sets the margin around the text used to display the currently selected key */
	void SetMargin(TAttribute<FMargin> InMargin);

	/** Sets the style of the button which is used enter key selection mode. */
	void SetButtonStyle(const FButtonStyle* InButtonStyle);

	/** Sets the style of the text on the button which is used enter key selection mode. */
	void SetTextStyle(const FTextBlockStyle* InTextStyle);

	/** Sets the text which is displayed when selecting a key. */
	void SetKeySelectionText(FText InKeySelectionText) { KeySelectionText = MoveTemp(InKeySelectionText); }

	/** Sets the text to display when no key text is available or not selecting a key. */
	void SetNoKeySpecifiedText(FText InNoKeySpecifiedText) { NoKeySpecifiedText = MoveTemp(InNoKeySpecifiedText); }

	void SetTextJustification(ETextJustify::Type Justification);

	void SetTextVerticalAlignment(EVerticalAlignment VerticalAlignment);

	void SetTextHorizontalAlignment(EHorizontalAlignment HorizontalAlignment);

	/** Returns true whenever key selection mode is active, otherwise returns false. */
	bool GetIsSelectingKey() const { return bIsSelectingKey; }

	/** Sets the input key capture info. */
	void SetInputKeyCaptureInfo(const FIndependentInputCaptureInfo& InInputKeyCaptureInfo);

	/** Returns current input key capture info. */
	const FIndependentInputCaptureInfo& GetInputKeyCaptureInfo() const { return InputKeyCaptureInfo; }

	/** Sets whether Escape and configured cancel keys cancel key selection. */
	void SetEscapeCancelsSelection(bool bInEscapeCancelsSelection) { bEscapeCancelsSelection = bInEscapeCancelsSelection; }


	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual bool SupportsKeyboardFocus() const override { return true; }

private:

	/** Gets the display text for the currently selected key. */
	FText GetSelectedKeyText() const;

	/** Handles the OnClicked event from the button which enables key selection mode. */
	FReply OnClicked();

	/** Sets the currently selected key and invokes the associated events. */
	void SelectKey(FKey Key, bool bShiftDown, bool bControllDown, bool bAltDown, bool bCommandDown);

	/** Sets bIsSelectingKey and invokes the associated events. */
	void SetIsSelectingKey(bool bInIsSelectingKey);

	// Handles the key down event when key selection mode is active. Returns true if the event was handled, otherwise returns false.
	bool ProcessKeyUp(const FKeyEvent& InKeyEvent);

	// Handles the analog input event when key selection mode is active. Returns true if the event was handled, otherwise returns false.
	bool ProcessAnalogInput(const FAnalogInputEvent& InAnalogInputEvent);

	// Handles the mouse button down event when key selection mode is active. Returns true if the event was handled, otherwise returns false.
	bool ProcessMouseMove(const FPointerEvent& InPointerEvent);

	void CompletedCapture(const FKeyEvent& InKeyEvent);
	void CancelCapture();
	bool ShouldCaptureTouchInput(const FAnalogInputEvent& InAnalogInputEvent);

private:

	/** True when key selection mode is active. */
	bool bIsSelectingKey;

	TSharedPtr<IInputProcessor> InputProcessor;

	/** The currently selected key chord. */
	TAttribute<FInputChord> SelectedKey;

	/** The margin around the text used to display the currently selected key. */
	TAttribute<FMargin> Margin;

	/** The text to display when selecting keys. */
	FText KeySelectionText;

	/**  The text to display while no key text is available or not selecting a key. */
	FText NoKeySpecifiedText;

	FButtonStyle ButtonStyle;

	FTextBlockStyle TextStyle;

	/** The information about the key capture. */
	FIndependentInputCaptureInfo InputKeyCaptureInfo;

	/** When true, pressing escape will cancel the key selection, when false, pressing escape will select the escape key. */
	bool bEscapeCancelsSelection;

	/** The button which starts the key selection mode. */
	TSharedPtr<SButton> Button;

	/** The text which is rendered on the button. */
	TSharedPtr<STextBlock> TextBlock;

	/** Can this button be focused? */
	bool bIsFocusable;

	/** Delegate which is run any time a new key is selected. */
	FOnKeySelected OnKeySelected;

	/** Delegate which is run when key selection mode starts and stops. */
	FOnSelectingKeyChanged OnSelectingKeyChanged;

	FOnKeySelectionCanceled OnKeySelectionCanceled;

	struct FTouchInputData
	{
		FTouchInputData() {}
		FTouchInputData(FKey InKey, double InTimestamp, float InValue)
			: Key(InKey), Timestamp(InTimestamp), Value(InValue)
		{
		}

		FKey Key;
		double Timestamp = 0;
		float Value = 0.0f;
	};

	TMap<FKey, FTouchInputData> TouchInputDataMap;
	
};
