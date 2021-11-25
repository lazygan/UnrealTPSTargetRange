// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "FPSProjectile.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComponent);
	CameraRotator = CameraComp->GetComponentRotation();
}

void ASCharacter::ReleaseGrenade()
{
	if (ProjectileClass)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		GetActorEyesViewPoint(EyeLocation,EyeRotation);
		// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
		//// Use controller rotation which is our view direction in first person
		FVector ThrowLoc =  GetMesh()->GetSocketLocation("ThrowSocket");
		//FRotator ThrowRotation =  GetMesh()->GetSocketRotation("ThrowSocket");
		FRotator ThrowRotation=  EyeRotation;

		////Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		//// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, ThrowLoc, ThrowRotation, ActorSpawnParams);
	}
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector()*value);
}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector()*value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp",this,&ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn",this,&ASCharacter::AddControllerYawInput);
	
    GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch",IE_Released,this,&ASCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if(CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	return Super::GetPawnViewLocation();
	
}


void ASCharacter::AddControllerPitchInput(float Val)
{
	Super::AddControllerPitchInput(Val);
	CameraRotator = CameraComp->GetComponentRotation();
}


void ASCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val);
	CameraRotator = CameraComp->GetComponentRotation();
}

