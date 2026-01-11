// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/ThiefHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ShooterCharacter.h"

void UThiefHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Configurar colores de la barra
	if (CoinsProgressBar)
	{
		CoinsProgressBar->SetFillColorAndOpacity(ProgressFillColor);
	}

	// Vincular al evento del jugador
	if (AThiefCharacter* Player = Cast<AThiefCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->OnCoinsChanged.AddDynamic(this, &UThiefHUDWidget::UpdateCoins);
		
		// Inicializar con valores actuales
		UpdateCoins(Player->GetCollectedCoins(), Player->GetRequiredCoins());
	}
	else
	{
		// Si no hay jugador todavía, inicializar vacío
		UpdateCoins(0, 10);
	}

	// Inicializar como oculto
	UpdateVisibilityIcon(true);
}

void UThiefHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Actualizar estado de visibilidad desde el jugador
	if (AThiefCharacter* Player = Cast<AThiefCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		bool bIsHidden = !Player->IsBeingSeen();
		if (bIsHidden != bCurrentlyHidden)
		{
			UpdateVisibilityIcon(bIsHidden);
		}
	}
}

void UThiefHUDWidget::UpdateCoins(int32 CurrentCoins, int32 RequiredCoins)
{
	// Actualizar barra de progreso
	if (CoinsProgressBar)
	{
		float Progress = (RequiredCoins > 0) ? (float)CurrentCoins / (float)RequiredCoins : 0.0f;
		CoinsProgressBar->SetPercent(FMath::Clamp(Progress, 0.0f, 1.0f));
	}

	// Actualizar texto opcional
	if (CoinsText)
	{
		FString CoinString = FString::Printf(TEXT("%d/%d"), CurrentCoins, RequiredCoins);
		CoinsText->SetText(FText::FromString(CoinString));
	}
}

void UThiefHUDWidget::UpdateVisibilityIcon(bool bIsHidden)
{
	bCurrentlyHidden = bIsHidden;

	// Mostrar un icono u otro según el estado
	if (HiddenIcon)
	{
		HiddenIcon->SetVisibility(bIsHidden ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (SeenIcon)
	{
		SeenIcon->SetVisibility(bIsHidden ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}
