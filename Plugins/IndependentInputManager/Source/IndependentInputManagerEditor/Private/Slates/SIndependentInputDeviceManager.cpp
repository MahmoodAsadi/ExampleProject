// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SIndependentInputDeviceManager.h"

#include "Framework/Docking/TabManager.h"
#include "Styling/AppStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"

#include "IndependentInputEditorSubsystem.h"
#include "Slates/SConnectedDeviceInfo.h"
#include "Slates/SDeviceKeyMapping.h"

#define LOCTEXT_NAMESPACE "SIndependentInputDeviceManager "


SIndependentInputDeviceManager::SIndependentInputDeviceManager()
{
	DefaultStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("FlatButton.DarkGrey");
	DefaultStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.12f, 0.12f, 0.12f, 0.8f));
	DefaultStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f));

	SelectedStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("FlatButton.Default");
	SelectedStyle.Normal.TintColor = FSlateColor(FLinearColor(0.18f, 0.18f, 0.18f, 0.8f));
	SelectedStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.25f, 0.25f, 0.25f, 0.8f));
	SelectedStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.1f, 0.1f, 0.1f, 0.8f));
}

TSharedPtr<SIndependentInputDeviceManager> SIndependentInputDeviceManager::Open(const FJoystickDeviceInfo& DeviceInfo)
{
	TSharedPtr<SDockTab> InputManagerTab = FGlobalTabmanager::Get()->TryInvokeTab(InputManagerTabId);
	if (!InputManagerTab.IsValid())
		return nullptr;

	TSharedRef<SWidget> TabContent = InputManagerTab->GetContent();
	TSharedPtr<SIndependentInputDeviceManager> DeviceManager = StaticCastSharedPtr<SIndependentInputDeviceManager>(TabContent.ToSharedPtr());
	if (DeviceManager)
		DeviceManager->DevicePluggedIn(DeviceInfo);

	return DeviceManager;
}

void SIndependentInputDeviceManager::Construct(const FArguments& InArgs)
{
	const FSlateBrush* BackgroundBrush = FAppStyle::GetBrush("ToolPanel.GroupBorder");
	
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SHorizontalBox)

			// Tabs button slot
			+ SHorizontalBox::Slot()
			.Padding(10.0f, 10.0f, 2.0f, 10.0f)
			.AutoWidth()
			[
				SNew(SBorder)
				.BorderImage(BackgroundBrush)
				.Padding(5.0f)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				[
					// Tabs vertical box.
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Fill)
					.Padding(0.0f, 2.5f)
					[
						SAssignNew(ConnectedDevicesButton, SButton)
						.OnClicked(this, &SIndependentInputDeviceManager::ConnectedDevicesButtonClicked)
						.ContentPadding(FMargin(0.0f, 5.0f, 20.0f, 5.0f))
						.Text(LOCTEXT("ConnectedDevicesLabel", "Connected Devices"))
						.TextStyle(FAppStyle::Get(), "DialogButtonText")
						.HAlign(HAlign_Left)
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Fill)
					.Padding(0.0f, 2.5f)
					[
						SAssignNew(KeyMappingButton, SButton)
						.OnClicked(this, &SIndependentInputDeviceManager::KeyMappingButtonClicked)
						.ContentPadding(FMargin(0.0f, 5.0f, 20.0f, 5.0f))
						.Text(LOCTEXT("ProfilesLabel", "Key Mappings"))
						.TextStyle(FAppStyle::Get(), "DialogButtonText")
						.HAlign(HAlign_Left)
					]
				]
			]

			// Selected Tab content slot
			+ SHorizontalBox::Slot()
			.Padding(2.0f, 10.0f, 10.0f, 10.0f)
			.FillWidth(1.0f)
			[
				SAssignNew(ContentWidgetSwitcher, SWidgetSwitcher)
				+ SWidgetSwitcher::Slot()
				[
					SAssignNew(ConnectedDeviceTab, SConnectedDeviceInfo)
				]

				+ SWidgetSwitcher::Slot()
				[
					SAssignNew(KeyMappingTab, SDeviceKeyMapping)
				]
			]
		]
	];
	
	if (SelectedTabIndex == INDEX_NONE)
		SelectedTabIndex = 0;

	UpdateTabButtons();
}

void SIndependentInputDeviceManager::DevicePluggedIn(const FJoystickDeviceInfo& DeviceInfo)
{
	SelectedDeviceId = DeviceInfo.InstanceId;
	if (ConnectedDeviceTab)
		ConnectedDeviceTab->DevicePluggedIn(DeviceInfo);

	if (KeyMappingTab)
		KeyMappingTab->DevicePluggedIn(DeviceInfo);
}

void SIndependentInputDeviceManager::DeviceUnplugged(const FJoystickDeviceInfo& DeviceInfo)
{
	if (SelectedDeviceId.Equals(DeviceInfo.InstanceId))
		SelectedDeviceId = FInputDeviceInstanceId();

	if (ConnectedDeviceTab)
		ConnectedDeviceTab->DeviceUnplugged(DeviceInfo);

	if (KeyMappingTab)
		KeyMappingTab->DeviceUnplugged(DeviceInfo);
}

void SIndependentInputDeviceManager::DeviceUpdated(const FJoystickDeviceInfo& DeviceInfo)
{
	if (ConnectedDeviceTab)
		ConnectedDeviceTab->DeviceInfoUpdated(DeviceInfo);
}

FReply SIndependentInputDeviceManager::ConnectedDevicesButtonClicked()
{
	SelectedTabIndex = 0;
	RefreshWindow();
	return FReply::Handled();
}

FReply SIndependentInputDeviceManager::KeyMappingButtonClicked()
{
	SelectedTabIndex = 1;
	RefreshWindow();
	return FReply::Handled();
}

void SIndependentInputDeviceManager::RefreshWindow()
{
	UpdateTabButtons();
	if (ContentWidgetSwitcher)
	{
		if (SelectedTabIndex < ContentWidgetSwitcher->GetNumWidgets() && SelectedTabIndex >= 0)
			ContentWidgetSwitcher->SetActiveWidgetIndex(SelectedTabIndex);
	}
}

void SIndependentInputDeviceManager::UpdateTabButtons()
{
	ConnectedDevicesButton->SetButtonStyle(SelectedTabIndex == 0 ? &SelectedStyle : &DefaultStyle);
	KeyMappingButton->SetButtonStyle(SelectedTabIndex == 1 ? &SelectedStyle : &DefaultStyle);
}

#undef LOCTEXT_NAMESPACE
