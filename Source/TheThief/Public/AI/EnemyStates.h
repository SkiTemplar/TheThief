// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Estado unificado para todos los enemigos (guardias)
UENUM(BlueprintType)
enum class EGuardState : uint8
{
	Patrol = 0     UMETA(DisplayName = "Patrol"),
	Investigate = 1 UMETA(DisplayName = "Investigate"),
	Chase = 2      UMETA(DisplayName = "Chase"),
	Attack = 3     UMETA(DisplayName = "Attack")
};

// Iconos para el widget de NPC
UENUM(BlueprintType)
enum class ENPCWidgetIcon : uint8
{
	None                    UMETA(DisplayName = "None"),
	Coin                    UMETA(DisplayName = "Coin"),
	TalkBubble               UMETA(DisplayName = "Talk Bubble"),
	TalkBubbleWithCoin      UMETA(DisplayName = "Talk Bubble with Coin"),
	Alert                   UMETA(DisplayName = "Alert"),
	Investigating           UMETA(DisplayName = "Investigating"),
	AlertWithCoin           UMETA(DisplayName = "Alert with Coin")
};
