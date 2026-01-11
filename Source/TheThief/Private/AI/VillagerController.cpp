// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/VillagerController.h"

#include "AI/VillagerCharacter.h"
#include "AI/GuardCharacter.h"
#include "AI/EnemyController.h"
#include "Player/ShooterCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"

AVillagerController::AVillagerController()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerceptionComponent);
}

void AVillagerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Configurar Sight con los valores editables
	if (AIPerceptionComponent)
	{
		UAISenseConfig_Sight* SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("SightConfig"));
		SightConfig->SightRadius = SightRadius;
		SightConfig->LoseSightRadius = LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
		
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AVillagerController::OnSensed);
	}
}

void AVillagerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
		SetupBlackboard();
	}
}

void AVillagerController::SetupBlackboard()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	// Inicializar valores del blackboard
	BB->SetValueAsBool("IsFleeing", false);
	BB->SetValueAsBool("IsAlertingGuard", false);
	BB->SetValueAsBool("IsTalking", false);
}

void AVillagerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Sincronizar estado del Character con el Blackboard
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(GetPawn()))
		{
			BB->SetValueAsBool("IsIdle", Villager->IsIdle());
			BB->SetValueAsBool("IsTalking", Villager->IsTalking());
		}
	}
}

void AVillagerController::OnSensed(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor) return;
	
	AThiefCharacter* Player = Cast<AThiefCharacter>(Actor);
	if (!Player) return;
	
	AVillagerCharacter* Villager = Cast<AVillagerCharacter>(GetPawn());
	if (!Villager) return;
	
	if (Stimulus.WasSuccessfullySensed())
	{
		// Marcar al jugador como "visto" por este aldeano
		if (!bCanSeePlayer)
		{
			bCanSeePlayer = true;
			Player->AddSeenBy();
		}
		
		// Los aldeanos SOLO reaccionan si ven un CRIMEN (robo o asesinato)
		// NO reaccionan por correr - eso es solo para guardias
		if (Player->IsStealing() || Player->IsCommittingCrime())
		{
			// Llamar a OnWitnessedTheft que hace el flujo completo:
			// Guarda la ubicación y va al guardia más cercano a avisarle
			Villager->OnWitnessedTheft(Actor);
		}
	}
	else
	{
		// Marcar al jugador como "no visto" por este aldeano
		if (bCanSeePlayer)
		{
			bCanSeePlayer = false;
			Player->RemoveSeenBy();
		}
		
		// Si perdió de vista al jugador y estaba huyendo de él
		if (bIsFleeing)
		{
			// Iniciar timer para calmarse
			GetWorldTimerManager().SetTimer(CalmDownTimerHandle, this, &AVillagerController::StopFleeing, FleeCalmnessTime, false);
		}
	}
}

void AVillagerController::FleeFrom(AActor* DangerActor)
{
	if (!DangerActor || bIsFleeing) return;
	
	bIsFleeing = true;
	GetWorldTimerManager().ClearTimer(CalmDownTimerHandle);
	
	if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(GetPawn()))
	{
		Villager->SetVillagerState(EVillagerState::Fleeing);
	}
	
	// Actualizar el Blackboard para que el BT reaccione
	SetBlackboardThreat(DangerActor, DangerActor->GetActorLocation());
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsBool("IsFleeing", true);
		BB->SetValueAsBool("IsAlertingGuard", false);
	}
}

void AVillagerController::StopFleeing()
{
	bIsFleeing = false;
	bIsAlertingGuard = false;
	
	if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(GetPawn()))
	{
		Villager->SetVillagerState(EVillagerState::Walking);
	}
	
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsBool("IsFleeing", false);
		BB->SetValueAsBool("IsAlertingGuard", false);
		BB->ClearValue("ThreatActor");
		BB->ClearValue("TargetGuard");
	}
}

void AVillagerController::FleeToGuard(AGuardCharacter* Guard, FVector ThiefLastLocation)
{
	if (!Guard || !GetPawn()) return;

	bIsAlertingGuard = true;
	bIsFleeing = false;
	GetWorldTimerManager().ClearTimer(CalmDownTimerHandle);

	UE_LOG(LogTemp, Warning, TEXT("VillagerController::FleeToGuard - ThiefLastLocation: %s"), *ThiefLastLocation.ToString());

	if (AVillagerCharacter* Villager = Cast<AVillagerCharacter>(GetPawn()))
	{
		Villager->SetVillagerState(EVillagerState::AlertingGuard);
	}

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsBool("IsFleeing", false);
		BB->SetValueAsBool("IsAlertingGuard", true);
		BB->SetValueAsObject("TargetGuard", Guard);
		BB->SetValueAsVector("ThreatLastLocation", ThiefLastLocation);
		
		UE_LOG(LogTemp, Warning, TEXT("VillagerController::FleeToGuard - Saved to BB: TargetGuard=%s, ThreatLastLocation=%s"), 
			*Guard->GetName(), *ThiefLastLocation.ToString());
	}
}

void AVillagerController::SetBlackboardThreat(AActor* ThreatActor, FVector ThreatLocation)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsObject("ThreatActor", ThreatActor);
		BB->SetValueAsVector("ThreatLastLocation", ThreatLocation);
	}
}
