// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/SlateDelegates.h"
#include "IndependentInputManagerTypes.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SButtonInputView : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SButtonInputView)
		: _ButtonIndex(INDEX_NONE)
		, _Key(FIndependentInputKey())
		, _IsPressed(false)
		, _DeviceInstanceId(FInputDeviceInstanceId())
		{
		}

		SLATE_ARGUMENT(int32, ButtonIndex)
		SLATE_ATTRIBUTE(FIndependentInputKey, Key)
		SLATE_ATTRIBUTE(bool, IsPressed)
		SLATE_ATTRIBUTE(FInputDeviceInstanceId, DeviceInstanceId)
		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	int32 ButtonIndex = INDEX_NONE;
	TAttribute<FIndependentInputKey> Key;
	TAttribute<bool> IsPressed;
	TAttribute<FInputDeviceInstanceId> DeviceInstanceId;
	FOnClicked OnClicked;

};