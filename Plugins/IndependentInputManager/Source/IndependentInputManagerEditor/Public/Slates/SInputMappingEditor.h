// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(FOnSavedClicked, const UObject*)

/**
 * 
 */
class INDEPENDENTINPUTMANAGEREDITOR_API SInputMappingEditor : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SInputMappingEditor)
		: _InputMappingObject(nullptr)
		{
		}

		SLATE_ARGUMENT(UObject*, InputMappingObject)
		SLATE_EVENT(FOnSavedClicked, OnSave)
		SLATE_EVENT(FOnClicked, OnCancel)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	TStrongObjectPtr<UObject> ObjectToModify;
	FOnSavedClicked OnSave;
	FOnClicked OnCancel;
};
