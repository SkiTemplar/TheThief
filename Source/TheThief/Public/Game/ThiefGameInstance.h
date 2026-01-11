// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "ThiefGameInstance.generated.h"

UCLASS()
class THETHIEF_API UThiefGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UThiefGameInstance();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void ResetGameData();

	// ========== GAME DATA ==========
	// Monedas recogidas
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 CollectedCoins = 0;

	// Monedas requeridas para ganar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game", meta = (ClampMin = "1", ClampMax = "100"))
	int32 RequiredCoins = 10;

	// Si true, salta el Main Menu al cargar el nivel (usado al reiniciar)
	UPROPERTY(BlueprintReadWrite, Category = "Game")
	bool bSkipMainMenuOnRestart = false;

	// ========== LEVEL NAMES ==========
	// Nombre del nivel principal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	FName MainMenuLevelName = "MainMenu";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	FName GameLevelName = "Lvl_FirstPerson";
};
