// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultThrowingWeapon.h"

ADefaultThrowingWeapon::ADefaultThrowingWeapon() : AThrowingWeaponBase{}
{

}

void ADefaultThrowingWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADefaultThrowingWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
// Launch the throwing weapon
void ADefaultThrowingWeapon::ThrowWeapon(FRotator cameraRotation, FVector throwDirection, FVector cameraLocation, const float throwSpeed)
{
	Super::ThrowWeapon(cameraRotation, throwDirection, cameraLocation, throwSpeed);
}
// Recall the throwing weapon
void ADefaultThrowingWeapon::RecallThrowingWeapon()
{
	Super::RecallThrowingWeapon();
}



