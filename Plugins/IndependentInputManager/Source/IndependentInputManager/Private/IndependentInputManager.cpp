// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputManager.h"

#include "Features/IModularFeatures.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

#include "IndependentInputDevice.h"
#include "IndependentInputManagerTypes.h"
#include "IndependentInputSubsystem.h"

#define LOCTEXT_NAMESPACE "FIndependentInputManagerModule"

void FIndependentInputManagerModule::StartupModule()
{
#if PLATFORM_WINDOWS
	FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("IndependentInputManager"))->GetBaseDir();
	FString DLLPath = FPaths::Combine(PluginDir, TEXT("Source/ThirdParty/SDL3/Lib/Win64/SDL3.dll"));
	FPlatformProcess::PushDllDirectory(*FPaths::GetPath(DLLPath));
	SDLDllHandle = FPlatformProcess::GetDllHandle(*DLLPath);
	FPlatformProcess::PopDllDirectory(*FPaths::GetPath(DLLPath));

	if (!SDLDllHandle)
	{
		UE_LOG(LogIndependentInput, Warning, TEXT("IndependentInputManager: SDL3.dll not loaded; joystick input disabled."));
		return;
	}
#endif

	FSDLInputUtils::RegisterDefaultKeys();
	// Register as an input device factory so the engine calls CreateInputDevice()
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
}

void FIndependentInputManagerModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);

	if (SDLDllHandle)
	{
		FPlatformProcess::FreeDllHandle(SDLDllHandle);
		SDLDllHandle = nullptr;
	}
}

TSharedPtr<IInputDevice> FIndependentInputManagerModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	InputDevice = MakeShared<FIndependentInputDevice>(InMessageHandler);
	UIndependentInputSubsystem* IndependentInputSubsystem = GEngine->GetEngineSubsystem<UIndependentInputSubsystem>();
	IndependentInputSubsystem->InitializeInputDevice(InputDevice);
	return InputDevice;
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FIndependentInputManagerModule, IndependentInputManager)

DEFINE_LOG_CATEGORY(LogIndependentInput);
