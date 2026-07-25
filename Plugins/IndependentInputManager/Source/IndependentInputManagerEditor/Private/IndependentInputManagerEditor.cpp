// Fill out your copyright notice in the Description page of Project Settings.

#include "IndependentInputManagerEditor.h"

#include "ToolMenus.h"
#include "PropertyEditorModule.h"
#include "Widgets/Docking/SDockTab.h"

#include "IndependentInputEditorSubsystem.h"
#include "IndependentInputManagerSettings.h"
#include "IndependentInputManagerSettingsDetails.h"
#include "Slates/SIndependentInputDeviceManager.h"

#define LOCTEXT_NAMESPACE "FIndependentInputManagerEditorModule"


void FIndependentInputManagerEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		InputManagerTabId,
		FOnSpawnTab::CreateRaw(this, &FIndependentInputManagerEditorModule::SpawnInputDeviceManagerTab))
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.PadEvent_16x"))
		.SetDisplayName(LOCTEXT("InputDeviceManagerTabDisplayNameLabel", "Input Device Manager"))
		.SetTooltipText(LOCTEXT("InputDeviceManagerTabTooltipLabel", "Open the Independent Input Device Manager."))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(
			this, &FIndependentInputManagerEditorModule::RegisterMenus));

	RegisterPropertyLayout();
}

void FIndependentInputManagerEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(InputManagerTabId);
	UnregisterPropertyLayout();
}

TSharedRef<SDockTab> FIndependentInputManagerEditorModule::SpawnInputDeviceManagerTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SIndependentInputDeviceManager)
		];
}

void FIndependentInputManagerEditorModule::RegisterMenus()
{
	UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"));
	FToolMenuSection& Section = ToolsMenu->FindOrAddSection(TEXT("IndependentInputManager"));

	Section.AddMenuEntry(
		TEXT("OpenInputDeviceManager"),
		LOCTEXT("InputDeviceManagerTabLabel", "Input Device Manager"),
		LOCTEXT("InputDeviceManagerTabTooltipLabel", "Open the Independent Input Device Manager."),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.PadEvent_16x"),
		FUIAction(FExecuteAction::CreateLambda([]
			{
				FGlobalTabmanager::Get()->TryInvokeTab(InputManagerTabId);
			}))
	);
}

void FIndependentInputManagerEditorModule::RegisterPropertyLayout() const
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->RegisterCustomClassLayout(UIndependentInputManagerSettings::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&FIndependentInputManagerSettingsDetails::MakeInstance));
	}
}

void FIndependentInputManagerEditorModule::UnregisterPropertyLayout() const
{
	if (FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		PropertyModule->UnregisterCustomClassLayout(UIndependentInputManagerSettings::StaticClass()->GetFName());
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FIndependentInputManagerEditorModule, IndependentInputManagerEditor)