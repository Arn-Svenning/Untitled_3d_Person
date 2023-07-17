// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "ThrowingWeaponBase.generated.h"

/// <summary>
/// Used for checking and setting the state of the throwing weapon
/// </summary>
UENUM(BlueprintType)
enum ThrowingWeaponState
{
	Idle UMETA(DisplayName = "Idle Throwing Weapon"),
	Launched UMETA(DisplayName = "Throwing Weapon Launched"),
	Lodged UMETA(DisplayName = "Throwing Weapon Lodged"),
	Wiggle UMETA(DisplayName = "Wiggle Lodged Throwing Weapon"),
	Returning UMETA(DisplayName = "Throwing Weapon Returning")
};

class UCapsuleComponent;
class UProjectileMovementComponent;
class APlayerCharacterBase;

UCLASS()
class WEAPON_API AThrowingWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowingWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets or gets the current state of the throwing weapon
	UPROPERTY(BlueprintReadWrite)
		TEnumAsByte<ThrowingWeaponState> CurrentThrowingWeaponState;

#pragma region COMPONENTS

public:

	// Mesh
	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, category = "Mesh")
		UStaticMeshComponent* ThrowingWeaponMeshComponent;

protected:

private:

	// Manage position of throwing weapon instance
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Scene", meta = (AllowPrivateAccess = true))
		USceneComponent* DeafultSceneRootComponent;

	// The point where the throwing weapon pivots
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Scene", meta = (AllowPrivateAccess = true))
		USceneComponent* PivotPointComponent; 

	// The point where the throwing weapon lodges
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Scene", meta = (AllowPrivateAccess = true))
		USceneComponent* LodgePointComponent; 

	// Necessary for projectile motions
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = true))
		UProjectileMovementComponent* ProjectileMovementComponent; 

	// Timeline component for throwing weapon rotation forward	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UTimelineComponent* TLThrowingWeaponRotationForwardComponent;

	// Timeline for the gravitation to kick in
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UTimelineComponent* TLWeaponThrowTraceComponent; 

	// Timeline for return movement (mainly speed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UTimelineComponent* TLThrowingWeaponReturnComponent; 

	// Timeline for wiggling the lodged throwing weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UTimelineComponent* TLWiggleThrowingWeaponComponent; 
#pragma endregion

#pragma region FUNCTIONS

public:

protected:		
	
	UFUNCTION()
		virtual void ThrowWeapon(FRotator cameraRotation, FVector throwDirection, FVector cameraLocation, const float throwSpeed); // All the logic gathered for throwing the weapon (called in children)

	UFUNCTION()
		virtual void RecallThrowingWeapon(); // All the logic gathered for Recalling the throwing weapon (called in children)

private:

	UFUNCTION()
		void TLThrowingWeaponRotationForwardFloatUpdate(float value); // Timeline update for throwing weapon forward rotation

	UFUNCTION()
		void TLThrowingWeaponRotationForwardFinished(); // Timeline finished throwing weapon forward rotation 

	UFUNCTION()
		void TLWeaponThrowTraceFloatUpdate(float value); // Timeline update for launched throwing weapon (trajectory and collision)

	UFUNCTION()
		void TLWeaponThrowTraceFinished(); // Timeline finished for launched throwing weapon

	UFUNCTION()
		void TLStopWeaponThrowTrace(); // Stops the throwing weapon trajectory trajectory

	UFUNCTION()
		void TLCalculateThrowingWeaponReturnPosition(); // Timeline Update for recalling the throwing weapon

	UFUNCTION()
		void TLThrowingWeaponReturnFinished(); // Timeline finished for recalling the throwing weapon

	UFUNCTION()
		void TLWiggleLodgedThrowingWeaponFloatUpdate(float value); // Timeline update for wiggling the lodged throwing weapon before recalling 

	UFUNCTION()
		void TLWiggleLodgedThrowingWeaponFinished(); // Timeline finished for wiggling the lodged throwing weapon

	UFUNCTION()
		void StartThrowingWeaponRotationForward(); // Starts the weapon rotation when it's thrown

	UFUNCTION()
		void StopThrowingWeaponRotation(); // Stops the weapon rotation	
	
	UFUNCTION()
		void SnapThrowingWeaponToStartPosition(); // Snap throwing weapon to center of screen ( corshair)
	
	UFUNCTION()
		void LaunchThrowingWeapon(); // Throw logic

	UFUNCTION()
		void LodgeThrowingWeapon(); // Lodge throwing weapon 

	UFUNCTION()
		void AdjustThrowingWeaponReturnLocation(); // Adjust return location of throwing weapon based on player location

	UFUNCTION()
		void ReturnPosition(); // Calculate return time

	UFUNCTION()
		void CalculateThrowingWeaponReturn(float speedCurve); // Calculate the return for all the return timeline curves

	UFUNCTION()
		void WiggleLodgedThrowingWeapon(); // Logic for wiggling the lodged throwing weapon

	UFUNCTION()
		void ResetWiggleTimerDelay(); // Timer for how long the throwing weapon should wiggle 

	UFUNCTION()
		void ResetThrowingWeaponReturnDelay(); // Reset the timer for how long the throwing weapon should wiggle
		
	UFUNCTION()
		FRotator ReturnCameraStartRotation(); // // Returns the rotation of the camera

	UFUNCTION()
		FRotator MakeRotationFromAxes(FVector Forward, FVector Right, FVector Up); // Return a rotation based on the current axis

	UFUNCTION()
		FVector AdjustThrowingWeaponImpactLocation(FVector impactNormal, FVector impactLocation); // Adjust how the throwing weapon should impact

	UFUNCTION()
		float AdjustThrowingWeaponImpactPitch(FVector impactNormal, float inclinedSurfaceRange, float regularSurfaceRange); // Adjusts the vertical rise of the throwing weapon handle based on surface

	UFUNCTION()
		float GetClampedThrowingWeaponDistanceFromPlayer(float maxDistance); // Gets the max distance from player 

	UFUNCTION(BlueprintCallable, BlueprintPure)
		float CalculateThrowingWeaponReturnTimelineSpeed(float optimalDistance, float throwingWeaponReturnSpeed); // Checks how long it will take for the throwing weapon to return to the player

	
