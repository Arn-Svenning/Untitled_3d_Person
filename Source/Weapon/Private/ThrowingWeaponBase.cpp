// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowingWeaponBase.h"
#include "GameFrameWork/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "PlayerCharacter/Public/PlayerCharacterBase.h"

// Sets default values
AThrowingWeaponBase::AThrowingWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// <summary>
	/// Normal components
	/// </summary>
	DeafultSceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));
	RootComponent = DeafultSceneRootComponent;

	PivotPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot Point"));
	PivotPointComponent->SetupAttachment(DeafultSceneRootComponent);

	LodgePointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Lodge Point"));
	LodgePointComponent->SetupAttachment(PivotPointComponent);

	ThrowingWeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ThrowingWeaponMeshComponent->SetupAttachment(LodgePointComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));

	/// <summary>
	/// Timelines
	/// </summary>
	
	// Throwing weapon rotation timeline
	TLThrowingWeaponRotationForwardComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Weapon Rotation Forward"));
	ThrowingWeaponRotationForwardInterpFunction.BindUFunction(this, FName("TLThrowingWeaponRotationForwardFloatUpdate"));
	ThrowingWeaponRotationForwardTimelineFinished.BindUFunction(this, FName("TLThrowingWeaponRotationForwardFinished"));

	// Throw curve timeline (i.e gravitation)
	TLWeaponThrowTraceComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Throwing Weapon Airtime"));
	WeaponThrowTraceInterpFunction.BindUFunction(this, FName("TLWeaponThrowTraceFloatUpdate"));
	WeaponThrowTraceTimelineFinished.BindUFunction(this, FName("TLWeaponThrowTraceFinished"));

	// Throwing weapon return timeline
	TLThrowingWeaponReturnComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Axe Return Time"));
	ThrowingWeaponReturnTickEvent.BindUFunction(this, FName("TLCalculateThrowingWeaponReturnPosition"));
	ThrowingWeaponReturnFinished.BindUFunction(this, FName("TLThrowingWeaponReturnFinished"));

	// Throwing weapon wiggle timeline
	TLWiggleThrowingWeaponComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Wiggle Axe Time"));
	WiggleLodgedThrowingWeaponInterpFunction.BindUFunction(this, FName("TLWiggleLodgedThrowingWeaponFloatUpdate"));
	WiggleLodgedThrowingWeaponTimelineFinished.BindUFunction(this, FName("TLWiggleLodgedThrowingWeaponFinished"));

}

// Called when the game starts or when spawned
void AThrowingWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerReference = Cast<APlayerCharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	
}

