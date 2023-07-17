// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "CableComponent.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon/public/DefaultThrowingWeapon.h"


// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoOnce.Reset();

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Rotation of camera doesn't affect player rotation for Y-axis, X-axis and Z-axis
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Player rotation
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...

	/// <summary>
	/// Normal components
	/// </summary>    
	
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera boom"));
	CameraBoomComponent->SetupAttachment(RootComponent);	
	CameraBoomComponent->bUsePawnControlRotation = true; // Should the Spring arm component rotate based on the player's rotation?
	
	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow camera")); // Creating cameraComponent
	FollowCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName); // Attach the character to the Spring arm component
	FollowCameraComponent->bUsePawnControlRotation = false; // Should the Camera component rotate relative to the player's rotation?

	ThrowingWeaponChildComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Throwing Weapon Child Actor"));
	ThrowingWeaponChildComponent->SetupAttachment(GetMesh(), FName("WeaponGripPoint"));
	
	RopeComponent = CreateDefaultSubobject<UCableComponent>(TEXT("Throwing Weapon Rope"));
	RopeComponent->SetupAttachment(GetMesh(), FName("RopeSocket"));
	RopeComponent->bAttachEnd = true;		

	/// <summary>
	/// Timelines
	/// </summary> 
	TLRangedCameraComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("Ranged Camera Timeline"));
	RangedCameraInterpFunction.BindUFunction(this, FName("TLRangedCameraUpdate"));
	RangedCameraTimelineFinished.BindUFunction(this, FName("TLRangedCameraFinished"));	


}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Add the mapping context
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subSystem->AddMappingContext(PlayerMovementContext, 0);
			subSystem->AddMappingContext(PlayerWeaponMappingContext, 0);
		}
	}

	TArray<FName> viableSockets = DefaultThrowingWeaponReference->ThrowingWeaponMeshComponent->GetAllSocketNames();
	for (int i = 0; i < viableSockets.Num(); i++)
	{
		if (viableSockets[i] == FName("ThrowingWeaponRope"))
		{
			RopeComponent->SetAttachEndTo(DefaultThrowingWeaponReference, FName("ThrowingWeaponMeshComponent"), viableSockets[i]);
		}
	}

	
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharacterRotation(DeltaTime);	

}

// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind functionality for the input actions	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(PlayerJumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(PlayerJumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(PlayerMovementAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Move);

		// Looking
		EnhancedInputComponent->BindAction(PlayerLookAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Look);

		// Aim
		EnhancedInputComponent->BindAction(PlayerAimAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::Aim);
		EnhancedInputComponent->BindAction(PlayerAimAction, ETriggerEvent::Completed, this, &APlayerCharacterBase::StopAim);

		// Throwing weapon
		EnhancedInputComponent->BindAction(LaunchThrowingWeaponAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::LaunchThrowingWeapon);
		EnhancedInputComponent->BindAction(ThrowingWeaponRecallAction, ETriggerEvent::Triggered, this, &APlayerCharacterBase::RecallThrowingWeapon);
	}
}
// Attach the throwing weapon to player socket (WeaponGripPoint)
void APlayerCharacterBase::CatchThrowingWeapon()
{
	if (DefaultThrowingWeaponReference != nullptr)
	{		
		RopeComponent->SetVisibility(false);

		DefaultThrowingWeaponReference->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false), FName("WeaponGripPoint"));
		
		bIsThrowingWeaponLaunched = false;

		DefaultThrowingWeaponReference->CurrentThrowingWeaponState = ThrowingWeaponState::Idle;

		DoOnce.Reset();
	}
}
// Called for default movement
void APlayerCharacterBase::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
// Called for looking around with the camera
void APlayerCharacterBase::Look(const FInputActionValue& Value)
{
	FVector2D lookingAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Yaw and pitch for the lookAxis controller input
		AddControllerYawInput(lookingAxisVector.X);
		AddControllerPitchInput(lookingAxisVector.Y);
	}
}
// Switch between idle camera and aimed camera
void APlayerCharacterBase::LerpCameraPosition(float boomLengthIdle, float boomLengthAimed, float alpha)
{
	float lerpBoomLength = FMath::Lerp(boomLengthIdle, boomLengthAimed, alpha);
	CameraBoomComponent->TargetArmLength = lerpBoomLength;
}
// Updates the camera based on whether or not the player is aiming 
void APlayerCharacterBase::UpdateRangedCamera()
{
	if (TLRangedCamera_Curve != nullptr)
	{
		TLRangedCameraComponent->AddInterpFloat(TLRangedCamera_Curve, RangedCameraInterpFunction, FName("Update Ranged Camera"));
	}
	TLRangedCameraComponent->SetPlayRate(8);
	TLRangedCameraComponent->Play();
	TLRangedCameraComponent->SetLooping(false);
}
// Update the timeline for switching between idle and aimed camera
void APlayerCharacterBase::TLRangedCameraUpdate(float value)
{
	LerpCameraPosition(CameraBoomIdle, CameraBoomAimed, value);
}
// If player isn't aiming reverse the timeline back to idle camera position
void APlayerCharacterBase::TLRangedCameraFinished()
{
	TLRangedCameraComponent->Reverse();
}
// Rotate the player accordingly when the player is aiming a weapon
void APlayerCharacterBase::CharacterRotation(float DeltaTime)
{
	if (bIsAiming)
	{
		FRotator actorRotation = GetActorRotation();
		FRotator controlRotation = GetControlRotation();

		FRotator newCharacterRotation = FRotator(actorRotation.Pitch, controlRotation.Yaw, actorRotation.Roll);
		FRotator interpRotation = FMath::RInterpTo(actorRotation, newCharacterRotation, DeltaTime, 50);

		SetActorRotation(interpRotation);
	}
}
// Aim the equipped weapon
void APlayerCharacterBase::Aim()
{	
	bIsAiming = true;	
	CameraTurnRate = CameraTurnRateAim;

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedAim;
	UpdateRangedCamera();
}
// Stop aiming the equipped weapon
void APlayerCharacterBase::StopAim()
{
	bIsAiming = false;	
	CameraTurnRate = CameraTurnRateIdle;

	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedIdle;
	TLRangedCameraComponent->SetPlayRate(8);
	TLRangedCameraComponent->Reverse();
}
// Launch the equipped throwing weapon
void APlayerCharacterBase::LaunchThrowingWeapon()
{
	if (Controller != nullptr && DefaultThrowingWeaponReference != nullptr)
	{
		if (bIsAiming)
		{			
			if (!bIsThrowingWeaponLaunched)
			{
				RopeComponent->SetVisibility(true);

				DefaultThrowingWeaponReference->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);				

				DefaultThrowingWeaponReference->ThrowWeapon(FollowCameraComponent->GetComponentRotation(), FollowCameraComponent->GetForwardVector(), GetMesh()->GetSocketLocation(FName("WeaponGripPoint")), WeaponThrowSpeed);

				bIsThrowingWeaponLaunched = true;
			}
		}
	}
}
// Recall the equipped throwing weapon 
void APlayerCharacterBase::RecallThrowingWeapon()
{
	if (Controller != nullptr && DefaultThrowingWeaponReference != nullptr)
	{
		if (bIsThrowingWeaponLaunched)
		{						
			if (DoOnce.Execute())
			{
				DefaultThrowingWeaponReference->RecallThrowingWeapon();
			}			
		}
	}
}


