// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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

#pragma endregion

#pragma region INPUT ACTIONS & MAPPING CONTEXT

public:

protected:

private:

	// Player movement mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputMappingContext* PlayerMovementContext;

	// Player movement input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerMovementAction;

	// Player looking input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerLookAction;

	// Player looking input action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = true))
		UInputAction* PlayerJumpAction;

#pragma endregion

#pragma region FUNCTIONS

public:

protected:

private:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

#pragma endregion

#pragma region VARIABLES

public:

protected:

private:

#pragma endregion

};
