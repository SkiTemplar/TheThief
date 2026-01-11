// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/VillagerCharacter.h"
#include "AI/GuardCharacter.h"
#include "AI/VillagerController.h"
#include "AI/EnemyController.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/NPCStatusWidget.h"
#include "Player/ShooterCharacter.h"

// Inicializar variables estáticas para control de alertas
AActor* AVillagerCharacter::CurrentAlertedThief = nullptr;
AVillagerCharacter* AVillagerCharacter::CurrentAlerter = nullptr;

AVillagerCharacter::AVillagerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusWidget"));
	StatusWidget->SetupAttachment(GetMesh());
	StatusWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	StatusWidget->SetWidgetSpace(EWidgetSpace::Screen); // Screen space para tamaño fijo en pantalla
	StatusWidget->SetDrawSize(FVector2D(50.f, 50.f));
	StatusWidget->SetDrawAtDesiredSize(true); // Usar el tamaño deseado del widget
	StatusWidget->SetVisibility(false);
	StatusWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StatusWidget->SetGenerateOverlapEvents(false);
}

void AVillagerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
	
	// Inicializar el widget con este actor como owner
	if (StatusWidget)
	{
		if (UNPCStatusWidget* Widget = Cast<UNPCStatusWidget>(StatusWidget->GetWidget()))
		{
			Widget->InitializeWidget(this);
		}
	}
	
	UpdateWidgetIcon();
}

void AVillagerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDead) return;

	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();

	// Si está hablando y se mueve, parar
	if (bIsTalking && Speed > 10.f)
	{
		StopTalking();
		return;
	}

	// Actualizar idle
	if (Speed <= 10.f)
	{
		IdleTimer += DeltaTime;
		if (IdleTimer >= IdleTimeToTalk)
		{
			bIsIdle = true;
			if (!bIsTalking && CurrentVillagerState != EVillagerState::Fleeing)
			{
				CheckForNearbyVillagers();
			}
		}
	}
	else
	{
		IdleTimer = 0.f;
		bIsIdle = false;
	}
}

void AVillagerCharacter::CheckForNearbyVillagers()
{
	if (!bIsIdle || bIsTalking || bIsDead || CurrentVillagerState == EVillagerState::Fleeing) return;

	TArray<AActor*> Villagers;
	UGameplayStatics::GetAllActorsOfClass(this, AVillagerCharacter::StaticClass(), Villagers);

	for (AActor* Actor : Villagers)
	{
		AVillagerCharacter* Other = Cast<AVillagerCharacter>(Actor);
		if (!Other || Other == this || Other->IsDead()) continue;
		if (FVector::Dist(GetActorLocation(), Other->GetActorLocation()) > TalkDistance) continue;
		if (Other->GetVillagerState() == EVillagerState::Fleeing || !Other->IsIdle() || Other->IsTalking()) continue;

		StartTalking(Other);
		Other->StartTalking(this);
		break;
	}
}

void AVillagerCharacter::StartTalking(AVillagerCharacter* Partner)
{
	if (!Partner || bIsTalking) return;

	bIsTalking = true;
	TalkPartner = Partner;
	SetVillagerState(EVillagerState::Talking);

	// Mirar al compañero
	FVector Dir = Partner->GetActorLocation() - GetActorLocation();
	Dir.Z = 0.f;
	if (!Dir.IsNearlyZero()) SetActorRotation(Dir.Rotation());

	if (TalkAnimations.Num() > 0)
	{
		if (USkeletalMeshComponent* M = GetMesh())
		{
			M->PlayAnimation(TalkAnimations[FMath::RandRange(0, TalkAnimations.Num() - 1)], true);
		}
	}
	PlayTalkSound();
	GetWorldTimerManager().SetTimer(TalkSoundTimerHandle, this, &AVillagerCharacter::PlayTalkSound, FMath::RandRange(2.f, 4.f), true);
}

void AVillagerCharacter::StopTalking()
{
	if (!bIsTalking) return;
	bIsTalking = false;

	GetWorldTimerManager().ClearTimer(TalkSoundTimerHandle);
	if (CurrentTalkAudioComponent && CurrentTalkAudioComponent->IsPlaying())
	{
		CurrentTalkAudioComponent->Stop();
		CurrentTalkAudioComponent = nullptr;
	}

	if (TalkPartner && TalkPartner->IsTalking())
	{
		AVillagerCharacter* Old = TalkPartner;
		TalkPartner = nullptr;
		Old->StopTalking();
	}
	else
	{
		TalkPartner = nullptr;
	}

	if (GetMesh()) GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SetVillagerState(EVillagerState::Walking);
}

