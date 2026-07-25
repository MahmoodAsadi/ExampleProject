// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SSensorInputView.h"

#include "Brushes/SlateColorBrush.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SSensorInputView"


static const FNumberFormattingOptions ValueFormattingOptions = []()
	{
		FNumberFormattingOptions Options;
		Options.UseGrouping = false;
		Options.SetMinimumFractionalDigits(2);
		Options.SetMaximumFractionalDigits(2);
		return Options;
	}();

void SSensorInputView::Construct(const FArguments& InArgs)
{
	ProgressBarStyle = FAppStyle::Get().GetWidgetStyle<FProgressBarStyle>("ProgressBar");

	ProgressBarStyle
		.SetBackgroundImage(FSlateColorBrush(FLinearColor::Transparent))
		.SetFillImage(FSlateColorBrush(FLinearColor::White));

	SensorType = InArgs._SensorType;
	SensorMapping = InArgs._SensorMapping;
	SensorState = InArgs._SensorState;
	OnClicked = InArgs._OnClicked;

	auto CardContent = SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.HeightOverride(120.0f)
			[
				CreateProgressBar(LOCTEXT("XLabel", "X"), EAxis::X)
			]
		]
	
		+ SHorizontalBox::Slot()
		.Padding(30.0f, 0.0f)
		[
			SNew(SBox)
			.HeightOverride(120.0f)
			[
				CreateProgressBar(LOCTEXT("YLabel", "Y"), EAxis::Y)
			]
		]

		+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.HeightOverride(120.0f)
			[
				CreateProgressBar(LOCTEXT("ZLabel", "Z"), EAxis::Z)
			]
		];


	ChildSlot
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(0.0f)
			.OnClicked(OnClicked)
			.ToolTipText(LOCTEXT("SensorInputViewTooltip", "Click to configure this Sensor mapping."))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.13f, 0.15f, 0.18f, 1.0f))
				.Padding(FMargin(12.0f, 10.0f))
				[
					SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(UEnum::GetDisplayValueAsText(SensorType))
						.TextStyle(FAppStyle::Get(), "NormalText")
						.Font(FAppStyle::GetFontStyle("NormalFontBold"))
					]

					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						CardContent
					]
				]
			]
		];
}

TSharedRef<SHorizontalBox> SSensorInputView::CreateProgressBar(const FText& ValueLabel, EAxis::Type Axis)
{
	return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		.AutoWidth()
		.Padding(0.0f, 0.0f, 5.0f, 0.0f)
		[
			SNew(SBox)
			.WidthOverride(55.0f)
			[
				SNew(STextBlock)
				.Text_Lambda([this, ValueLabel, Axis]
					{
						float RawValue = 0.0f;
						switch (Axis)
						{
						case EAxis::X: RawValue = SensorState.Get().Value.X; break;
						case EAxis::Y: RawValue = SensorState.Get().Value.Y; break;
						case EAxis::Z: RawValue = SensorState.Get().Value.Z; break;
						}
						return FText::Format(LOCTEXT("ValueFormat", "{0}: {1}"), ValueLabel,
							FText::AsNumber(RawValue, &ValueFormattingOptions));
					})
				.TextStyle(FAppStyle::Get(), "NormalText")
			]
		]

		+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(14.0f)
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FLinearColor(0.04f, 0.04f, 0.04f, 1.0f))
				.Padding(1.0f)
				[
					SNew(SVerticalBox)

					// Positive side
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					[
						SNew(SProgressBar)
						.Style(&ProgressBarStyle)
						.BarFillType(EProgressBarFillType::BottomToTop)
						.BarFillStyle(EProgressBarFillStyle::Scale)
						.FillColorAndOpacity(FLinearColor(0.1f, 0.65f, 0.22f, 1.0f))
						.Percent_Lambda([this, Axis]
							{
								float Percent = 0.0f;
								switch (SensorType)
								{
								case EDeviceSensorType::Accelerometer:
								case EDeviceSensorType::LeftAccelerometer:
								case EDeviceSensorType::RightAccelerometer:
									switch (Axis)
									{
									case EAxis::X: Percent = SensorState.Get().Value.GetSafeNormal().X; break;
									case EAxis::Y: Percent = SensorState.Get().Value.GetSafeNormal().Y; break;
									case EAxis::Z: Percent = SensorState.Get().Value.GetSafeNormal().Z; break;
									}
									break;
								case EDeviceSensorType::Gyroscope:
								case EDeviceSensorType::LeftGyroscope:
								case EDeviceSensorType::RightGyroscope:
									switch (Axis)
									{
									case EAxis::X: Percent = FMath::Clamp(SensorState.Get().Value.X, -1.0f, 1.0f); break;
									case EAxis::Y: Percent = FMath::Clamp(SensorState.Get().Value.Y, -1.0f, 1.0f); break;
									case EAxis::Z: Percent = FMath::Clamp(SensorState.Get().Value.Z, -1.0f, 1.0f); break;
									}
									break;
								}
								
								if (Percent <= 0.0f)
									return 0.0f;

								return FMath::Clamp(Percent, 0.0f, 1.0f);
							})
					]

					// Dead-zone center marker
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(2.0f)
						[
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("WhiteBrush"))
							.BorderBackgroundColor(FLinearColor(0.55f, 0.55f, 0.55f, 1.0f))
							.Padding(0.0f)
						]
					]

					// Negative side
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					[
						SNew(SProgressBar)
						.Style(&ProgressBarStyle)
						.BarFillType(EProgressBarFillType::TopToBottom)
						.BarFillStyle(EProgressBarFillStyle::Scale)
						.FillColorAndOpacity(FLinearColor(0.75f, 0.12f, 0.12f, 1.0f))
						.Percent_Lambda([this, Axis]
							{
								float Percent = 0.0f;
								switch (SensorType)
								{
								case EDeviceSensorType::Accelerometer:
								case EDeviceSensorType::LeftAccelerometer:
								case EDeviceSensorType::RightAccelerometer:
									switch (Axis)
									{
									case EAxis::X: Percent = SensorState.Get().Value.GetSafeNormal().X; break;
									case EAxis::Y: Percent = SensorState.Get().Value.GetSafeNormal().Y; break;
									case EAxis::Z: Percent = SensorState.Get().Value.GetSafeNormal().Z; break;
									}
									break;
								case EDeviceSensorType::Gyroscope:
								case EDeviceSensorType::LeftGyroscope:
								case EDeviceSensorType::RightGyroscope:
									switch (Axis)
									{
									case EAxis::X: Percent = FMath::Clamp(SensorState.Get().Value.X, -1.0f, 1.0f); break;
									case EAxis::Y: Percent = FMath::Clamp(SensorState.Get().Value.Y, -1.0f, 1.0f); break;
									case EAxis::Z: Percent = FMath::Clamp(SensorState.Get().Value.Z, -1.0f, 1.0f); break;
									}
									break;
								}
								
								if (Percent >= 0.0f)
									return 0.0f;

								return FMath::Clamp(FMath::Abs(Percent), 0.0f, 1.0f);
							})
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
