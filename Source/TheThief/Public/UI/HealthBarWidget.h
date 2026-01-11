// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "HealthBarWidget.generated.h"

class UProgressBar;

/**
 * Widget que muestra la barra de salud del jugador
 */
UCLASS()
class THETHIEF_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Función para actualizar el porcentaje de salud
	UFUNCTION(BlueprintCallable, Category = "Health")
	void UpdateHealth(float HealthPercent);

	// Alias para compatibilidad
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealthPercent(float HealthPercent) { UpdateHealth(HealthPercent); }

protected:
	// Referencia a la barra de progreso (debe estar vinculada en el Blueprint)
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* HealthBar;

	// Color cuando la salud es alta
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	FLinearColor HighHealthColor = FLinearColor::Green;

	// Color cuando la salud es baja
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	FLinearColor LowHealthColor = FLinearColor::Red;

	// Umbral para considerar la salud como baja (0.0 - 1.0)
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float LowHealthThreshold = 0.3f;
};
