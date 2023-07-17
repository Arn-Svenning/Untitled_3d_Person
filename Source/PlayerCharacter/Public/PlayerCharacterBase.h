// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Struct/public/DoOnce.h"
#include "InputActionValue.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "PlayerCharacterBase.generated.h"

class USpringArmComponent;
class UInputMappingContext;
class ADefaultThrowingWeapon;
class UInputAction;
class UCameraComponent;
class UCableComponent;


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

	// Player camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* FollowCameraComponent;

protected:
	

private:

	// Holds camera at a fixed distance from player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		USpringArmComponent* CameraBoomComponent;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throwing Weapon", meta = (AllowPrivateAccess = true))
		UChildActorComponent* ThrowingWeaponChildComponent;

	// A rope that attaches the start point to the character's hand and the the end point to the throwing weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon", meta = (AllowPrivateAccess = true))
		UCableComponent* RopeComponent;

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

	// Weapon mapping context
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

	// Launch throwing weapon action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* LaunchThrowingWeaponAction;

	// Recall throwing weapon action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* ThrowingWeaponRecallAction;

#pragma endregion

#pragma region FUNCTIONS

public:

	UFUNCTION()
		void CatchThrowingWeapon(); // Catch the throwing weapon and set CurrentThrowingWeaponState == ThrowingWeaponState::Idle

protected:

private:

	UFUNCTION()
	void Move(const FInputActionValue& Value); // Default movement 

	UFUNCTION()
	void Look(const FInputActionValue& Value);	// Looking around with the camera

	UFUNCTION()
		void TLRangedCameraUpdate(float value); // Update camera position when aiming

	UFUNCTION()
		void TLRangedCameraFinished(); // Handles idle aim camera

	UFUNCTION()
		void LerpCameraPosition(float boomLengthIdle, float boomLengthAimed, float alpha); // Lerp between aimed camera and idle aim camera for smooth transition

	UFUNCTION()
		void UpdateRangedCamera(); // Handle the logic for camera

	UFUNCTION()
		void CharacterRotation(float DeltaTime); // Handle character rotation properly

	UFUNCTION()
		void Aim(); // Aiming function for all the weapons

	UFUNCTION()
		void StopAim(); // Stop the aiming for all the weapons

	UFUNCTION()
		void LaunchThrowingWeapon(); // Throw the throwing weapon 

	UFUNCTION()
		void RecallThrowingWeapon(); // Make the throwing weapon go back to the player

#pragma endregion

#pragma region VARIABLES

public:

protected:

private:

	// Reference to the default throwingWeapon
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
		ADefaultThrowingWeapon* DefaultThrowingWeaponReference;

	// A struct that lets the player do something once until it's reset
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		FDoOnce DoOnce;

	// The curve that handles lerping between aimed camera and idle camera
	UPROPERTY(EditDefaultsOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UCurveFloat* TLRangedCamera_Curve;		

	UPROPERTY()
		FVector CameraVector;

	UPROPERTY()
		FVector RangedCameraVector;

	UPROPERTY()
		float CameraTurnRate; 

	// How fast should the camera turn while not aiming?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraTurnRateIdle; 

	// How fast should the camera turn while aiming?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraTurnRateAim; 

	// How far should idle camera be from player?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraBoomIdle; 

	// How far should aimed camera be from player?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = true))
		float CameraBoomAimed; 

	// How fast should the player walk when aiming?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
		float MaxWalkSpeedAim;

	// How fast should player be when not aiming?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
		float MaxWalkSpeedIdle;

	// Throwing weapon velocity when thrown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon", meta = (AllowPrivateAccess = true))
		float WeaponThrowSpeed;	

	UPROPERTY()
		bool bIsAiming;	// Is player aiming?

	UPROPERTY()
		bool bIsThrowingWeaponLaunched; // Is the throwing weapon launched?
	
#pragma endregion

#pragma region DELEGATE

public:

protected:

private:

	FOnTimelineFloat RangedCameraInterpFunction{}; // Delegate to track time for update ranged camera
	FOnTimelineEvent RangedCameraTimelineFinished; // Delegate to track time for ranged camera finished

#pragma endregion


};
