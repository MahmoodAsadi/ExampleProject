// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "Styling/SlateTypes.h"
#include "UObject/NoExportTypes.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

class SHorizontalBox;

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SSensorInputView : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SSensorInputView)
		{
		}

		SLATE_ARGUMENT(EDeviceSensorType, SensorType)
		SLATE_ARGUMENT(FJoystickSensorKeyMapping, SensorMapping)
		SLATE_ARGUMENT(TAttribute<FSensorState>, SensorState)
		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	

private:

	TSharedRef<SHorizontalBox> CreateProgressBar(const FText& ValueLabel, EAxis::Type Axis);
	FProgressBarStyle ProgressBarStyle;
	EDeviceSensorType SensorType;
	FJoystickSensorKeyMapping SensorMapping;
	TAttribute<FSensorState> SensorState;
	FOnClicked OnClicked;
};
