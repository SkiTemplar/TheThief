// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ShooterController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "UI/HealthBarWidget.h"

void AShooterController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Input = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	Input->AddMappingContext(MappingContext, 0);

	// Crear y añadir el widget de salud al viewport
	if (HealthBarClass)
	{
		HealthBarWidget = CreateWidget<UHealthBarWidget>(this, HealthBarClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
		}
	}
}

void AShooterController::UpdateHealthBar(float HealthPercent)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(HealthPercent);
	}
}
