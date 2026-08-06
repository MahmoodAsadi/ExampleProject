// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "Widgets/SCompoundWidget.h"

#include "IndependentInputManagerTypes.h"

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SBallInputView : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SBallInputView)
		{
		}

		SLATE_ARGUMENT(FJoystickBallKeyMapping, BallMapping)
		SLATE_ARGUMENT(TAttribute<FVector2D>, BallState)
		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	FJoystickBallKeyMapping BallMapping;
	TAttribute<FVector2D> BallState;
	FOnClicked OnClicked;
	FVector2D InterpValue;
	FSlateBrush SlateBrush;

};
