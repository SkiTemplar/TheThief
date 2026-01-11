// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Tasks/BTTask_VillagerAlertGuard.h"
#include "AI/GuardCharacter.h"
#include "AI/EnemyController.h"
#include "AI/VillagerCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_VillagerAlertGuard::UBTTask_VillagerAlertGuard()
{
	NodeName = "Alert Guard About Threat";
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTask_VillagerAlertGuard::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_VillagerAlertGuard: No Controller!"));
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_VillagerAlertGuard: No Blackboard!"));
		return EBTNodeResult::Failed;
	}

	AGuardCharacter* Guard = Cast<AGuardCharacter>(BB->GetValueAsObject(TargetGuardKey.SelectedKeyName));
	if (!Guard)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_VillagerAlertGuard: No Guard found in key '%s'"), *TargetGuardKey.SelectedKeyName.ToString());
		return EBTNodeResult::Failed;
	}

	FVector ThreatLocation = BB->GetValueAsVector(ThreatLastLocationKey.SelectedKeyName);
	
	// Validar que la ubicación es válida
	bool bValidLocation = !ThreatLocation.ContainsNaN() && 
	                       ThreatLocation.X < 1e30f && 
	                       !ThreatLocation.IsNearlyZero();
	
	if (!bValidLocation)
	{
		// Intentar usar la ubicación del villager como fallback
		if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Controller->GetPawn()))
		{
			ThreatLocation = Villager->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("BTTask_VillagerAlertGuard: Invalid ThreatLocation, using Villager location: %s"), *ThreatLocation.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BTTask_VillagerAlertGuard: Invalid ThreatLocation and no Villager fallback"));
			return EBTNodeResult::Failed;
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("BTTask_VillagerAlertGuard: Alerting Guard %s at location %s"), *Guard->GetName(), *ThreatLocation.ToString());

	// Alertar al guardia con la última posición conocida del ladrón
	if (AEnemyController* GuardController = Cast<AEnemyController>(Guard->GetController()))
	{
		GuardController->ReceiveAlert(ThreatLocation);
		UE_LOG(LogTemp, Display, TEXT("BTTask_VillagerAlertGuard: Guard alerted successfully!"));
	}
	else
	{
		// Si no tiene EnemyController, llamar directamente a OnAlerted
		Guard->OnAlerted(Controller->GetPawn());
	}

	// Limpiar el estado de alerta del villager
	if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(Controller->GetPawn()))
	{
		Villager->SetVillagerState(EVillagerState::Walking);
	}

	// Limpiar el blackboard
	BB->ClearValue(TargetGuardKey.SelectedKeyName);
	BB->SetValueAsBool("IsAlertingGuard", false);

	return EBTNodeResult::Succeeded;
}

FString UBTTask_VillagerAlertGuard::GetStaticDescription() const
{
	return FString::Printf(TEXT("Alert %s about threat"), *TargetGuardKey.SelectedKeyName.ToString());
}
