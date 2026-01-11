// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GameEndWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ThiefGameMode.h"

void UGameEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameEndWidget::OnRestartClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UGameEndWidget::OnMainMenuClicked);
	}
}

void UGameEndWidget::SetAsWinScreen()
{
	if (TitleText)
	{
		TitleText->SetText(WinTitle);
		// Sin cambio de color para Victory
	}

	if (SubtitleText)
	{
		SubtitleText->SetText(WinSubtitle);
	}
}

void UGameEndWidget::SetAsLoseScreen()
{
	if (TitleText)
	{
		TitleText->SetText(LoseTitle);
		// Sin cambio de color para Lose
	}

	if (SubtitleText)
	{
		SubtitleText->SetText(LoseSubtitle);
	}
}

void UGameEndWidget::OnRestartClicked()
{
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->RestartGame();
	}
}

void UGameEndWidget::OnMainMenuClicked()
{
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->GoToMainMenu();
	}
}
