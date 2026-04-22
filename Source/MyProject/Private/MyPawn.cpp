// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPawn.h"
#include "EnhancedInputComponent.h"
#include "MyPlayerController.h"
#include "DrawDebugHelpers.h"
#include "MaterialHLSLTree.h"
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
	SpringArm->bUsePawnControlRotation = false; // 컨트롤러의 회전을 따라감
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // 카메라는 회전하지 않고 스프링암에 따라감
	
	Capsule->SetSimulatePhysics(false);
	SkeletalMesh->SetSimulatePhysics(false);
	
	MoveSpeed = 600.f;
	LookSensitivity = 100.f;
	PitchMin = -80.f;
	PitchMax = 80.f;
	Gravity = -980.0f;
	GroundCheckDistance = 20.0f;
	AirControlRatio = 0.4f; 
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
	bIsGrounded = CheckGround(); 
	
	FVector ForwardDir = GetActorForwardVector();
	FVector RightDir   = GetActorRightVector();
	FVector UpDir = GetActorUpVector();
	
	//도전과제
	float zInput = bIsFlying ? UpDownInput : 0.0f;
	float CurrentSpeed = bIsGrounded ? MoveSpeed : MoveSpeed * AirControlRatio;

	FVector MoveDir = (RightDir * MoveInput.Y) + (ForwardDir * MoveInput.X) + (UpDir * zInput);
	MoveDir = MoveDir.GetSafeNormal(); // 대각선 이동 시 속도 보정
	
	AddActorWorldOffset(MoveDir * CurrentSpeed * DeltaTime, true); // true = 충돌 sweep 활성화
	
	
	float YawDelta = LookInput.X * LookSensitivity * DeltaTime;
	AddActorLocalRotation(FRotator(0.f, YawDelta, 0.f));
	
	if (bIsFlying)
	{
		float RollDelta = (RollInput * LookSensitivity * DeltaTime);
		AddActorLocalRotation(FRotator(0.f, 0.f, RollDelta));
	}
	// 도전과제 중력
	if (!bIsFlying)  // 비행 모드가 아닐 때만 중력 적용
	{
		if (!bIsGrounded)
		{
			// 공중: 중력 누적
			Velocity.Z += Gravity * DeltaTime;
		}
		else if (Velocity.Z < 0)
		{
			// 착지 순간: 낙하 속도 리셋
			Velocity.Z = 0;
		}
		// Z축 이동 적용
		AddActorWorldOffset(FVector(0, 0, Velocity.Z * DeltaTime), true);
	}
	else
	{
		Velocity = FVector::ZeroVector;
	}
	
	CurrentPitch = FMath::Clamp(
		CurrentPitch + (-LookInput.Y * LookSensitivity * DeltaTime),
		PitchMin,
		PitchMax
	);
	// 도전과제
	if (!bIsFlying)
	{
		SpringArm->SetRelativeRotation(FRotator(CurrentPitch, 0.f, 0.f));
	}
	else
	{
		float PitchDelta = -LookInput.Y * LookSensitivity * DeltaTime;
		AddActorLocalRotation(FRotator(PitchDelta, 0.f, 0.0f));
	}
	LookInput = FVector2D::ZeroVector; // Look은 매 프레임 리셋
	
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
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Completed,
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
					&AMyPawn::Look
					);
			}
			//도전과제
			if (PlayerController->RollAction)
			{
				EnhancedInput->BindAction(
					PlayerController->RollAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::Roll
					);
				EnhancedInput->BindAction(
					PlayerController->RollAction,
					ETriggerEvent::Completed,
					this,
					&AMyPawn::Roll
					);
			}
			if (PlayerController->UpDownAction)
			{
				EnhancedInput->BindAction(
					PlayerController->UpDownAction,
					ETriggerEvent::Triggered,
					this,
					&AMyPawn::UpDown
					);
				EnhancedInput->BindAction(
					PlayerController->UpDownAction,
					ETriggerEvent::Completed,
					this,
					&AMyPawn::UpDown
					);
			}
			if (PlayerController->SwitchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SwitchAction,
					ETriggerEvent::Started,
					this,
					&AMyPawn::SwitchMode
					);
			}
		}
	}
}

void AMyPawn::Move(const FInputActionValue& value)
{
	MoveInput = value.Get<FVector2D>();
}

void AMyPawn::Look(const FInputActionValue& value)
{
	LookInput = value.Get<FVector2D>();
}
void AMyPawn::Roll(const FInputActionValue& value)
{
	RollInput = value.Get<float>();
}

void AMyPawn::UpDown(const FInputActionValue& value)
{
	UpDownInput = value.Get<float>();
}

void AMyPawn::SwitchMode(const FInputActionValue& value)
{
	bIsFlying = !bIsFlying; // 토글
	
	//상태별 로직 분기
	if (bIsFlying)
	{
		CurrentPitch = 0.0f;
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
	}
	else
	{
		SetActorRotation(FRotator(0.f, GetActorRotation().Yaw, 0.f));
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Flying mode: %s"), bIsFlying ? TEXT("ON") : TEXT("OFF"));
}

//라인트레이스
bool AMyPawn::CheckGround()
{
	FVector Start = GetActorLocation();
	float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	FVector End = Start - FVector(0, 0, HalfHeight);
    
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
    
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params
	);
	
	DrawDebugLine(GetWorld(), Start, End, 
		bHit ? FColor::Green : FColor::Red, 
		false, 0.1f);
    
	return bHit;
}