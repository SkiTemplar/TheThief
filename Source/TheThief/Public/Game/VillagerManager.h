// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VillagerManager.generated.h"

class AVillagerCharacter;

/**
 * Manager que asigna monedas aleatoriamente a los aldeanos del nivel
 */
UCLASS()
class THETHIEF_API AVillagerManager : public AActor
{
	GENERATED_BODY()

public:
	AVillagerManager();

	// Llamar para redistribuir monedas (útil para reiniciar nivel)
	UFUNCTION(BlueprintCallable, Category = "Villager Manager")
	void DistributeCoins();

protected:
	virtual void BeginPlay() override;

	// Número de aldeanos que tendrán monedas
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Villager Manager")
	int32 NumVillagersWithCoins = 5;

	// Si es true, busca automáticamente todos los villagers del nivel
	// Si es false, usa el array Villagers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Villager Manager")
	bool bAutoFindVillagers = true;

	// Array manual de villagers (solo se usa si bAutoFindVillagers es false)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Villager Manager", meta = (EditCondition = "!bAutoFindVillagers"))
	TArray<AVillagerCharacter*> Villagers;

private:
	void FindAllVillagers();
	void AssignCoinsRandomly();
};

