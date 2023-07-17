// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThrowingWeaponBase.h"
#include "DefaultThrowingWeapon.generated.h"

/**
 * 
 */
UCLASS()
class WEAPON_API ADefaultThrowingWeapon : public AThrowingWeaponBase
{
	GENERATED_BODY()

public:

	ADefaultThrowingWeapon();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

#pragma region COMPONENTS

public:

protected:

private:

#pragma endregion

#pragma region FUNCTIONS

public:
	
		virtual void ThrowWeapon(FRotator cameraRotation, FVector throwDirection, FVector cameraLocation, const float throwSpeed) override; // Logic from ThrowingWeaponBase used for throwing the weapon
	
		virtual void RecallThrowingWeapon() override; // Logic from ThrowingWeaponBase used for recalling the throwing weapon

protected:


private:

#pragma endregion


	
};
