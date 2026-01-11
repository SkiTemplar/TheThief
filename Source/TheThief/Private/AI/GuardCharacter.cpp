// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/GuardCharacter.h"
#include "AI/EnemyController.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ShooterCharacter.h"
#include "UI/NPCStatusWidget.h"

AGuardCharacter::AGuardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Weapon Mesh
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), WeaponSocketName);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StatusWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StatusWidget"));
	StatusWidget->SetupAttachment(GetMesh());
	StatusWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	StatusWidget->SetWidgetSpace(EWidgetSpace::Screen);
	StatusWidget->SetDrawSize(FVector2D(48.f, 48.f));
	StatusWidget->SetVisibility(false);
}

void AGuardCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	}
	
	// Inicializar el widget con este actor como owner
	if (StatusWidget)
	{
		if (UNPCStatusWidget* Widget = Cast<UNPCStatusWidget>(StatusWidget->GetWidget()))
		{
			Widget->InitializeWidget(this);
		}
	}
	
	PreviousState = CurrentState;
	UpdateWidgetIcon();
}

void AGuardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return;

	// Si está persiguiendo y el jugador está en rango, capturar
	if (CurrentState == EGuardState::Chase && IsPlayerInCaptureRange())
	{
		CapturePlayer();
	}
}

void AGuardCharacter::SetGuardState(EGuardState NewState)
{
	if (CurrentState == NewState) return;
	
	PreviousState = CurrentState;
	CurrentState = NewState;

	// Actualizar velocidad
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = (NewState == EGuardState::Chase) ? ChaseSpeed : PatrolSpeed;
	}

	// Manejar sonidos según el cambio de estado
	if (NewState == EGuardState::Chase && PreviousState != EGuardState::Chase)
	{
		// Acaba de entrar en persecución
		PlayChaseStartSound();
		StartChaseVoiceTimer();
	}
	else if (NewState != EGuardState::Chase && PreviousState == EGuardState::Chase)
	{
		// Dejó de perseguir
		StopChaseVoiceTimer();
	}

	UpdateWidgetIcon();
}

void AGuardCharacter::UpdateWidgetIcon()
{
	if (bIsDead)
	{
		CurrentWidgetIcon = ENPCWidgetIcon::None;
	}
	else
	{
		switch (CurrentState)
		{
		case EGuardState::Chase:
		case EGuardState::Attack:
			CurrentWidgetIcon = ENPCWidgetIcon::Alert;
			break;
		case EGuardState::Investigate:
			CurrentWidgetIcon = ENPCWidgetIcon::Investigating;
			break;
		default:
			CurrentWidgetIcon = ENPCWidgetIcon::None;
			break;
		}
	}

	if (StatusWidget)
	{
		StatusWidget->SetVisibility(CurrentWidgetIcon != ENPCWidgetIcon::None);
	}
}

bool AGuardCharacter::IsPlayerInCaptureRange() const
{
	if (ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		return FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= CaptureRange;
	}
	return false;
}

void AGuardCharacter::CapturePlayer()
{
	if (bIsDead || bHasCapturedPlayer) return;
	
	bHasCapturedPlayer = true;  // Evitar capturas múltiples

	StopChaseVoiceTimer();
	PlayCaptureSound();
	PlayAttackMontage();

	if (AThiefCharacter* Player = Cast<AThiefCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		Player->OnCaptured();
	}
}

void AGuardCharacter::PlayAttackMontage()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void AGuardCharacter::OnAlerted(AActor* Target)
{
	if (bIsDead || CurrentState == EGuardState::Chase || !Target) return;

	if (AEnemyController* EC = Cast<AEnemyController>(GetController()))
	{
		// Ir a investigar la ubicación del objetivo, no perseguir directamente
		EC->ReceiveAlert(Target->GetActorLocation());
	}
}

void AGuardCharacter::AlertNearbyGuards(AActor* Target)
{
	if (!Target) return;

	TArray<AActor*> FoundGuards;
	UGameplayStatics::GetAllActorsOfClass(this, AGuardCharacter::StaticClass(), FoundGuards);

	for (AActor* Actor : FoundGuards)
	{
		if (Actor == this) continue;
		AGuardCharacter* OtherGuard = Cast<AGuardCharacter>(Actor);
		if (OtherGuard && !OtherGuard->IsDead() && FVector::Dist(GetActorLocation(), OtherGuard->GetActorLocation()) <= AlertRadius)
		{
			OtherGuard->OnAlerted(Target);
		}
	}
}

float AGuardCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.f;
	
	// Alertar a guardias cercanos para que también vayan a por el atacante
	if (DamageCauser)
	{
		AlertNearbyGuards(DamageCauser);
	}
	
	// Matar de un golpe - aplicar daño igual a la vida máxima
	return Super::TakeDamage(MaxHealth, DamageEvent, EventInstigator, DamageCauser);
}

void AGuardCharacter::OnDeath()
{
	StopChaseVoiceTimer();
	Super::OnDeath();
}

// === FUNCIONES DE SONIDO ===

void AGuardCharacter::PlayChaseStartSound()
{
	if (ChaseStartSound)
	{
		float Pitch = 1.0f + FMath::RandRange(-PitchVariation, PitchVariation);
		UGameplayStatics::PlaySoundAtLocation(this, ChaseStartSound, GetActorLocation(), 1.0f, Pitch, 0.0f, SoundAttenuation);
	}
}

void AGuardCharacter::PlayCaptureSound()
{
	if (CaptureSound)
	{
		float Pitch = 1.0f + FMath::RandRange(-PitchVariation, PitchVariation);
		UGameplayStatics::PlaySoundAtLocation(this, CaptureSound, GetActorLocation(), 1.0f, Pitch, 0.0f, SoundAttenuation);
	}
}

void AGuardCharacter::PlayChaseVoiceLine()
{
	if (ChaseSounds.Num() == 0 || CurrentState != EGuardState::Chase || bIsDead)
	{
		return;
	}

	// Elegir sonido aleatorio
	int32 RandomIndex = FMath::RandRange(0, ChaseSounds.Num() - 1);
	USoundBase* Sound = ChaseSounds[RandomIndex];
	
	if (Sound)
	{
		float Pitch = 1.0f + FMath::RandRange(-PitchVariation, PitchVariation);
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation(), 1.0f, Pitch, 0.0f, SoundAttenuation);
	}

	// Programar próxima frase
	StartChaseVoiceTimer();
}

void AGuardCharacter::StartChaseVoiceTimer()
{
	if (CurrentState != EGuardState::Chase || bIsDead)
	{
		return;
	}

	float NextInterval = FMath::RandRange(MinChaseVoiceInterval, MaxChaseVoiceInterval);
	GetWorldTimerManager().SetTimer(ChaseVoiceTimerHandle, this, &AGuardCharacter::PlayChaseVoiceLine, NextInterval, false);
}

void AGuardCharacter::StopChaseVoiceTimer()
{
	GetWorldTimerManager().ClearTimer(ChaseVoiceTimerHandle);
}