// Called every frame
void AThrowingWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Start the timeline for rotating the throwing weapon forward
void AThrowingWeaponBase::StartThrowingWeaponRotationForward()
{
	if (TLThrowingWeaponRotationForward_Curve)
	{		
		TLThrowingWeaponRotationForwardComponent->AddInterpFloat(TLThrowingWeaponRotationForward_Curve, ThrowingWeaponRotationForwardInterpFunction, FName("Rotation"));		
	}
	TLThrowingWeaponRotationForwardComponent->SetPlayRate(ThrowingWeaponSpinRate);
	TLThrowingWeaponRotationForwardComponent->SetTimelineFinishedFunc(ThrowingWeaponRotationForwardTimelineFinished);
	TLThrowingWeaponRotationForwardComponent->SetLooping(false);
	TLThrowingWeaponRotationForwardComponent->PlayFromStart();
}
// Stops the throwing weapon forward rotation
void AThrowingWeaponBase::StopThrowingWeaponRotation()
{
	TLThrowingWeaponRotationForwardFinished();
}
// Launch the throwing weapon
void AThrowingWeaponBase::ThrowWeapon(FRotator cameraRotation, FVector throwDirection, FVector cameraLocation, const float throwSpeed)
{
	StartCameraRotation = cameraRotation;
	ThrowDirection = throwDirection;
	CameraLocationAtThrow = cameraLocation;

	if (TLWeaponThrowTrace_Curve != nullptr)
	{
		TLWeaponThrowTraceComponent->AddInterpFloat(TLWeaponThrowTrace_Curve, WeaponThrowTraceInterpFunction, FName("Axe Throw Trace Time"));		
	}

	TLWeaponThrowTraceComponent->SetTimelineFinishedFunc(WeaponThrowTraceTimelineFinished);
	TLWeaponThrowTraceComponent->SetLooping(true);

	SnapThrowingWeaponToStartPosition();
	TLWeaponThrowTraceComponent->PlayFromStart();
	LaunchThrowingWeapon();
}
// Return the throwing weapon to player
void AThrowingWeaponBase::RecallThrowingWeapon()
{
	TLStopWeaponThrowTrace();
	ThrowingWeaponMeshComponent->SetVisibility(true, false);
	ThrowingWeaponMeshComponent->SetRelativeRotation(FRotator(0, 0, 0));
	AdjustThrowingWeaponReturnLocation();

	switch (CurrentThrowingWeaponState)
	{
	case ThrowingWeaponState::Launched:

		CurrentThrowingWeaponState = ThrowingWeaponState::Returning;

		ReturnPosition();
		StopThrowingWeaponRotation();
		PivotPointComponent->SetRelativeRotation(FRotator(0, 0, 0));
		break;


	case ThrowingWeaponState::Lodged:
		WiggleLodgedThrowingWeapon();
		GetWorld()->GetTimerManager().SetTimer(ThrowingWeaponReturnDelay, this, &AThrowingWeaponBase::ResetThrowingWeaponReturnDelay, 0.2, false);
		ReturnPosition();
		break;

	}
}
// The timeline update for rotating the throwing weapon forward
void AThrowingWeaponBase::TLThrowingWeaponRotationForwardFloatUpdate(float value)
{
	PivotPointComponent->SetRelativeRotation(FRotator(value * ThrowingWeaponRotationMultiplier, 0, 0), false, false);
}
// The Timeline finished for rotating the throwing weapon forward
void AThrowingWeaponBase::TLThrowingWeaponRotationForwardFinished()
{
	TLThrowingWeaponRotationForwardComponent->Stop();
}
// The timeline update for throwing weapon trajectory and collision
void AThrowingWeaponBase::TLWeaponThrowTraceFloatUpdate(float value)
{
	float WeaponThrowTraceDistance = 60;
	FVector start = GetActorLocation();
	FVector end = ((GetActorForwardVector() * WeaponThrowTraceDistance) + start);

	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	bool bHit = UKismetSystemLibrary::LineTraceSingle(this, start, end, UEngineTypes::ConvertToTraceType(ECC_Visibility),
		true, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResult, true, FLinearColor::Yellow, FLinearColor::Red, 1);

	if (bHit)
	{
		ImpactLocation = HitResult.ImpactPoint;
		ImpactNormal = HitResult.ImpactNormal;

		ProjectileMovementComponent->Deactivate();

		TLStopWeaponThrowTrace();

		LodgeThrowingWeapon();
	}
}
// Timeline for finished throwing weapon trajectory
void AThrowingWeaponBase::TLWeaponThrowTraceFinished()
{	
	ProjectileMovementComponent->Deactivate();
	StopThrowingWeaponRotation();

	ThrowingWeaponMeshComponent->SetVisibility(false);
}
// Immediately stop the throwing weapon movement
void AThrowingWeaponBase::TLStopWeaponThrowTrace()
{
	TLWeaponThrowTraceComponent->Stop();
}
// Handle the return position based on the return speed curve
void AThrowingWeaponBase::TLCalculateThrowingWeaponReturnPosition()
{
	CalculateThrowingWeaponReturn(		
		TLThrowingWeaponReturnSpeed_Curve->GetFloatValue(TLThrowingWeaponReturnComponent->GetPlaybackPosition())		
	);
}
// Timeline for finished throwing weapon return
void AThrowingWeaponBase::TLThrowingWeaponReturnFinished()
{
	ProjectileMovementComponent->Deactivate();
	StopThrowingWeaponRotation();

	PlayerReference->CatchThrowingWeapon();
	bIsThrowingWeaponReturnDelayFinished = true;
}
// Timeline for updating the lodged throwing weapon wiggle
void AThrowingWeaponBase::TLWiggleLodgedThrowingWeaponFloatUpdate(float value)
{
	LodgePointComponent->SetRelativeRotation(FRotator(LodgePointBaseRotation.Pitch + value * -30, LodgePointBaseRotation.Yaw, LodgePointBaseRotation.Roll));
}
// Timeline for finished wiggle 
void AThrowingWeaponBase::TLWiggleLodgedThrowingWeaponFinished()
{
	CurrentThrowingWeaponState = ThrowingWeaponState::Returning;
}
// Snap the throwing weapon to center of screen (corshair)
void AThrowingWeaponBase::SnapThrowingWeaponToStartPosition()
{
	SetActorLocationAndRotation((ThrowDirection * WeaponThrowDirectionMultiplier + CameraLocationAtThrow) - PivotPointComponent->GetRelativeLocation(), ReturnCameraStartRotation());
}
// Launch the throwing weapon
void AThrowingWeaponBase::LaunchThrowingWeapon()
{
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	CurrentThrowingWeaponState = ThrowingWeaponState::Launched;

	ThrowingWeaponMeshComponent->SetRelativeRotation(FRotator(0, 180, 0));

	ProjectileMovementComponent->Activate();

	ProjectileMovementComponent->Velocity = ThrowDirection * WeaponThrowSpeed;

	StartThrowingWeaponRotationForward();
}
// Lodge throwing weapon on impact
void AThrowingWeaponBase::LodgeThrowingWeapon()
{
	ProjectileMovementComponent->Deactivate();
	StopThrowingWeaponRotation();

	PivotPointComponent->SetRelativeRotation(FRotator(0, 0, 0));

	SetActorRotation(StartCameraRotation);

	// Calculate the rotation for the lodge point based on the projectile's velocity
	FRotator LodgeRotation = ProjectileMovementComponent->Velocity.ToOrientationRotator();

	// Adjust the pitch of the rotation based on the impact normal (The vertical rotation)
	LodgeRotation.Pitch += AdjustThrowingWeaponImpactPitch(ImpactNormal, FMath::RandRange(-30, -45), FMath::RandRange(-25, -35));

	// Set the rotation for the lodge point
	LodgePointComponent->SetRelativeRotation(LodgeRotation);

	// Adjust the location of the projectile based on the impact location and normal
	SetActorLocation(AdjustThrowingWeaponImpactLocation(ImpactNormal, ImpactLocation));

	CurrentThrowingWeaponState = ThrowingWeaponState::Lodged;
}
// Adjust where the throwing weapon will return
void AThrowingWeaponBase::AdjustThrowingWeaponReturnLocation()
{
	if (PlayerReference != nullptr)
	{
		MaxCalculationDistanceFromPlayer = 3000;
		DistanceFromPlayer = GetClampedThrowingWeaponDistanceFromPlayer(MaxCalculationDistanceFromPlayer);

		FVector newActorLocation = GetActorLocation();
		SetActorLocation(newActorLocation);
		
		InitialLocation = GetActorLocation();

		InitialRotation = GetActorRotation();

		StartCameraRotation = PlayerReference->FollowCameraComponent->GetComponentRotation();		

		LodgePointComponent->SetRelativeRotation(FRotator(0, 0, 0));
	}
}

