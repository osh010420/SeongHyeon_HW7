// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;

struct FInputActionValue;

UCLASS()
class MYPROJECT_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* Capsule;
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;
	
	// 전방선언 struct FInputActionValue;
	UFUNCTION()
	void Move(const FInputActionValue& value);
	
	UFUNCTION()
	void Look(const FInputActionValue& value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
