// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/Attack.h"

#include "AIController.h"
#include "AI/GuardCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

UAttack::UAttack()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	MyComponent = &OwnerComp;
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (AGuardCharacter* Guard = Cast<AGuardCharacter>(Pawn))
	{
		Guard->PlayAttackMontage();
		
		if (UAnimInstance* AnimInstance = Guard->GetMesh()->GetAnimInstance())
		{
			AnimInstance->OnMontageEnded.AddDynamic(this, &UAttack::OnAttackEnded);
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UAttack::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (MyComponent)
	{
		// Limpiar el delegate para evitar llamadas múltiples
		if (AAIController* AIController = MyComponent->GetAIOwner())
		{
			if (APawn* Pawn = AIController->GetPawn())
			{
				if (AGuardCharacter* Guard = Cast<AGuardCharacter>(Pawn))
				{
					if (UAnimInstance* AnimInstance = Guard->GetMesh()->GetAnimInstance())
					{
						AnimInstance->OnMontageEnded.RemoveDynamic(this, &UAttack::OnAttackEnded);
					}
				}
			}
		}
		
		FinishLatentTask(*MyComponent, EBTNodeResult::Succeeded);
	}
}
