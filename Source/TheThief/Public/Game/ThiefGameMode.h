// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "ThiefGameMode.generated.h"

class UAudioComponent;
class ACameraActor;

UENUM(BlueprintType)
enum class EThiefGameState : uint8
{
	MainMenu UMETA(DisplayName = "MainMenu"),
	Playing UMETA(DisplayName = "Playing"),
	Paused UMETA(DisplayName = "Paused"),
	Won UMETA(DisplayName = "Won"),
	Captured UMETA(DisplayName = "Captured")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EThiefGameState, NewState);

UCLASS()
class THETHIEF_API AThiefGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AThiefGameMode();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGameStateChanged OnGameStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void TogglePause();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPlayerCaptured();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnPlayerWon();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void GoToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void QuitGame();

	UFUNCTION(BlueprintPure, Category = "Game")
	EThiefGameState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Game")
	bool IsInMainMenu() const { return CurrentState == EThiefGameState::MainMenu; }

	UFUNCTION(BlueprintCallable, Category = "Coins")
	void RegisterCoin();

	UFUNCTION(BlueprintCallable, Category = "Coins")
	void UnregisterCoin();

	UFUNCTION(BlueprintPure, Category = "Coins")
	int32 GetTotalCoins() const { return TotalCoinsInLevel; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	TArray<USoundBase*> AmbientSoundsToPlay;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SetGameState(EThiefGameState NewState);
	void ShowWidget(TSubclassOf<UUserWidget> WidgetClass);
	void HideCurrentMenu();

	// === CONFIGURACIÓN DEL MAIN MENU ===
	// Si true, el nivel empieza mostrando el Main Menu
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	bool bStartWithMainMenu = true;

	// Cámara que se usa durante el Main Menu (asignar en el nivel con un Tag)
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FName MenuCameraTag = "MenuCamera";

	// === WIDGETS ===
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameEndWidgetClass;

	// === AUDIO ===
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float CapturedPitchTarget = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float PitchLerpSpeed = 1.0f;

	UPROPERTY()
	UUserWidget* CurrentHUD;

	UPROPERTY()
	UUserWidget* CurrentMenuWidget;

private:
	EThiefGameState CurrentState;
	int32 TotalCoinsInLevel;
	bool bSkipMainMenu = false;

	UPROPERTY()
	TArray<UAudioComponent*> AmbientAudioComponents;

	UPROPERTY()
	ACameraActor* MenuCamera;

	float CurrentAmbientPitch = 1.0f;
	float TargetAmbientPitch = 1.0f;
	bool bIsLerpingPitch = false;

	void StartPitchLerp(float Target);
	void UpdateAmbientPitch(float DeltaTime);
	void SetupMainMenu();
	void SetupGameplay();
	void SwitchToMenuCamera();
	void SwitchToPlayerCamera();
	ACameraActor* FindMenuCamera();
	void StopAllAmbientSounds();
};
