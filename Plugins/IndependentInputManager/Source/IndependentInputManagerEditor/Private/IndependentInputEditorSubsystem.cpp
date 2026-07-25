// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputEditorSubsystem.h"

#include "IndependentInputSubsystem.h"
#include "Slates/SIndependentInputDeviceManager.h"


UIndependentInputEditorSubsystem* UIndependentInputEditorSubsystem::Get()
{
	if (GEngine)
		return GEngine->GetEngineSubsystem<UIndependentInputEditorSubsystem>();

	return nullptr;
}

TSharedPtr<SIndependentInputDeviceManager> UIndependentInputEditorSubsystem::FindInputManagerTab()
{
	TSharedPtr<SDockTab> InputManagerTab = FGlobalTabmanager::Get()->FindExistingLiveTab(InputManagerTabId);
	if (InputManagerTab)
	{
		TSharedRef<SWidget> TabContent = InputManagerTab->GetContent();
		TSharedPtr<SIndependentInputDeviceManager> DeviceManager = StaticCastSharedPtr<SIndependentInputDeviceManager>(TabContent.ToSharedPtr());
		return DeviceManager;
	}
	
	return nullptr;
}

void UIndependentInputEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency(UIndependentInputSubsystem::StaticClass());
	
	if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
	{
		InputSubsystem->OnDevicePluggedInFirstTime.AddUniqueDynamic(this, &ThisClass::OnDevicePluggedInFirstTime);
		InputSubsystem->OnDevicePluggedIn.AddUniqueDynamic(this, &ThisClass::OnDevicePluggedIn);
		InputSubsystem->OnDeviceUnplugged.AddUniqueDynamic(this, &ThisClass::OnDeviceUnplugged);
		InputSubsystem->OnDeviceInfoUpdated.AddUniqueDynamic(this, &ThisClass::OnDeviceInfoUpdated);
	}
	
}

void UIndependentInputEditorSubsystem::Deinitialize()
{
	if (UIndependentInputSubsystem* InputSubsystem = UIndependentInputSubsystem::Get())
	{
		InputSubsystem->OnDevicePluggedInFirstTime.RemoveDynamic(this, &ThisClass::OnDevicePluggedInFirstTime);
		InputSubsystem->OnDevicePluggedIn.RemoveDynamic(this, &ThisClass::OnDevicePluggedIn);
		InputSubsystem->OnDeviceUnplugged.RemoveDynamic(this, &ThisClass::OnDeviceUnplugged);
		InputSubsystem->OnDeviceInfoUpdated.RemoveDynamic(this, &ThisClass::OnDeviceInfoUpdated);
	}

	Super::Deinitialize();
}

void UIndependentInputEditorSubsystem::OnDevicePluggedInFirstTime(const FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice)
{
	
	if (TSharedPtr<SIndependentInputDeviceManager> InputDeviceManager = UIndependentInputEditorSubsystem::FindInputManagerTab())
	{
		InputDeviceManager->DevicePluggedIn(DeviceInfo);
		return;
	}

	SIndependentInputDeviceManager::Open(DeviceInfo);
}

void UIndependentInputEditorSubsystem::OnDevicePluggedIn(const FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice)
{
	if (TSharedPtr<SIndependentInputDeviceManager> InputDeviceManager = UIndependentInputEditorSubsystem::FindInputManagerTab())
		InputDeviceManager->DevicePluggedIn(DeviceInfo);
}

void UIndependentInputEditorSubsystem::OnDeviceUnplugged(const FJoystickDeviceInfo& DeviceInfo, const FSDLJoystickDevice& SDLDevice)
{
	if (TSharedPtr<SIndependentInputDeviceManager> InputDeviceManager = UIndependentInputEditorSubsystem::FindInputManagerTab())
		InputDeviceManager->DeviceUnplugged(DeviceInfo);
}

void UIndependentInputEditorSubsystem::OnDeviceInfoUpdated(const FJoystickDeviceInfo& DeviceInfo)
{
	if (TSharedPtr<SIndependentInputDeviceManager> InputDeviceManager = UIndependentInputEditorSubsystem::FindInputManagerTab())
		InputDeviceManager->DeviceUpdated(DeviceInfo);
}