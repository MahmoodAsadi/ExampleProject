// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputVisualCollection.h"


bool UInputKeyVisualCollection::FindKeyVisualInfo(const FKey& Key, FInputKeyVisualInfo& OutVisualInfo) const
{
	OutVisualInfo = FInputKeyVisualInfo();

	if (const FInputKeyVisualInfo* KeyVisual = KeyVisuals.Find(Key))
	{
		OutVisualInfo = *KeyVisual;
		return true;
	}

	return false;
}
