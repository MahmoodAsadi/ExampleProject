// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SHatInputView : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SHatInputView)
		{
		}

		SLATE_ARGUMENT(FJoystickHatKeyMapping, HatMapping)
		SLATE_ARGUMENT(TAttribute<uint8>, Direction)
		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);


	FJoystickHatKeyMapping HatMapping;
	TAttribute<uint8> Direction;
	FOnClicked OnClicked;
	FSlateBrush CircleBrush;
};
