// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSProjectile.h"

#include <string>

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SCharacter.h"
#include "Internationalization/FastDecimalFormat.h"
#include "Kismet/GameplayStatics.h"

void AFPSProjectile::Explode()
{
	FVector ExplodeLocation = CollisionComp->GetComponentLocation();
	DamageSphere->SetWorldLocation(ExplodeLocation);
	UKismetSystemLibrary::PrintString(GetWorld(),"Explode");
	TArray<UPrimitiveComponent*> OverlappingComponent;
	DamageSphere->GetOverlappingComponents(OverlappingComponent);
	if(ExplodeEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ExplodeEffect,ExplodeLocation);
	}
	for(UPrimitiveComponent*  c: OverlappingComponent)
	{
		if(c->IsSimulatingPhysics())
		{
			FVector C_Location = c->GetComponentLocation();
			c->AddImpulseAtLocation((C_Location-ExplodeLocation)* 800.0f,C_Location);
		}
	}
    UKismetSystemLibrary::PrintString(GetWorld(),FString::FromInt(OverlappingComponent.Num()));
	Destroy();
}

AFPSProjectile::AFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(20.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);	// set up a notification for when this component hits something blocking
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	// Set as root component
	RootComponent = CollisionComp;
	
	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 0;
}


void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		
	}
	if(bFirstHit){
		bFirstHit =false;
        UKismetSystemLibrary::PrintString(GetWorld(),"SetTimer");
        GetWorldTimerManager().SetTimer(TExplodeHandle,this,&AFPSProjectile::Explode,2.0,false);
    }
}
