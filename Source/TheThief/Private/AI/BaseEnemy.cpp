// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BaseEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseEnemy::OnDeath()
{
	Super::OnDeath();
	
	if (AAIController* AICtrl = Cast<AAIController>(GetController()))
	{
		if (AICtrl->GetBrainComponent())
		{
			AICtrl->GetBrainComponent()->StopLogic(TEXT("Dead"));
		}
	}
	
	SetLifeSpan(DestroyDelay);
}
