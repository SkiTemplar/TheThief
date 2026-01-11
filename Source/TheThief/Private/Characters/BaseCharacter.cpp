// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}

float ABaseCharacter::GetHealthPercent() const
{
	if (MaxHealth <= 0.f) return 0.f;
	return Health / MaxHealth;
}

EHitDirection ABaseCharacter::GetHitDirection(AActor* DamageCauser) const
{
	if (!DamageCauser) return EHitDirection::Front;
	
	// Vector desde este personaje hacia el atacante
	FVector ToAttacker = DamageCauser->GetActorLocation() - GetActorLocation();
	ToAttacker.Z = 0.f;
	ToAttacker.Normalize();
	
	// Vectores de referencia del personaje
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();
	
	// Calcular dot products
	float ForwardDot = FVector::DotProduct(ToAttacker, Forward);
	float RightDot = FVector::DotProduct(ToAttacker, Right);
	
	// Determinar dirección basada en los dot products
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		return ForwardDot > 0 ? EHitDirection::Front : EHitDirection::Back;
	}
	else
	{
		return RightDot > 0 ? EHitDirection::Right : EHitDirection::Left;
	}
}

void ABaseCharacter::PlayHitAnimation(EHitDirection HitDirection)
{
	UAnimMontage* MontageToPlay = nullptr;
	
	switch (HitDirection)
	{
		case EHitDirection::Front:
			MontageToPlay = HitFrontMontage;
			break;
		case EHitDirection::Back:
			MontageToPlay = HitBackMontage;
			break;
		case EHitDirection::Left:
			MontageToPlay = HitLeftMontage;
			break;
		case EHitDirection::Right:
			MontageToPlay = HitRightMontage;
			break;
	}
	
	if (MontageToPlay)
	{
		// Forzar que se reproduzca el montage incluso si está muriendo
		float Duration = PlayAnimMontage(MontageToPlay);
		UE_LOG(LogTemp, Warning, TEXT("%s playing hit montage, duration: %.2f"), *GetName(), Duration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no hit montage for direction %d"), *GetName(), (int32)HitDirection);
	}
}

void ABaseCharacter::OnHitReaction(EHitDirection HitDirection, AActor* DamageCauser)
{
	if (bUseHitAnimations)
	{
		PlayHitAnimation(HitDirection);
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return 0.f;
	
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	Health = FMath::Clamp(Health - DamageApplied, 0.f, MaxHealth);
	
	// Guardar la dirección del golpe
	LastHitDirection = GetHitDirection(DamageCauser);
	
	UE_LOG(LogTemp, Warning, TEXT("%s received %.1f damage from direction %d. Health: %.1f"), 
		*GetName(), DamageApplied, (int32)LastHitDirection, Health);
	
	if (Health <= 0.f)
	{
		bIsDead = true;
		
		// Detener movimiento inmediatamente
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();
		}
		
		// Reproducir la animación de hit según la dirección del golpe
		if (bUseHitAnimations)
		{
			PlayHitAnimation(LastHitDirection);
		}
		
		// Programar el ragdoll después de la animación
		GetWorldTimerManager().SetTimer(RagdollTimerHandle, this, &ABaseCharacter::ActivateRagdoll, DeathAnimationDuration, false);
		
		// Llamar OnDeath para efectos adicionales (sonidos, etc.)
		OnDeath();
	}
	else if (DamageApplied > 0.f)
	{
		// No murió, reproducir reacción al golpe
		OnHitReaction(LastHitDirection, DamageCauser);
	}
	
	return DamageApplied;
}

void ABaseCharacter::ActivateRagdoll()
{
	UE_LOG(LogTemp, Warning, TEXT("%s ragdoll activated"), *GetName());
	
	// Detener cualquier montage que esté reproduciéndose
	StopAnimMontage();
	
	// Activar ragdoll
	if (GetMesh())
	{
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	}
	
	// Detener colisiones de la cápsula
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABaseCharacter::OnDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died!"), *GetName());
}
