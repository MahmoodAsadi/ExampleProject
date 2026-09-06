// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputMappingTypes.h"


bool FIndependentInputCaptureInfo::IsKeyCompatibleForCapture(FKey InKey) const
{
	if (!InKey.IsValid())
		return false;

	if (!InKey.IsBindableInBlueprints())
		return false;

	if (SupportedCaptureInputDevice == 0)
		return false;

	if (EscapeKeys.Contains(InKey))
		return false;

	if (HasFlag(SupportedCaptureInputDevice, EIndependentInputBindingDeviceMask::Controller))
	{
		if (!InKey.IsGamepadKey() && !HasFlag(SupportedCaptureInputDevice, EIndependentInputBindingDeviceMask::KeyboardMouse))
			return false;
	}

	if (!HasFlag(SupportedCaptureInputDevice, EIndependentInputBindingDeviceMask::Controller))
	{
		if (InKey.IsGamepadKey())
			return false;
	}

	if (!bAllowModifierKeys)
	{
		if (InKey.IsModifierKey())
			return false;
	}
	
	if (!bAllowTouchKeys)
	{
		if (InKey.IsTouch())
			return false;
	}

	if (HasFlag(SupportedCaptureInputDevice, EIndependentInputBindingDeviceMask::KeyboardMouse))
	{
		if (!bAllowMouseKeys && InKey.IsMouseButton())
			return false;
	}

	switch (BindingCaptureType)
	{
		case EIndependentInputBindingCaptureType::Button: return !InKey.IsAxis1D() && !InKey.IsAxis2D() && !InKey.IsAnalog();
		case EIndependentInputBindingCaptureType::Axis1D: return InKey.IsAxis1D();
		case EIndependentInputBindingCaptureType::Axis2D: return InKey.IsAxis2D();
	}

	return true;
}

bool FIndependentInputCaptureInfo::IsEscapeKey(FKey InKey) const
{
	if (InKey == EKeys::Escape)
		return true;

	if (InKey == EKeys::Gamepad_Special_Right)
		return true;

	if (EscapeKeys.Contains(InKey))
		return true;

	return false;
}
