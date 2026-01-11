// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// Estado para los Villagers en el Blackboard
UENUM(BlueprintType)
enum class EVillagerBTState : uint8
{
	Idle = 0       UMETA(DisplayName = "Idle"),
	Walking = 1    UMETA(DisplayName = "Walking"),
	Talking = 2    UMETA(DisplayName = "Talking"),
	Fleeing = 3    UMETA(DisplayName = "Fleeing"),
	AlertingGuard = 4 UMETA(DisplayName = "Alerting Guard")
};

