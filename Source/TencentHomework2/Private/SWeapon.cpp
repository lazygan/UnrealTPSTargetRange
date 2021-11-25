// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp  = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName= "Target";
	

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if(!MyOwner) return;
	
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);

	FHitResult Hit;
	FVector TraceEnd = EyeLocation + (EyeRotation.Vector()*1000000);
	FVector TracerEndPoint = TraceEnd;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex=true;
	
	if(GetWorld()->LineTraceSingleByChannel(Hit,EyeLocation,TraceEnd,ECC_Visibility,QueryParams))
	{
		AActor* HitActor = Hit.GetActor();
		UPrimitiveComponent* HitComp = Hit.GetComponent();
		//只有击中才开始仿真
		//if(HitComp->CanEditSimulatePhysics())
			//HitComp->SetSimulatePhysics(true);
		if ((HitActor!= NULL) && (HitActor!= this) && (HitComp!= NULL) && HitComp->IsSimulatingPhysics())
		{
			FVector ShotDirection = EyeRotation.Vector();
			ShotDirection.Normalize();
			TracerEndPoint = Hit.ImpactPoint;
			HitComp->AddImpulseAtLocation(ShotDirection* 30000.0f,Hit.ImpactPoint);
			
			if(ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),ImpactEffect,Hit.ImpactPoint,Hit.ImpactNormal.Rotation());
			}
		}
		
	}
	
	//DrawDebugLine(GetWorld(),EyeLocation,TraceEnd,FColor::White,false,1.0f,0,1.0f);
	
	if(MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}
	//if(TracerEffect)
	//{
	//	UParticleSystemComponent* TracerComp =  UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),TracerEffect,MeshComp->GetSocketLocation(MuzzleSocketName));
	//	if(TracerComp)
	//	{
	//			TracerComp->SetVectorParameter(TracerTargetName,TracerEndPoint);
	//	}
	//}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
Super::Tick(DeltaTime);

}

