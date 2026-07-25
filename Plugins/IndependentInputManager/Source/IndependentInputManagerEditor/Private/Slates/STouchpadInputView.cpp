// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/STouchpadInputView.h"

#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "STouchpadInputView"


void STouchpadInputView::Construct(const FArguments& InArgs)
{
	TouchpadMapping = InArgs._TouchpadMapping;
	FingersState = InArgs._FingersState;
	OnClicked = InArgs._OnClicked;

	TSharedRef<SConstraintCanvas> TouchSurface = SNew(SConstraintCanvas);
	for (const FJoystickTouchpadFingerKeyMapping& FingerMapping : TouchpadMapping.Fingers)
	{
		const int32 FingerIndex = FingerMapping.FingerIndex;

		TouchSurface->AddSlot()
		.Anchors(TAttribute<FAnchors>::CreateLambda([this, FingerIndex]
			{
				if (!FingersState.IsValidIndex(FingerIndex))
					return FAnchors(0.0f);

				FVector2D Position(FingersState[FingerIndex].Get().X.GetValue(), FingersState[FingerIndex].Get().Y.GetValue());
				return FAnchors(Position.X, Position.Y);
			}))
		.Alignment(FVector2D(0.5f, 0.5f))
		.AutoSize(true)
		.ZOrder(1.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("●")))
			.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 16))
			.ColorAndOpacity(FLinearColor(0.15f, 0.85f, 0.25f, 1.0f))
			.Visibility(TAttribute<EVisibility>::CreateLambda([this, FingerIndex]
				{
					if (!FingersState.IsValidIndex(FingerIndex))
						return EVisibility::Collapsed;

					return FingersState[FingerIndex].Get().Touch.GetValue() ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
				}))
			.ToolTipText(FText::Format(LOCTEXT("FingerTooltipFormat", "Finger {0}"), FText::AsNumber(FingerIndex)))
		];
	}

	ChildSlot
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(0.0f)
			.OnClicked(OnClicked)
			.ToolTipText(LOCTEXT("TouchpadInputViewTooltip", "Click to configure this Touchpad Mapping."))
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
				.BorderBackgroundColor(FLinearColor(0.13f, 0.15f, 0.18f, 1.0f))
				.Padding(FMargin(12.0f, 10.0f))
				[
					SNew(SVerticalBox)

					// Title
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text_Lambda([this]()
							{
								return FText::Format(LOCTEXT("TouchpadIndexFormat", "Touchpad {0}"),
									FText::AsNumber(TouchpadMapping.TouchpadIndex));
							})
						.TextStyle(FAppStyle::Get(), "NormalText")
						.Font(FAppStyle::GetFontStyle("NormalFontBold"))
					]

					// Number of supported fingers
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 6.0f, 0.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text_Lambda([this]()
							{
								return FText::Format(LOCTEXT("SupportedFingerCountFormat", "Fingers: {0}"),
									FText::AsNumber(TouchpadMapping.Fingers.Num()));
							})
						.TextStyle(FAppStyle::Get(), "NormalText")
						.ColorAndOpacity(FSlateColor::UseSubduedForeground())
					]

					// Touch surface
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						SNew(SBox)
						.MinDesiredWidth(220.0f)
						.HeightOverride(120.0f)
						[
							// Touch surface border
							SNew(SBorder)
							.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
							.BorderBackgroundColor(FLinearColor(0.45f, 0.45f, 0.45f, 1.0f))
							.Padding(1.0f)
							[
								TouchSurface
							]
						]
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
