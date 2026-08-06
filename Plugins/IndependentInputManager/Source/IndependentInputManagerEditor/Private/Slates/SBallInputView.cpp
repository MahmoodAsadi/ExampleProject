// Fill out your copyright notice in the Description page of Project Settings.

#include "Slates/SBallInputView.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SConstraintCanvas.h"

#define LOCTEXT_NAMESPACE "SBallInputView"


void SBallInputView::Construct(const FArguments& InArgs)
{
	BallMapping = InArgs._BallMapping;
	BallState = InArgs._BallState;
	OnClicked = InArgs._OnClicked;

	auto CardContent = SNew(SConstraintCanvas)

		// Center marker
		+ SConstraintCanvas::Slot()
		.Anchors(FAnchors(0.5f, 0.5f))
		.Alignment(FVector2D(0.5f, 0.5f))
		.AutoSize(true)
		.ZOrder(1)
		[
			SNew(STextBlock)
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			.Text(FText::FromString(TEXT("●")))
			.ShadowOffset(FVector2D::ZeroVector)
			.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 9.0f))
			.ColorAndOpacity(FLinearColor(0.15f, 0.85f, 0.25f, 1.0f))
		]

		// Value marker
		+ SConstraintCanvas::Slot()
		.Anchors(TAttribute<FAnchors>::CreateLambda([this]
			{
				float DeltaSec = 0.003;
				if (GEditor && GEditor->GetWorld())
					DeltaSec = GEditor->GetWorld()->GetDeltaSeconds();

				const FVector2D CurrentBallState = BallState.Get();
				FVector2D NewValue = FVector2D(
					FMath::GetMappedRangeValueClamped(FVector2D(-10.0f, 10.0f), FVector2D(0.0f, 1.0f), CurrentBallState.X),
					FMath::GetMappedRangeValueClamped(FVector2D(-10.0f, 10.0f), FVector2D(0.0f, 1.0f), CurrentBallState.Y));

				const bool bReturningToCenter = (NewValue - FVector2D(0.5f, 0.5f)).IsNearlyZero();
				const float InterpSpeed = bReturningToCenter ? 8.0f : 20.0f;

				InterpValue = FMath::Vector2DInterpTo(InterpValue, NewValue, DeltaSec, InterpSpeed);
				return FAnchors(InterpValue.X, InterpValue.Y);
			}))
		.Alignment(FVector2D(0.5f, 0.5f))
		.AutoSize(true)
		.ZOrder(2)
		[
			SNew(STextBlock)
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			.Text(FText::FromString(TEXT("●")))
			.ShadowOffset(FVector2D::ZeroVector)
			.Font(FCoreStyle::GetDefaultFontStyle(TEXT("Regular"), 9.0f))
			.ColorAndOpacity(FLinearColor(0.15f, 0.85f, 0.25f, 1.0f))
		]

		// Connection line
		+ SConstraintCanvas::Slot()
		.Anchors(TAttribute<FAnchors>::CreateLambda([this]
			{
				FVector2D Position = InterpValue - FVector2D(0.5f);
				Position /= 2.0f;
				Position += FVector2D(0.5f);
				return FAnchors(Position.X, Position.Y);
			}))
		.Alignment(FVector2D(0.5f, 0.5f))
		.AutoSize(true)
		.ZOrder(0)
		[
			SNew(SBox)
			.WidthOverride(1.0f)
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			.RenderTransform_Lambda([this]
				{
					const FVector2D Position = InterpValue - FVector2D(0.5f);

					if (Position.IsNearlyZero())
						return FSlateRenderTransform();

					// Angle from the center toward the moving dot.
					const float TargetAngleRadians = FMath::Atan2(Position.Y, Position.X);

					/*
					 * The unrotated SBox points downward because its height is the
					 * line length. Subtract 90 degrees to align that vertical line
					 * with the target direction.
					 */
					const float RotationRadians = TargetAngleRadians - UE_HALF_PI;

					return FSlateRenderTransform(FQuat2D(RotationRadians));
				})
			.HeightOverride_Lambda([this]
				{
					FVector2D Position = InterpValue - FVector2D(0.5f);
					Position *= 120.0f;
					return Position.Size();
				})
			[
				SNew(SImage)
				.ColorAndOpacity(FLinearColor(0.15f, 0.85f, 0.25f, 1.0f))
			]
		];


	ChildSlot
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.ContentPadding(0.0f)
			.OnClicked(OnClicked)
			.ToolTipText(LOCTEXT("BallInputViewTooltip", "Click to configure this Ball Mapping."))
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
								return FText::Format(LOCTEXT("BallIndexFormat", "Ball {0}"),
									FText::AsNumber(BallMapping.BallIndex));
							})
						.TextStyle(FAppStyle::Get(), "NormalText")
						.Font(FAppStyle::GetFontStyle("NormalFontBold"))
					]

					// Content
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 10.0f, 0.0f, 0.0f)
					[
						// Content border
						SNew(SBorder)
						.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
						.BorderBackgroundColor(FLinearColor(0.45f, 0.45f, 0.45f, 1.0f))
						.Padding(1.0f)
						[
							SNew(SBox)
							.MinDesiredWidth(120.0f)
							.HeightOverride(120.0f)
							[
								CardContent
							]
						]
					]
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE
