// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/ThiefGameMode.h"
#include "Game/ThiefGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraActor.h"
#include "UI/GameEndWidget.h"

AThiefGameMode::AThiefGameMode()
{
	CurrentState = EThiefGameState::MainMenu;
	TotalCoinsInLevel = 0;
	PrimaryActorTick.bCanEverTick = true;
}

void AThiefGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentAmbientPitch = 1.0f;
	TargetAmbientPitch = 1.0f;

	// Buscar la cámara del menú
	MenuCamera = FindMenuCamera();

	// Limpiar sonidos anteriores por si acaso
	StopAllAmbientSounds();

	// Crear sonidos ambientales (NO persisten entre niveles)
	for (USoundBase* Sound : AmbientSoundsToPlay)
	{
		if (Sound)
		{
			UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(this, Sound, 1.0f, 1.0f, 0.0f, nullptr, false, false);
			if (AudioComp)
			{
				AudioComp->Play();
				AmbientAudioComponents.Add(AudioComp);
			}
		}
	}

	// Verificar si debemos saltar el Main Menu
	bool bShouldSkip = false;
	if (UThiefGameInstance* GI = Cast<UThiefGameInstance>(GetGameInstance()))
	{
		bShouldSkip = GI->bSkipMainMenuOnRestart;
		GI->bSkipMainMenuOnRestart = false;
	}

	// Decidir si empezar con Main Menu o gameplay
	if (bStartWithMainMenu && MainMenuWidgetClass && !bShouldSkip)
	{
		SetupMainMenu();
	}
	else
	{
		SetupGameplay();
	}
}

ACameraActor* AThiefGameMode::FindMenuCamera()
{
	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), MenuCameraTag, Cameras);
	
	if (Cameras.Num() > 0)
	{
		return Cast<ACameraActor>(Cameras[0]);
	}
	return nullptr;
}

void AThiefGameMode::SwitchToMenuCamera()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (MenuCamera)
		{
			PC->SetViewTargetWithBlend(MenuCamera, 0.0f);
		}
	}
}

void AThiefGameMode::SwitchToPlayerCamera()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			PC->SetViewTargetWithBlend(PlayerPawn, 0.5f);
		}
	}
}

void AThiefGameMode::SetupMainMenu()
{
	CurrentState = EThiefGameState::MainMenu;
	
	SwitchToMenuCamera();
	
	if (CurrentHUD)
	{
		CurrentHUD->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	ShowWidget(MainMenuWidgetClass);
	
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
		
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			PlayerPawn->DisableInput(PC);
		}
	}
}

void AThiefGameMode::SetupGameplay()
{
	CurrentState = EThiefGameState::Playing;
	
	SwitchToPlayerCamera();
	
	if (!CurrentHUD && HUDWidgetClass)
	{
		CurrentHUD = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentHUD)
		{
			CurrentHUD->AddToViewport(0);
		}
	}
	else if (CurrentHUD)
	{
		CurrentHUD->SetVisibility(ESlateVisibility::Visible);
	}
	
	HideCurrentMenu();
	
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
		
		if (APawn* PlayerPawn = PC->GetPawn())
		{
			PlayerPawn->EnableInput(PC);
		}
	}
}

void AThiefGameMode::StartGame()
{
	if (CurrentState != EThiefGameState::MainMenu) return;
	
	SetGameState(EThiefGameState::Playing);
	SetupGameplay();
}

void AThiefGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsLerpingPitch)
	{
		UpdateAmbientPitch(DeltaTime);
	}
}

void AThiefGameMode::SetGameState(EThiefGameState NewState)
{
	if (CurrentState == NewState) return;
	CurrentState = NewState;
	OnGameStateChanged.Broadcast(NewState);
}