// Return position and speed
void AThrowingWeaponBase::ReturnPosition()
{
	TLThrowingWeaponReturnComponent->PlayFromStart();
	OptimalDistance = 1400;
	
	TLThrowingWeaponReturnComponent->SetPlayRate(CalculateThrowingWeaponReturnTimelineSpeed(OptimalDistance, ThrowingWeaponReturnSpeed));
	if (TLThrowingWeaponReturnSpeed_Curve)
	{		
		TLThrowingWeaponReturnComponent->AddInterpFloat(TLThrowingWeaponReturnSpeed_Curve, ThrowingWeaponReturnSpeedValue, FName("Update Speed"));		
	}

	TLThrowingWeaponReturnComponent->SetTimelinePostUpdateFunc(ThrowingWeaponReturnTickEvent);
	TLThrowingWeaponReturnComponent->SetTimelineFinishedFunc(ThrowingWeaponReturnFinished);

	TLThrowingWeaponReturnComponent->SetLooping(false);
}

// The speed, location (player location) and rotation when recalling the throwing weapon                    
void AThrowingWeaponBase::CalculateThrowingWeaponReturn(float speedCurve) //  Rework method to properly handle returning without the need for speedCurve
{	
	if (PlayerReference != nullptr)
	{
		bIsThrowingWeaponReturnDelayFinished = false;
		
		FVector CharacterLocation = PlayerReference->FollowCameraComponent->GetRightVector()
			+ PlayerReference->GetMesh()->GetSocketLocation(FName("WeaponGripPoint"));

		FVector lerpActorRightVector = FMath::Lerp(InitialLocation, CharacterLocation, speedCurve);

		ReturnTargetLocation = lerpActorRightVector;

		FRotator socketRotation = PlayerReference->GetMesh()->GetSocketRotation(FName("WeaponGripPoint"));

		SetActorLocationAndRotation(ReturnTargetLocation, socketRotation, false, 0, ETeleportType::None);

	}
}
// Wiggle the lodged throwing weapon
void AThrowingWeaponBase::WiggleLodgedThrowingWeapon()
{
	LodgePointBaseRotation = LodgePointComponent->GetRelativeRotation();

	if (TLWiggleThrowingWeapon_Curve)
	{
		GetWorld()->GetTimerManager().SetTimer(ThrowingWeaponWiggleTimerDelay, this, &AThrowingWeaponBase::ResetWiggleTimerDelay, 0.2, false);
		
		TLWiggleThrowingWeaponComponent->AddInterpFloat(TLWiggleThrowingWeapon_Curve, WiggleLodgedThrowingWeaponInterpFunction, FName("Lodged Throwing Weapon Wiggle Time"));

		TLWiggleThrowingWeaponComponent->SetLooping(false);
	}
	TLWiggleThrowingWeaponComponent->SetPlayRate(3);
	TLWiggleThrowingWeaponComponent->PlayFromStart();
	TLWiggleThrowingWeaponComponent->SetTimelineFinishedFunc(WiggleLodgedThrowingWeaponTimelineFinished);
}
// Reset the wiggle so that the timer is available for next throw
void AThrowingWeaponBase::ResetWiggleTimerDelay()
{	
	GetWorldTimerManager().ClearTimer(ThrowingWeaponWiggleTimerDelay);
}
// Check for the wiggle timer finished event 
void AThrowingWeaponBase::ResetThrowingWeaponReturnDelay()
{
	bIsThrowingWeaponReturnDelayFinished = true;
}
// Returns the rotation of the camera
FRotator AThrowingWeaponBase::ReturnCameraStartRotation()
{
	float axeSpinOffsetC = 0;

	return FRotator(StartCameraRotation.Pitch, StartCameraRotation.Yaw, StartCameraRotation.Roll + axeSpinOffsetC);
}
// Return a rotation based on the current axis
FRotator AThrowingWeaponBase::MakeRotationFromAxes(FVector forward, FVector right, FVector up)
{
	forward.Normalize();
	right.Normalize();
	up.Normalize();

	FMatrix rotationMatrix(forward, right, up, FVector::ZeroVector);

	return rotationMatrix.Rotator();
}
// Adjust how the throwing weapon should impact
FVector AThrowingWeaponBase::AdjustThrowingWeaponImpactLocation(FVector impactNormal, FVector impactLocation)
{
	FVector right = FVector(0, 0, 0);
	FVector up = FVector(0, 0, 0);

	FVector vectorAdditionVariable = FVector(0, 0, 0);
	FVector returningVariable = FVector(0, 0, 0);

	if (MakeRotationFromAxes(impactNormal, right, up).Pitch > 0)
	{
		impactNormal.Y = ((MakeRotationFromAxes(impactNormal, right, up).Pitch - 90) / 90) * 10;

		vectorAdditionVariable = impactNormal;

		impactLocation = impactLocation + vectorAdditionVariable.Z;

		returningVariable = LodgePointComponent->GetComponentLocation() - GetActorLocation() + impactLocation;
	}
	else
	{
		returningVariable = GetActorLocation() - LodgePointComponent->GetComponentLocation() + impactLocation;
	}

	return returningVariable;
}
// Adjusts the vertical rise of the throwing weapon handle based on surface
float AThrowingWeaponBase::AdjustThrowingWeaponImpactPitch(FVector impactNormal, float inclinedSurfaceRange, float regularSurfaceRange)
{
	FVector right = FVector(0, 0, 0);
	FVector up = FVector(0, 0, 0);

	if (MakeRotationFromAxes(impactNormal, right, up).Pitch > 0)
	{
		regularSurfaceRange = inclinedSurfaceRange - MakeRotationFromAxes(impactNormal, right, up).Pitch;

		if (MakeRotationFromAxes(impactNormal, right, up).Pitch > -80)
		{
			return -90;
		}
		return regularSurfaceRange;
	}
	else
	{
		if (MakeRotationFromAxes(impactNormal, right, up).Pitch > -80)
		{
			return -90;
		}
		return inclinedSurfaceRange;
	}
}
// Gets the max distance from player 
float AThrowingWeaponBase::GetClampedThrowingWeaponDistanceFromPlayer(float maxDistance)
{
	FVector distanceFromWeaponToPlayer = GetActorLocation() - PlayerReference->GetMesh()->GetSocketLocation(FName("WeaponGripPoint"));
	float clampedWeaponDistanceFromPlayer = FMath::Clamp(distanceFromWeaponToPlayer.Size(), 0, maxDistance);

	return clampedWeaponDistanceFromPlayer;
}
// Checks how long it will take for the throwing weapon to return to the player
float AThrowingWeaponBase::CalculateThrowingWeaponReturnTimelineSpeed(float optimalDistance, float throwingWeaponReturnSpeed)
{
	float calculateReturnSpeed = (optimalDistance * ThrowingWeaponReturnSpeed) / DistanceFromPlayer;
	float ClampCalculateReturnSpeed = FMath::Clamp(calculateReturnSpeed, 0.4, 0.7);

	return ClampCalculateReturnSpeed;
}

