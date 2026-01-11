// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterController.generated.h"

class UInputMappingContext;
class UHealthBarWidget;

/**
 * 
 */
UCLASS()
class THETHIEF_API AShooterController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHealthBarWidget> HealthBarClass;

	UPROPERTY()
	UHealthBarWidget* HealthBarWidget;

	// Función para actualizar la barra de salud
	void UpdateHealthBar(float HealthPercent);
};