void AVillagerCharacter::SetVillagerState(EVillagerState NewState)
{
	if (CurrentVillagerState == NewState) return;
	CurrentVillagerState = NewState;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = (NewState == EVillagerState::Fleeing || NewState == EVillagerState::AlertingGuard) ? FleeSpeed : WalkSpeed;
	}

	UpdateWidgetIcon();
}

float AVillagerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsTalking) StopTalking();
	bIsIdle = false;
	IdleTimer = 0.f;

	bool bWasAlive = !bIsDead;
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bWasAlive && bIsDead)
	{
		PlayDeathSound();
		
		// Si el aldeano tenía moneda, dársela al atacante (jugador)
		if (bHasCoin && DamageCauser)
		{
			if (AThiefCharacter* Player = Cast<AThiefCharacter>(DamageCauser))
			{
				Player->AddCoin(1);
				SetHasCoin(false);
				UE_LOG(LogTemp, Log, TEXT("Villager %s: Player killed me and took my coin!"), *GetName());
			}
		}
		
		if (DamageCauser) AlertNearbyVillagers(DamageCauser);
	}
	else if (Damage > 0.f && DamageCauser && !bIsDead)
	{
		AlertGuards(DamageCauser);
		AlertNearbyVillagers(DamageCauser);
		StartFleeing();
	}
	return Damage;
}

void AVillagerCharacter::AlertNearbyVillagers(AActor* Attacker)
{
	if (!Attacker) return;

	TArray<AActor*> Villagers;
	UGameplayStatics::GetAllActorsOfClass(this, AVillagerCharacter::StaticClass(), Villagers);

	for (AActor* Actor : Villagers)
	{
		AVillagerCharacter* Other = Cast<AVillagerCharacter>(Actor);
		if (!Other || Other == this || Other->IsDead()) continue;
		if (FVector::Dist(GetActorLocation(), Other->GetActorLocation()) > AlertRadius) continue;

		// Verificar línea de visión
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Other);
		Params.AddIgnoredActor(Attacker);

		if (!GetWorld()->LineTraceSingleByChannel(Hit, Other->GetActorLocation() + FVector(0,0,50), GetActorLocation() + FVector(0,0,50), ECC_Visibility, Params))
		{
			Other->OnNearbyVillagerAttacked(Attacker);
		}
	}
}

void AVillagerCharacter::OnNearbyVillagerAttacked(AActor* Attacker)
{
	if (bIsDead || CurrentVillagerState == EVillagerState::Fleeing) return;
	if (bIsTalking) StopTalking();
	bIsIdle = false;
	IdleTimer = 0.f;

	PlayPanicSound();
	AlertGuards(Attacker);
	StartFleeing();

	if (AVillagerController* VC = Cast<AVillagerController>(GetController()))
	{
		VC->FleeFrom(Attacker);
	}
}

void AVillagerCharacter::AlertGuards(AActor* SuspiciousActor)
{
	if (bHasAlertedGuards || bIsDead || !SuspiciousActor) return;
	bHasAlertedGuards = true;
	PlayPanicSound();

	// Guardar la ubicación del sospechoso para cuando llegue al guardia
	LastKnownThiefLocation = SuspiciousActor->GetActorLocation();
	
	UE_LOG(LogTemp, Display, TEXT("Villager %s: AlertGuards called, ThiefLocation: %s"), *GetName(), *LastKnownThiefLocation.ToString());

	TArray<AActor*> Guards;
	UGameplayStatics::GetAllActorsOfClass(this, AGuardCharacter::StaticClass(), Guards);

	bool bAlertedAnyGuard = false;
	
	for (AActor* Actor : Guards)
	{
		AGuardCharacter* Guard = Cast<AGuardCharacter>(Actor);
		if (!Guard || Guard->IsDead()) continue;
		
		float DistToGuard = FVector::Dist(GetActorLocation(), Guard->GetActorLocation());
		if (DistToGuard > AlertRadius) continue;

		// Verificar línea de visión hacia el guardia (no hacia el sospechoso)
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Guard);
		
		FVector MyEyes = GetActorLocation() + FVector(0, 0, 60);
		FVector GuardPos = Guard->GetActorLocation() + FVector(0, 0, 60);
		
		// Si puedo ver al guardia, alertarle directamente
		if (!GetWorld()->LineTraceSingleByChannel(Hit, MyEyes, GuardPos, ECC_Visibility, Params))
		{
			if (AEnemyController* EC = Cast<AEnemyController>(Guard->GetController()))
			{
				// Alertar con la ubicación del ladrón, no del villager
				EC->ReceiveAlert(LastKnownThiefLocation);
				bAlertedAnyGuard = true;
				UE_LOG(LogTemp, Display, TEXT("Villager %s: Alerted Guard %s directly"), *GetName(), *Guard->GetName());
			}
		}
	}
	
	// Si no pudo alertar a ningún guardia directamente, ir a buscar uno
	if (!bAlertedAnyGuard)
	{
		FleeToNearestGuard();
	}
}

