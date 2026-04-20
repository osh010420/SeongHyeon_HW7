// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"
#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"
#include "Components/CapsuleComponent.h"        
#include "Components/SkeletalMeshComponent.h"   
#include "Camera/CameraComponent.h"             
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule	= CreateDefaultSubobject<UCapsuleComponent>("Capsule");
	SetRootComponent(Capsule);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	SkeletalMesh->SetupAttachment(Capsule);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(Capsule);
	SpringArm->TargetArmLength = 300.0f;  // 카메라의 거리
	SpringArm->bUsePawnControlRotation = true; // 컨트롤러의 회전을 따라감
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // 카메라는 회전하지 않고 스프링암에 따라감
	
	Capsule->SetSimulatePhysics(false);
	SkeletalMesh->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* PlayerController = Cast<AMyPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Move
					);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Move
					);
			}
		}
	}
}

void AMyPawn::Move(const FInputActionValue& value)
{
	if (!Controller)
		return;
	
	const FVector2D MoveInput = value.Get<FVector2D>();
	
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AMyPawn::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	
	AddControllerPitchInput(LookInput.Y);
}