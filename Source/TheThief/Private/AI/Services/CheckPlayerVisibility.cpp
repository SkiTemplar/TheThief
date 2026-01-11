// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Services/CheckPlayerVisibility.h"
#include "AI/EnemyController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UCheckPlayerVisibility::UCheckPlayerVisibility()
{
	NodeName = "Check Player Visibility";
	bNotifyTick = true;
	Interval = 0.5f;
}

void UCheckPlayerVisibility::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	// Obtener el controlador para verificar si realmente puede ver al jugador
	AEnemyController* EnemyController = Cast<AEnemyController>(OwnerComp.GetAIOwner());
	if (!EnemyController) return;

	// Usar el sistema de percepción del controlador para verificar visibilidad
	bool bCanSee = EnemyController->CanSeeTarget();
	
	// Actualizar el blackboard con las claves correctas
	BB->SetValueAsBool(CanSeePlayerKeyName, bCanSee);
}