void AThiefGameMode::TogglePause()
{
	if (CurrentState == EThiefGameState::MainMenu) return;
	
	if (CurrentState == EThiefGameState::Playing)
	{
		SetGameState(EThiefGameState::Paused);
		UGameplayStatics::SetGamePaused(this, true);
		ShowWidget(PauseMenuWidgetClass);

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
	else if (CurrentState == EThiefGameState::Paused)
	{
		ResumeGame();
	}
}

void AThiefGameMode::ResumeGame()
{
	if (CurrentState != EThiefGameState::Paused) return;

	HideCurrentMenu();
	SetGameState(EThiefGameState::Playing);
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void AThiefGameMode::OnPlayerCaptured()
{
	if (CurrentState != EThiefGameState::Playing) return;

	SetGameState(EThiefGameState::Captured);
	StartPitchLerp(CapturedPitchTarget);
	
	if (GameEndWidgetClass)
	{
		UGameplayStatics::SetGamePaused(this, true);
		
		CurrentMenuWidget = CreateWidget<UUserWidget>(GetWorld(), GameEndWidgetClass);
		if (CurrentMenuWidget)
		{
			if (UGameEndWidget* EndWidget = Cast<UGameEndWidget>(CurrentMenuWidget))
			{
				EndWidget->SetAsLoseScreen();
			}
			CurrentMenuWidget->AddToViewport(10);
		}

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeUIOnly());
		}
	}
}

void AThiefGameMode::OnPlayerWon()
{
	if (CurrentState != EThiefGameState::Playing) return;

	SetGameState(EThiefGameState::Won);
	UGameplayStatics::SetGamePaused(this, true);
	
	if (GameEndWidgetClass)
	{
		CurrentMenuWidget = CreateWidget<UUserWidget>(GetWorld(), GameEndWidgetClass);
		if (CurrentMenuWidget)
		{
			if (UGameEndWidget* EndWidget = Cast<UGameEndWidget>(CurrentMenuWidget))
			{
				EndWidget->SetAsWinScreen();
			}
			CurrentMenuWidget->AddToViewport(10);
		}
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
}

void AThiefGameMode::GoToMainMenu()
{
	UGameplayStatics::SetGamePaused(this, false);
	StartPitchLerp(1.0f);
	
	// Ir al nivel del Main Menu
	if (UThiefGameInstance* GI = Cast<UThiefGameInstance>(GetGameInstance()))
	{
		GI->bSkipMainMenuOnRestart = false;
		GI->CollectedCoins = 0; // Resetear monedas
		UGameplayStatics::OpenLevel(this, GI->MainMenuLevelName);
	}
	else
	{
		// Fallback si no hay GameInstance
		UGameplayStatics::OpenLevel(this, FName("MainMenu"));
	}
}

void AThiefGameMode::RestartGame()
{
	UGameplayStatics::SetGamePaused(this, false);
	
	if (UThiefGameInstance* GI = Cast<UThiefGameInstance>(GetGameInstance()))
	{
		GI->bSkipMainMenuOnRestart = true;
	}
	
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void AThiefGameMode::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void AThiefGameMode::RegisterCoin()
{
	TotalCoinsInLevel++;
}

void AThiefGameMode::UnregisterCoin()
{
	TotalCoinsInLevel = FMath::Max(0, TotalCoinsInLevel - 1);
}

void AThiefGameMode::ShowWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	HideCurrentMenu();

	if (WidgetClass)
	{
		CurrentMenuWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass);
		if (CurrentMenuWidget)
		{
			CurrentMenuWidget->AddToViewport(10);
		}
	}
}

void AThiefGameMode::HideCurrentMenu()
{
	if (CurrentMenuWidget)
	{
		CurrentMenuWidget->RemoveFromParent();
		CurrentMenuWidget = nullptr;
	}
}

void AThiefGameMode::StartPitchLerp(float Target)
{
	TargetAmbientPitch = Target;
	bIsLerpingPitch = true;
}

void AThiefGameMode::UpdateAmbientPitch(float DeltaTime)
{
	if (FMath::IsNearlyEqual(CurrentAmbientPitch, TargetAmbientPitch, 0.01f))
	{
		CurrentAmbientPitch = TargetAmbientPitch;
		bIsLerpingPitch = false;
	}
	else
	{
		CurrentAmbientPitch = FMath::FInterpTo(CurrentAmbientPitch, TargetAmbientPitch, DeltaTime, PitchLerpSpeed);
	}

	for (UAudioComponent* AudioComp : AmbientAudioComponents)
	{
		if (AudioComp && AudioComp->IsPlaying())
		{
			AudioComp->SetPitchMultiplier(CurrentAmbientPitch);
		}
	}
}

void AThiefGameMode::StopAllAmbientSounds()
{
	for (UAudioComponent* AudioComp : AmbientAudioComponents)
	{
		if (AudioComp)
		{
			AudioComp->Stop();
			AudioComp->DestroyComponent();
		}
	}
	AmbientAudioComponents.Empty();
}
