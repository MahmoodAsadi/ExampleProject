// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SAxisInputView.h"

#include "Brushes/SlateColorBrush.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SAxisInputView"


void SAxisInputView::Construct(const FArguments& InArgs)
{
	AxisProgressBarStyle = FAppStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar");

	AxisProgressBarStyle
		.SetBackgroundImage(FSlateColorBrush(FLinearColor::Transparent))
		.SetFillImage(FSlateColorBrush(FLinearColor::White));

	AxisMapping = InArgs._AxisMapping;
	RawInputValue = InArgs._RawInputValue;
	OutputInputValue = InArgs._OutputInputValue;
	VirtualButtonStates = InArgs._VirtualButtonStates;
	OnClicked = InArgs._OnClicked;

	static const FNumberFormattingOptions InputValueFormattingOptions = []()
		{
			FNumberFormattingOptions Options;
			Options.UseGrouping = false;
			return Options;
		}();

	static const FNumberFormattingOptions AxisValueFormattingOptions = []()
		{
			FNumberFormattingOptions Options;
			Options.UseGrouping = false;
			Options.SetMinimumFractionalDigits(2);
			Options.SetMaximumFractionalDigits(2);
			return Options;
		}();

	static const FNumberFormattingOptions RangeValueFormattingOptions = []()
		{
			FNumberFormattingOptions Options;
			Options.UseGrouping = false;
			Options.SetMinimumFractionalDigits(0);
			Options.SetMaximumFractionalDigits(2);
			return Options;
		}();

	const auto GetKeyDisplayName = [](const FIndependentInputKey& IndependentKey)
		{
			return IndependentKey.IsValid()
				? FText::FromString(IndependentKey.GetKeyDisplayName())
				: LOCTEXT("AxisKeyNotAssigned", "Not Assigned");
		};

	const auto IsVirtualButtonPressed = [this](const int32 VirtualButtonIndex)
		{
			if (!VirtualButtonStates.IsValidIndex(VirtualButtonIndex))
				return false;

			return VirtualButtonStates[VirtualButtonIndex].Get(false);
		};

	const auto GetAxisRange = [this](const bool bOutputRange)
		{
			if (!bOutputRange)
				return AxisMapping.InputRange;

			// When remapping is disabled, output should use the same
			// visual range as the raw input.
			return AxisMapping.bRemap ? AxisMapping.OutputRange : AxisMapping.InputRange;
		};

	const auto CreateAxisValueView = [this, GetAxisRange](const FText& ValueLabel, const TAttribute<float>& ValueAttribute, const bool bOutputRange)
		{
			const FAxisValueRange Range = GetAxisRange(bOutputRange);
			const float DeadZoneCenter = AxisMapping.DeadZoneCenter;
			const float LeftRange = FMath::Max(DeadZoneCenter - FMath::Min(Range.Min, Range.Max), KINDA_SMALL_NUMBER);
			const float RightRange = FMath::Max(FMath::Max(Range.Min, Range.Max) - DeadZoneCenter, KINDA_SMALL_NUMBER);

			return SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text_Lambda([ValueLabel, ValueAttribute, bOutputRange, DeadZoneCenter]
						{
							const float RawValue = ValueAttribute.Get(DeadZoneCenter);
							return FText::Format(LOCTEXT("AxisValueFormat", "{0}: {1}"), ValueLabel,
								FText::AsNumber(FMath::IsNearlyZero(RawValue) ? 0.0f : RawValue, bOutputRange
									? &AxisValueFormattingOptions
									: &InputValueFormattingOptions));
						})
					.TextStyle(FAppStyle::Get(), "NormalText")
				]

				// Min ---- progress bar ---- Max
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 4.0f, 0.0f, 0.0f)
				[
					SNew(SHorizontalBox)

					// Minimum range
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0.0f, 0.0f, 6.0f, 0.0f)
					[
						SNew(SBox)
						.MinDesiredWidth(28.0f)
						.HAlign(HAlign_Right)
						[
							SNew(STextBlock)
							.Text_Lambda([GetAxisRange, bOutputRange]()
								{
									const FAxisValueRange Range = GetAxisRange(bOutputRange);
									const float RawValue = FMath::Min(Range.Min, Range.Max);
									return FText::AsNumber(FMath::IsNearlyZero(RawValue) ? 0.0f : RawValue, bOutputRange
										? &RangeValueFormattingOptions
										: &InputValueFormattingOptions);
								})
							.TextStyle(FAppStyle::Get(), "SmallText")
							.ColorAndOpacity(FSlateColor::UseSubduedForeground())
						]
					]

					// Axis progress
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					[
						SNew(SBox)
						.HeightOverride(14.0f)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
							.BorderBackgroundColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.0f))
							.Padding(1.0f)
							[
								SNew(SHorizontalBox)

								// Negative side
								+ SHorizontalBox::Slot()
								.FillWidth(LeftRange)
								[
									SNew(SProgressBar)
									.Style(&AxisProgressBarStyle)
									.BarFillType(EProgressBarFillType::RightToLeft)
									.BarFillStyle(EProgressBarFillStyle::Scale)
									.FillColorAndOpacity(FLinearColor(0.75f, 0.12f, 0.12f, 1.0f))
									//.FillColorAndOpacity(FLinearColor(0.0f, 0.45f, 1.0f, 1.0f))
									.Percent_Lambda([this, LeftRange, DeadZoneCenter, ValueAttribute]
										{
											const float Value = ValueAttribute.Get(DeadZoneCenter);
											if (Value >= DeadZoneCenter)
												return 0.0f;

											return FMath::Clamp((DeadZoneCenter - Value) / LeftRange, 0.0f, 1.0f);
										})
								]

								// Dead-zone center marker
								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(SBox)
									.WidthOverride(2.0f)
									[
										SNew(SBorder)
										.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
										.BorderBackgroundColor(FLinearColor(0.55f, 0.55f, 0.55f, 1.0f))
										.Padding(0.0f)
									]
								]

								// Positive side
								+ SHorizontalBox::Slot()
								.FillWidth(RightRange)
								[
									SNew(SProgressBar)
									.Style(&AxisProgressBarStyle)
									.BarFillType(EProgressBarFillType::LeftToRight)
									.BarFillStyle(EProgressBarFillStyle::Scale)
									.FillColorAndOpacity(FLinearColor(0.1f, 0.65f, 0.22f, 1.0f))
									.Percent_Lambda([this, RightRange, ValueAttribute, DeadZoneCenter]
										{
											const float Value = ValueAttribute.Get(DeadZoneCenter);
											if (Value <= DeadZoneCenter)
												return 0.0f;

											return FMath::Clamp(
												(Value - DeadZoneCenter) / RightRange,
												0.0f,
												1.0f);
										})
								]
							]
						]
					]

					// Maximum range
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(6.0f, 0.0f, 0.0f, 0.0f)
					[
						SNew(SBox)
						.MinDesiredWidth(28.0f)
						.HAlign(HAlign_Left)
						[
							SNew(STextBlock)
							.Text_Lambda([GetAxisRange, bOutputRange]
								{
									const FAxisValueRange Range = GetAxisRange(bOutputRange);
									const float RawValue = FMath::Max(Range.Min, Range.Max);
									return FText::AsNumber(FMath::IsNearlyZero(RawValue) ? 0.0f : RawValue, bOutputRange
										? &RangeValueFormattingOptions
										: &InputValueFormattingOptions);
								})
							.TextStyle(FAppStyle::Get(), "SmallText")
							.ColorAndOpacity(FSlateColor::UseSubduedForeground())
						]
					]
				];
		};

	TSharedRef<SVerticalBox> CardContent = SNew(SVerticalBox);

	// Axis index
	CardContent->AddSlot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::Printf(TEXT("Axis %d"), AxisMapping.AxisIndex)))
			.TextStyle(FAppStyle::Get(), "NormalText")
			.Font(FAppStyle::GetFontStyle("NormalFontBold"))
		];

	// Assigned key
	CardContent->AddSlot()
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
				.Text(LOCTEXT("AxisKeyLabel", "Key:"))
				.TextStyle(FAppStyle::Get(), "NormalText")
				.ColorAndOpacity(FSlateColor::UseSubduedForeground())
			]

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(GetKeyDisplayName(AxisMapping.Key))
				.TextStyle(FAppStyle::Get(), "NormalText")
				.ColorAndOpacity_Lambda(
					[this]()
					{
						const FKey Key =
							AxisMapping.Key.GetKey();

						return Key.IsValid()
							? FSlateColor::UseForeground()
							: FSlateColor::
							UseSubduedForeground();
					})
			]
		];

	// Raw input progress
	CardContent->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 14.0f, 0.0f, 0.0f)
		[
			CreateAxisValueView(LOCTEXT("RawAxisInputLabel", "Input"), RawInputValue, false)
		];

	// Output progress
	CardContent->AddSlot()
		.AutoHeight()
		.Padding(0.0f, 12.0f, 0.0f, 0.0f)
		[
			CreateAxisValueView(LOCTEXT("AxisOutputLabel", "Output"), OutputInputValue, true)
		];

	// Add the virtual-key section only when this axis has virtual buttons.
	if (!AxisMapping.VirtualButtons.IsEmpty())
	{
		TSharedRef<SVerticalBox> VirtualButtonsContainer = SNew(SVerticalBox);

		for (int32 VirtualButtonIndex = 0; VirtualButtonIndex < AxisMapping.VirtualButtons.Num(); ++VirtualButtonIndex)
		{
			const FAxisVirtualButtonKeyMapping& VirtualButtonMapping = AxisMapping.VirtualButtons[VirtualButtonIndex];
			const FText VirtualKeyDisplayName = GetKeyDisplayName(VirtualButtonMapping.Key);

			VirtualButtonsContainer->AddSlot()
				.AutoHeight()
				.Padding(0.0f, VirtualButtonIndex == 0 ? 0.0f : 6.0f, 0.0f, 0.0f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
					.BorderBackgroundColor_Lambda([IsVirtualButtonPressed, VirtualButtonIndex]
						{
							return IsVirtualButtonPressed(VirtualButtonIndex)
								? FLinearColor(0.15f, 0.85f, 0.25f, 1.0f)
								: FLinearColor(0.35f, 0.35f, 0.35f, 1.0f);
						})
					.Padding(FMargin(8.0f, 6.0f))
					[
						SNew(SHorizontalBox)

						// Virtual key
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						[
							SNew(STextBlock)
							.Text(VirtualKeyDisplayName)
							.TextStyle(FAppStyle::Get(), "NormalText")
						]

						// Live state
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.Padding(10.0f, 0.0f, 0.0f, 0.0f)
						[
							SNew(SOverlay)

							// Fake hidden state to keep the slot size persistant.
							+ SOverlay::Slot()
							.HAlign(HAlign_Left)
							[
								SNew(STextBlock)
								.Visibility(EVisibility::Hidden)
								.Text(LOCTEXT("VirtualButtonReleased", "○ Released"))
								.TextStyle(FAppStyle::Get(), "NormalText")
							]

							+ SOverlay::Slot()
							.HAlign(HAlign_Left)
							[
								SNew(STextBlock)
								.Text_Lambda([IsVirtualButtonPressed, VirtualButtonIndex]
									{
										return IsVirtualButtonPressed(VirtualButtonIndex)
											? LOCTEXT("VirtualButtonPressed", "● Pressed")
											: LOCTEXT("VirtualButtonReleased", "○ Released");
									})
								.TextStyle(FAppStyle::Get(), "NormalText")
								.ColorAndOpacity_Lambda([IsVirtualButtonPressed, VirtualButtonIndex]
									{
										return IsVirtualButtonPressed(VirtualButtonIndex)
											? FSlateColor(FLinearColor(0.15f, 1.0f, 0.25f, 1.0f))
											: FSlateColor::UseSubduedForeground();
									})
							]
						]
					]
				];
		}

		CardContent->AddSlot()
			.AutoHeight()
			.Padding(0.0f, 14.0f, 0.0f, 0.0f)
			[
				SNew(SVerticalBox)

				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 0.0f, 6.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("AxisVirtualKeysLabel", "Virtual Keys:"))
					.TextStyle(FAppStyle::Get(), "NormalText")
					.Font(FAppStyle::GetFontStyle("NormalFontBold"))
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					VirtualButtonsContainer
				]
			];
	}

	ChildSlot
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(0.0f)
			.OnClicked(OnClicked)
			.ToolTipText(LOCTEXT("AxisInputViewTooltip", "Click to configure this Axis Mapping."))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.13f, 0.15f, 0.18f, 1.0f))
				.Padding(FMargin(12.0f, 10.0f))
				[
					CardContent
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
