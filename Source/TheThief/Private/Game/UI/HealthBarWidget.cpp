// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HealthBarWidget.h"
#include "Components/ProgressBar.h"

void UHealthBarWidget::UpdateHealth(float HealthPercent)
{
	if (HealthBar)
	{
		// Asegurarse de que el valor esté entre 0 y 1
		HealthPercent = FMath::Clamp(HealthPercent, 0.0f, 1.0f);
		
		// Actualizar el porcentaje de la barra
		HealthBar->SetPercent(HealthPercent);
		
		// Cambiar el color según el porcentaje de salud
		FLinearColor CurrentColor = FMath::Lerp(LowHealthColor, HighHealthColor, HealthPercent / LowHealthThreshold);
		HealthBar->SetFillColorAndOpacity(CurrentColor);
	}
}

