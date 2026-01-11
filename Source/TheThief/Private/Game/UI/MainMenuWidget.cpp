// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Game/ThiefGameMode.h"
#include "Game/ThiefGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PlayButton)
	{
		PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayClicked);
	}

	if (ControlsButton)
	{
		ControlsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnControlsClicked);
	}

	if (HowToPlayButton)
	{
		HowToPlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHowToPlayClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}

	// Mostrar cursor y configurar input
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void UMainMenuWidget::OnPlayClicked()
{
	// Cerrar submenús abiertos
	CloseSubMenu();
	
	// Llamar a StartGame en el GameMode
	if (AThiefGameMode* GM = Cast<AThiefGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->StartGame();
	}
}

void UMainMenuWidget::OnControlsClicked()
{
	CloseSubMenu();

	if (ControlsWidgetClass)
	{
		CurrentSubMenu = CreateWidget<UUserWidget>(this, ControlsWidgetClass);
		if (CurrentSubMenu)
		{
			CurrentSubMenu->AddToViewport(20);
		}
	}
}

void UMainMenuWidget::OnHowToPlayClicked()
{
	CloseSubMenu();

	if (HowToPlayWidgetClass)
	{
		CurrentSubMenu = CreateWidget<UUserWidget>(this, HowToPlayWidgetClass);
		if (CurrentSubMenu)
		{
			CurrentSubMenu->AddToViewport(20);
		}
	}
}

void UMainMenuWidget::OnQuitClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UMainMenuWidget::CloseSubMenu()
{
	if (CurrentSubMenu)
	{
		CurrentSubMenu->RemoveFromParent();
		CurrentSubMenu = nullptr;
	}
}
