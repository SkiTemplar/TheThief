// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/ThiefGameInstance.h"

UThiefGameInstance::UThiefGameInstance()
{
	CollectedCoins = 0;
	RequiredCoins = 10;
}

void UThiefGameInstance::ResetGameData()
{
	CollectedCoins = 0;
}
