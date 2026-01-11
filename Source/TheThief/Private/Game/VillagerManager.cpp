// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/VillagerManager.h"
#include "AI/VillagerCharacter.h"
#include "Kismet/GameplayStatics.h"

AVillagerManager::AVillagerManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVillagerManager::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoFindVillagers)
	{
		FindAllVillagers();
	}

	DistributeCoins();
}

void AVillagerManager::FindAllVillagers()
{
	Villagers.Empty();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVillagerCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Actor))
		{
			Villagers.Add(Villager);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("VillagerManager: Encontrados %d aldeanos en el nivel"), Villagers.Num());
}

void AVillagerManager::DistributeCoins()
{
	// Primero quitar todas las monedas
	for (AVillagerCharacter* Villager : Villagers)
	{
		if (Villager)
		{
			Villager->SetHasCoin(false);
		}
	}

	// Verificar que hay suficientes villagers
	int32 NumToAssign = FMath::Min(NumVillagersWithCoins, Villagers.Num());
	
	if (NumToAssign <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("VillagerManager: No hay aldeanos para asignar monedas"));
		return;
	}

	// Crear array de índices y mezclarlo aleatoriamente
	TArray<int32> Indices;
	for (int32 i = 0; i < Villagers.Num(); i++)
	{
		if (Villagers[i] != nullptr)
		{
			Indices.Add(i);
		}
	}

	// Mezclar aleatoriamente (Fisher-Yates shuffle)
	for (int32 i = Indices.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Indices.Swap(i, j);
	}

	// Asignar monedas a los primeros N
	for (int32 i = 0; i < NumToAssign && i < Indices.Num(); i++)
	{
		int32 Index = Indices[i];
		if (Villagers.IsValidIndex(Index) && Villagers[Index])
		{
			Villagers[Index]->SetHasCoin(true);
			UE_LOG(LogTemp, Log, TEXT("VillagerManager: Moneda asignada a %s"), *Villagers[Index]->GetName());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("VillagerManager: %d monedas distribuidas entre %d aldeanos"), NumToAssign, Villagers.Num());
}