#pragma endregion

#pragma region VARIABLES

public:

protected:
	
	// How fast the throwing weapon spins while thrown
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon")
		float ThrowingWeaponSpinRate;

	// Rotation multiplier for spin
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon")
		float ThrowingWeaponRotationMultiplier; 

	// Speed for throwing weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon")
		float WeaponThrowSpeed; 

	// How far ahead the weapon will start its course
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon")
		float WeaponThrowDirectionMultiplier; 

	// How fast the throwing weapon should return
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing Weapon")
		float ThrowingWeaponReturnSpeed;

	

private:

	// Reference to the player character
	UPROPERTY(BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = true))
		APlayerCharacterBase* PlayerReference;

	// Handles rotation forward 
	UPROPERTY(EditDefaultsOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UCurveFloat* TLThrowingWeaponRotationForward_Curve;

	// Throwing weapon trajectory
	UPROPERTY(EditDefaultsOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UCurveFloat* TLWeaponThrowTrace_Curve;	

	// How fast the throwing weapon takes to return to the player
	UPROPERTY(EditDefaultsOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UCurveFloat* TLThrowingWeaponReturnSpeed_Curve;	

	// How much the throwing weapon should wiggle when lodged before recalling
	UPROPERTY(EditDefaultsOnly, Category = "Timeline", meta = (AllowPrivateAccess = true))
		UCurveFloat* TLWiggleThrowingWeapon_Curve;
		
	UPROPERTY()
		FRotator StartCameraRotation; // Initial camera rotation when weapon was thrown	

	UPROPERTY()
		FRotator InitialRotation; // Initial Throwing weapon rotation when recalling

	UPROPERTY()
		FRotator LodgePointBaseRotation; // Lodged throwing weapon rotation

	UPROPERTY()
		FVector ThrowDirection; // Direction in which the weapon was thrown

	UPROPERTY()
		FVector CameraLocationAtThrow; // Camera direction when weapon was thrown 

	UPROPERTY()
		FVector ImpactLocation; // Throwing weapon impact location

	UPROPERTY()
		FVector ImpactNormal; // Throwing weapon impact normal

	UPROPERTY()
		FVector InitialLocation; // Initial throwing weapon location when it is lodged

	UPROPERTY()
		FVector ReturnTargetLocation; // The target to where the throwing weapon will return (i.e the player)

	UPROPERTY()
		float OptimalDistance; // Used to determine how fast the throwing weapon should return based on the speed curve (max duration)

	UPROPERTY()
		float DistanceFromPlayer; // How far away the player is 

	UPROPERTY()
		float MaxCalculationDistanceFromPlayer; // Used to set the value for distanceFromPlayer

	UPROPERTY()
		bool bIsThrowingWeaponReturnDelayFinished; // Checks based on timer for how long the throwing weapon should wiggle before recalling
	

#pragma endregion

#pragma region DELEGATE

public:

protected:

private:
	
	// Delegate for throwing weapon rotation timeline
	FOnTimelineFloat ThrowingWeaponRotationForwardInterpFunction{}; // Update
	FOnTimelineEvent ThrowingWeaponRotationForwardTimelineFinished; // Finished

	// Delegate for throwing weapon throw timeline
	FOnTimelineFloat WeaponThrowTraceInterpFunction{}; // Update
	FOnTimelineEvent WeaponThrowTraceTimelineFinished; // Finished

	// Delegate for throwing weapon return timeline
	FOnTimelineFloat ThrowingWeaponReturnInterpFunction{}; // Update
	FOnTimelineFloat ThrowingWeaponReturnSpeedValue{}; // Update
	FOnTimelineFloat ThrowingWeaponReturnRotTwoValue{}; // Update
	FOnTimelineFloat ThrowingWeaponReturnVectorValue{}; // Update
	FOnTimelineEvent ThrowingWeaponReturnTickEvent; // Update
	FOnTimelineEvent ThrowingWeaponReturnFinished; // Finished

	// Timer delegate for lodged throwing weapon wiggle
	FTimerHandle ThrowingWeaponWiggleTimerDelay;
	FTimerHandle ThrowingWeaponReturnDelay;

	// Delegate for lodged axe wiggle timeline
	FOnTimelineFloat WiggleLodgedThrowingWeaponInterpFunction{}; // Update
	FOnTimelineEvent WiggleLodgedThrowingWeaponTimelineFinished; // Update

#pragma endregion


};