void AVillagerCharacter::StartFleeing()
{
	if (bIsDead) return;
	if (bIsTalking) StopTalking();
	bIsIdle = false;
	IdleTimer = 0.f;

	SetVillagerState(EVillagerState::Fleeing);
	GetWorldTimerManager().ClearTimer(CalmDownTimerHandle);
	GetWorldTimerManager().SetTimer(CalmDownTimerHandle, this, &AVillagerCharacter::CalmDown, FleeDuration, false);
}

void AVillagerCharacter::CalmDown()
{
	if (bIsDead) return;
	bHasAlertedGuards = false;
	
	// Si este aldeano era el que estaba alertando, limpiar las variables estáticas
	if (CurrentAlerter == this)
	{
		CurrentAlertedThief = nullptr;
		CurrentAlerter = nullptr;
	}
	
	SetVillagerState(EVillagerState::Walking);

	if (AVillagerController* VC = Cast<AVillagerController>(GetController()))
	{
		VC->StopFleeing();
	}
}

void AVillagerCharacter::PlayPanicSound()
{
	if (PanicSounds.Num() == 0) return;
	USoundBase* Sound = PanicSounds[FMath::RandRange(0, PanicSounds.Num() - 1)];
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation(), 1.f, 1.f + FMath::RandRange(-PitchVariation, PitchVariation), 0.f, SoundAttenuation);
	}
}

void AVillagerCharacter::PlayDeathSound()
{
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), 1.f, 1.f + FMath::RandRange(-PitchVariation, PitchVariation), 0.f, SoundAttenuation);
	}
}

void AVillagerCharacter::PlayTalkSound()
{
	if (TalkSounds.Num() == 0 || !bIsTalking) return;

	if (CurrentTalkAudioComponent && CurrentTalkAudioComponent->IsPlaying())
	{
		CurrentTalkAudioComponent->Stop();
	}

	USoundBase* Sound = TalkSounds[FMath::RandRange(0, TalkSounds.Num() - 1)];
	if (Sound)
	{
		CurrentTalkAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, Sound, GetActorLocation(), FRotator::ZeroRotator, TalkVolumeMultiplier, 1.f + FMath::RandRange(-PitchVariation, PitchVariation), 0.f, SoundAttenuation, nullptr, true);
	}
}

void AVillagerCharacter::SetHasCoin(bool bNewHasCoin)
{
	bHasCoin = bNewHasCoin;
	UpdateWidgetIcon();
}

void AVillagerCharacter::UpdateWidgetIcon()
{
	if (bIsDead)
	{
		CurrentWidgetIcon = ENPCWidgetIcon::None;
	}
	else if (CurrentVillagerState == EVillagerState::Fleeing || CurrentVillagerState == EVillagerState::AlertingGuard)
	{
		// Si está huyendo/alertando Y tiene moneda, mostrar AlertWithCoin
		CurrentWidgetIcon = bHasCoin ? ENPCWidgetIcon::AlertWithCoin : ENPCWidgetIcon::Alert;
	}
	else if (bIsTalking)
	{
		CurrentWidgetIcon = bHasCoin ? ENPCWidgetIcon::TalkBubbleWithCoin : ENPCWidgetIcon::TalkBubble;
	}
	else if (bHasCoin)
	{
		CurrentWidgetIcon = ENPCWidgetIcon::Coin;
	}
	else
	{
		CurrentWidgetIcon = ENPCWidgetIcon::None;
	}

	if (StatusWidget)
	{
		StatusWidget->SetVisibility(CurrentWidgetIcon != ENPCWidgetIcon::None);
	}
}

