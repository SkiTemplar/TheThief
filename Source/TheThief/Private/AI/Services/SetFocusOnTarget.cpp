// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Services/SetFocusOnTarget.h"

#include "AIController.h"
#include "AI/BaseEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USetFocusOnTarget::USetFocusOnTarget()
{
	NodeName = "Set Focus On Target";
	bNotifyBecomeRelevant = true;
	bNotifyTick = false;
	bNotifyCeaseRelevant = true;
	
	TargetKeyName = "TargetActor";  // Cambiado de "Target" a "TargetActor"
}

void USetFocusOnTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKeyName));
	if (!TargetActor) return;

	// Establecer el foco en el target
	AIController->SetFocus(TargetActor);

	// Activar strafing
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (Enemy)
	{
		UCharacterMovementComponent* MovementComp = Enemy->GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->bOrientRotationToMovement = false;
			MovementComp->bUseControllerDesiredRotation = true;
			
			UE_LOG(LogTemp, Display, TEXT("SetFocusOnTarget: Strafing enabled for %s"), *Enemy->GetName());
		}
	}
}

void USetFocusOnTarget::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	// Limpiar el foco
	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	// Desactivar strafing y volver al movimiento normal
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(AIController->GetPawn());
	if (Enemy)
	{
		UCharacterMovementComponent* MovementComp = Enemy->GetCharacterMovement();
		if (MovementComp)
		{
			MovementComp->bOrientRotationToMovement = true;
			MovementComp->bUseControllerDesiredRotation = false;
			
			UE_LOG(LogTemp, Display, TEXT("SetFocusOnTarget: Strafing disabled for %s"), *Enemy->GetName());
		}
	}
}
