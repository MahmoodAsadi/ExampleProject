// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "IndependentInputManagerTypes.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SAxisInputView : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SAxisInputView)
		: _RawInputValue(0.0f)
		, _OutputInputValue(0.0f)
		{
		}

		SLATE_ARGUMENT(FJoystickAxisKeyMapping, AxisMapping)

		SLATE_ATTRIBUTE(float, RawInputValue)
		SLATE_ATTRIBUTE(float, OutputInputValue)

		// Must use the same ordering as AxisMapping.VirtualButtons.
		SLATE_ARGUMENT(TArray<TAttribute<bool>>, VirtualButtonStates)

		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	FJoystickAxisKeyMapping AxisMapping;
	FProgressBarStyle AxisProgressBarStyle;
	TAttribute<float> RawInputValue;
	TAttribute<float> OutputInputValue;

	TArray<TAttribute<bool>> VirtualButtonStates;

	FOnClicked OnClicked;

};
