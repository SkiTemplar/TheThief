// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AI/EnemyStates.h"
#include "NPCStatusWidget.generated.h"

class UImage;
class UTexture2D;

/**
 * Widget 3D simplificado - Solo muestra iconos (sin texto)
 */
UCLASS()
class THETHIEF_API UNPCStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Inicializar el widget con el Actor dueño
	UFUNCTION(BlueprintCallable, Category = "NPC Widget")
	void InitializeWidget(AActor* NewOwner);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ========== COMPONENTE UI (Solo una imagen) ==========
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	UImage* IMG_Icon = nullptr;

	// ========== TEXTURAS (Asignar en Blueprint) ==========
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* T_Coin = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* T_TalkBubble = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* T_TalkBubbleGold = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* T_Alert = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* T_Question = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
	UTexture2D* T_AlertWithCoin = nullptr;

	// ========== DISTANCIA DE VISIBILIDAD ==========
	// Distancia máxima a la que el widget es visible (0 = siempre visible)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	float MaxVisibilityDistance = 1500.f;

	// Distancia a la que empieza a hacer fade out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
	float FadeStartDistance = 1000.f;

private:
	UPROPERTY()
	AActor* OwnerNPC = nullptr;

	ENPCWidgetIcon CurrentIcon = ENPCWidgetIcon::None;

	void UpdateDisplay(ENPCWidgetIcon NewIcon);
	void UpdateVisibilityByDistance();
};
