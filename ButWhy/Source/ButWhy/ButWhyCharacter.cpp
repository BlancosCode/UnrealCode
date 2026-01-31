// Copyright Epic Games, Inc. All Rights Reserved.

#include "ButWhyCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ButWhy.h"

AButWhyCharacter::AButWhyCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 150.f;
	GetCharacterMovement()->AirControl = 0.05f;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->GroundFriction = 0.8f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 30.0f);
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AButWhyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AButWhyCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AButWhyCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AButWhyCharacter::Look);
	}
	else
	{
		UE_LOG(LogButWhy, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AButWhyCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AButWhyCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AButWhyCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AButWhyCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AButWhyCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AButWhyCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AButWhyCharacter::BeginPlay()
{
	Super::BeginPlay();
	currentTimeLife = MaxTimeLife;

	CameraBoom=FindComponentByClass<USpringArmComponent>();
}

void AButWhyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool IsStumbling=false;
	if(GetMesh()->GetAnimInstance() && StumbleMontage)
	{
		IsStumbling=GetMesh()->GetAnimInstance()->Montage_IsPlaying(StumbleMontage);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if(PC && !bIsDead){
		PC->SetIgnoreMoveInput(IsStumbling);
	}

	if(bStartDeathCam && CameraBoom)
	{
		float NewArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, DeathArmLength, DeltaTime, CameraTransitionSpeed);
		CameraBoom->TargetArmLength = NewArmLength;

		FVector CurrentOffset=CameraBoom->SocketOffset;
		float NewZ=FMath::FInterpTo(CurrentOffset.Z, DeathZOffset, DeltaTime, CameraTransitionSpeed);
		CameraBoom->SocketOffset=FVector(CurrentOffset.X,CurrentOffset.Y,NewZ);
	}
	if (bIsDead){return;}

	currentTimeLife -= DeltaTime;
	if (currentTimeLife <= 0.0f)
	{
		HandleDeath();
	}
	else if (GetVelocity().Size() > 10.0f)
	{
		CheckForStumble();
	}

	
}

void AButWhyCharacter::HandleDeath()
{
	if (bIsDead) return;

	bStartDeathCam = true;
	bIsDead = true;


	APlayerController* PC = Cast<APlayerController>(GetController());	
	if (PC)
	{
		PC->SetCinematicMode(true, false, false, true, true);
		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(true);
	}
	GetCharacterMovement()->DisableMovement();
	
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	//va aggiunto il fade in nero e le scritte migliori nel prossimo sprint
	UE_LOG(LogButWhy, Warning, TEXT("You died"));
}

void AButWhyCharacter::CheckForStumble()
{
	
	float StumbleChance = FMath::FRandRange(0.0f, 100.0f);
	if (StumbleChance < 0.05f) // 0.05% chance to stumble each tick while moving
	{	
		PlayAnimMontage(StumbleMontage);
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC){
			PC->PlayDynamicForceFeedback(0.5f, 0.2f, true, true, true, true, EDynamicForceFeedbackAction::Start);
		}
	}
}