void AVillagerCharacter::OnRobbed(AActor* Thief)
{
	if (!bHasCoin || bIsDead) return;

	SetHasCoin(false);
	NotifyNearbyWitnesses(Thief);
}

void AVillagerCharacter::NotifyNearbyWitnesses(AActor* Thief)
{
	if (!Thief) return;

	TArray<AActor*> Villagers;
	UGameplayStatics::GetAllActorsOfClass(this, AVillagerCharacter::StaticClass(), Villagers);

	for (AActor* Actor : Villagers)
	{
		AVillagerCharacter* Other = Cast<AVillagerCharacter>(Actor);
		if (!Other || Other == this || Other->IsDead()) continue;
		if (FVector::Dist(GetActorLocation(), Other->GetActorLocation()) > WitnessRadius) continue;

		if (Other->CanSeeActor(Thief))
		{
			Other->OnWitnessedTheft(Thief);
		}
	}
}

void AVillagerCharacter::OnWitnessedTheft(AActor* Thief)
{
	if (bIsDead || CurrentVillagerState == EVillagerState::Fleeing || CurrentVillagerState == EVillagerState::AlertingGuard) return;
	if (bIsTalking) StopTalking();

	WitnessedThief = Thief;
	LastKnownThiefLocation = Thief->GetActorLocation();

	PlayPanicSound();
	
	// Solo UN aldeano va a alertar al guardia por cada ladrón
	// Si ya hay alguien alertando sobre este ladrón, solo huir
	if (CurrentAlertedThief == Thief && CurrentAlerter != nullptr && !CurrentAlerter->IsDead())
	{
		// Ya hay otro aldeano alertando, solo huir
		UE_LOG(LogTemp, Display, TEXT("Villager %s: Another villager is already alerting, just fleeing"), *GetName());
		StartFleeing();
		return;
	}
	
	// Este aldeano será el que alerte
	CurrentAlertedThief = Thief;
	CurrentAlerter = this;
	
	UE_LOG(LogTemp, Display, TEXT("Villager %s: I will alert the guard about the thief"), *GetName());
	FleeToNearestGuard();
}

void AVillagerCharacter::FleeToNearestGuard()
{
	AGuardCharacter* NearestGuard = FindNearestGuard();
	if (!NearestGuard)
	{
		StartFleeing();
		return;
	}

	SetVillagerState(EVillagerState::AlertingGuard);

	if (AVillagerController* VC = Cast<AVillagerController>(GetController()))
	{
		VC->FleeToGuard(NearestGuard, LastKnownThiefLocation);
	}
}

AGuardCharacter* AVillagerCharacter::FindNearestGuard()
{
	TArray<AActor*> Guards;
	UGameplayStatics::GetAllActorsOfClass(this, AGuardCharacter::StaticClass(), Guards);

	AGuardCharacter* Nearest = nullptr;
	float MinDist = MAX_FLT;

	for (AActor* Actor : Guards)
	{
		AGuardCharacter* Guard = Cast<AGuardCharacter>(Actor);
		if (!Guard || Guard->IsDead()) continue;

		float Dist = FVector::Dist(GetActorLocation(), Guard->GetActorLocation());
		if (Dist < MinDist)
		{
			MinDist = Dist;
			Nearest = Guard;
		}
	}

	return Nearest;
}

bool AVillagerCharacter::CanSeeActor(AActor* Target) const
{
	if (!Target) return false;

	FVector Start = GetActorLocation() + FVector(0, 0, 50);
	FVector End = Target->GetActorLocation() + FVector(0, 0, 50);

	// Verificar si está mirando hacia el target
	FVector ToTarget = (End - Start).GetSafeNormal();
	FVector Forward = GetActorForwardVector();
	float Dot = FVector::DotProduct(Forward, ToTarget);

	if (Dot < 0.5f) return false;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Target);

	return !GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}

void AVillagerCharacter::PlayRandomTalkAnimation()
{
	if (TalkAnimations.Num() == 0) return;
	
	if (USkeletalMeshComponent* M = GetMesh())
	{
		int32 Index = FMath::RandRange(0, TalkAnimations.Num() - 1);
		M->PlayAnimation(TalkAnimations[Index], false);
	}
}
