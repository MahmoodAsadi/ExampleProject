// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SButtonInputView.h"

#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SButtonInputView"


void SButtonInputView::Construct(const FArguments& InArgs)
{
	ButtonIndex = InArgs._ButtonIndex;
	Key = InArgs._Key;
	IsPressed = InArgs._IsPressed;
	DeviceInstanceId = InArgs._DeviceInstanceId;
	OnClicked = InArgs._OnClicked;

	auto CardContent = SNew(SVerticalBox)

	// Button index
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text_Lambda([this]()
			{
				return FText::Format(LOCTEXT("ButtonIndexFormat", "Button {0}"), FText::AsNumber(ButtonIndex));
			})
		.TextStyle(FAppStyle::Get(), "NormalText")
		.Font(FAppStyle::GetFontStyle("NormalFontBold"))
	]

	// Assigned key
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 6.0f, 0.0f, 0.0f)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 5.0f, 0.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ButtonKeyLabel", "Key:"))
			.TextStyle(FAppStyle::Get(), "NormalText")
			.ColorAndOpacity(FSlateColor::UseSubduedForeground())
		]

		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
				{
					const FIndependentInputKey AssignedKey = Key.Get();

					return AssignedKey.IsValid()
						? FText::FromString(AssignedKey.GetKeyDisplayName())
						: LOCTEXT("ButtonNotAssigned", "Not Assigned");
				})
			.TextStyle(FAppStyle::Get(), "NormalText")
			.ColorAndOpacity_Lambda([this]()
				{
					return Key.Get().IsValid()
						? FSlateColor::UseForeground()
						: FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));
				})
		]
	]

	// Live button state
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 10.0f, 0.0f, 0.0f)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 7.0f, 0.0f)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
				{
					return IsPressed.Get(false)
						? FText::FromString(TEXT("●"))
						: FText::FromString(TEXT("○"));
				})
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
			.ColorAndOpacity_Lambda([this]()
				{
					return IsPressed.Get(false)
						? FSlateColor(FLinearColor(0.15f, 1.0f, 0.25f, 1.0f))
						: FSlateColor::UseSubduedForeground();
				})
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
				{
					return IsPressed.Get(false)
						? LOCTEXT("ButtonPressed", "Pressed")
						: LOCTEXT("ButtonReleased", "Released");
				})
			.TextStyle(FAppStyle::Get(), "NormalText")
			.ColorAndOpacity_Lambda([this]()
				{
					return IsPressed.Get(false)
						? FSlateColor(FLinearColor(0.7f, 1.0f, 0.7f, 1.0f))
						: FSlateColor::UseSubduedForeground();
				})
		]
	];

	ChildSlot
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(0.0f)
			.OnClicked(OnClicked)
			.ToolTipText(LOCTEXT("ButtonInputViewTooltip", "Click to configure this button mapping."))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(FMargin(12.0f, 10.0f))
				.BorderBackgroundColor_Lambda([this]()
					{
						return IsPressed.Get(false)
							? FLinearColor(0.15f, 0.85f, 0.25f, 1.0f)
							: FLinearColor(0.13f, 0.15f, 0.18f, 1.0f);
					})
				[
					CardContent
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE