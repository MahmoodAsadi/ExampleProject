// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API STouchpadInputView : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(STouchpadInputView)
		{
		}

		SLATE_ARGUMENT(FJoystickTouchpadKeyMapping, TouchpadMapping)
		SLATE_ARGUMENT(TArray<TAttribute<FTouchFingerState>>, FingersState)
		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	FJoystickTouchpadKeyMapping TouchpadMapping;
	TArray<TAttribute<FTouchFingerState>> FingersState;
	FOnClicked OnClicked;

};
