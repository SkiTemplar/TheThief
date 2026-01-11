// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Game/ThiefGameMode.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnRestartClicked);
	}

	if (ControlsButton)
	{
		ControlsButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnControlsClicked);
	}

	if (HowToPlayButton)
	{
		HowToPlayButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnHowToPlayClicked);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnMainMenuClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	CloseSubMenu();
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->ResumeGame();
	}
}

void UPauseMenuWidget::OnRestartClicked()
{
	CloseSubMenu();
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->RestartGame();
	}
}

void UPauseMenuWidget::OnControlsClicked()
{
	CloseSubMenu();

	if (ControlsWidgetClass)
	{
		CurrentSubMenu = CreateWidget<UUserWidget>(this, ControlsWidgetClass);
		if (CurrentSubMenu)
		{
			CurrentSubMenu->AddToViewport(25);
		}
	}
}

void UPauseMenuWidget::OnHowToPlayClicked()
{
	CloseSubMenu();

	if (HowToPlayWidgetClass)
	{
		CurrentSubMenu = CreateWidget<UUserWidget>(this, HowToPlayWidgetClass);
		if (CurrentSubMenu)
		{
			CurrentSubMenu->AddToViewport(25);
		}
	}
}

void UPauseMenuWidget::OnMainMenuClicked()
{
	CloseSubMenu();
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->GoToMainMenu();
	}
}

void UPauseMenuWidget::OnQuitClicked()
{
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->QuitGame();
	}
}

void UPauseMenuWidget::CloseSubMenu()
{
	if (CurrentSubMenu)
	{
		CurrentSubMenu->RemoveFromParent();
		CurrentSubMenu = nullptr;
	}
}
