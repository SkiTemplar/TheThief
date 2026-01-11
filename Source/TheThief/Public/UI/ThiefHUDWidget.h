// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ThiefHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

/**
 * Widget principal del HUD del jugador
 * Muestra una barra de progreso de monedas e icono de visibilidad
 */
UCLASS()
class THETHIEF_API UThiefHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateCoins(int32 CurrentCoins, int32 RequiredCoins);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateVisibilityIcon(bool bIsHidden);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// === BARRA DE PROGRESO ===
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* CoinsProgressBar;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* CoinsText;

	// === ICONOS DE VISIBILIDAD ===
	// Imagen que muestra cuando el jugador está OCULTO (ojo cerrado)
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* HiddenIcon;

	// Imagen que muestra cuando el jugador es VISTO (ojo abierto)
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* SeenIcon;

	// === COLORES DE LA BARRA ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD|Coins")
	FLinearColor ProgressFillColor = FLinearColor(1.0f, 0.84f, 0.0f, 1.0f);

private:
	bool bCurrentlyHidden = true;
};
