// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Rotation of camera doesn't affect player rotation for Y-axis, X-axis and Z-axis
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Player rotation
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...

	// USpringArmComponent Configuration
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera boom"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->TargetArmLength = 400.f; // From how far should the Spring arm Component be placed
	CameraBoomComponent->bUsePawnControlRotation = true; // Should the Spring arm component rotate based on the player's rotation?

	// UCameraComponent Configuration
	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow camera")); // Creating cameraComponent
	FollowCameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName); // Attach the character to the Spring arm component
	FollowCameraComponent->bUsePawnControlRotation = false; // Should the Camera component rotate relative to the player's rotation?

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
		}
	}
}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}
}

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

