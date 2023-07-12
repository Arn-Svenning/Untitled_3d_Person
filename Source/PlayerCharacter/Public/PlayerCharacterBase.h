// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "PlayerCharacterBase.generated.h"

class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;


UCLASS()
class PLAYERCHARACTER_API APlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoomComponent; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCameraComponent; }

#pragma region COMPONENTS

public:

protected:

private:

	// Holds camera at a fixed distance from player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		USpringArmComponent* CameraBoomComponent;

	// Player camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		UCameraComponent* FollowCameraComponent;

	// Timeline that handles lerping between aim camera and no aim camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UTimelineComponent* TLRangedCameraComponent;

#pragma endregion

#pragma region INPUT ACTIONS & MAPPING CONTEXT

public:

protected:

private:

	// Player movement mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputMappingContext* PlayerMovementContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputMappingContext* PlayerWeaponMappingContext;

	// Player movement input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerMovementAction;

	// Player looking input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerLookAction;

	// Player looking input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerJumpAction;

	// Player aim input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerAimAction;

#pragma endregion

#pragma region FUNCTIONS

public:

protected:

private:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	UFUNCTION()
		void LerpCameraPositionC(float boomLengthIdle, float boomLengthAimed, float alpha); // Lerp between aimed camera and idle aim camera for smooth transition

	UFUNCTION()
		void UpdateRangedCamera(); // Handle the logic for camera

	UFUNCTION()
		void TLRangedCameraUpdate(float value); // Update camera position when aiming

	UFUNCTION()
		void TLRangedCameraFinished(); // Handles idle aim camera

	UFUNCTION()
		void CharacterRotation(float DeltaTime); // Handle character rotation properly

	UFUNCTION()
		void Aim();

	UFUNCTION()
		void StopAim();

#pragma endregion

#pragma region VARIABLES

public:

protected:

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UCurveFloat* TLRangedCamera_Curve;

	FOnTimelineFloat RangedCameraInterpFunction{}; // Track time for update ranged camera
	FOnTimelineEvent RangedCameraTimelineFinished; // ranged camera finished

	UPROPERTY()
		FVector CameraVector;

	UPROPERTY()
		FVector RangedCameraVector;

	UPROPERTY()
		float CameraTurnRate; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraTurnRateIdle; // How fast should the camera turn while not aiming?

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraTurnRateAim; // How fast should the camera turn while aiming?

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraBoomIdle; // How far should idle camera be from player?

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraBoomAimed; // How far should aimed camera be from player?

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
		float MaxWalkSpeedAim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
		float MaxWalkSpeedIdle;

	UPROPERTY()
		bool bIsLeftTriggerDown;

	UPROPERTY()
		bool bUseControllerRotation;

	UPROPERTY()
		bool bIsAiming;

	

#pragma endregion

};
