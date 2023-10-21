// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TACCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TACCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Input/TACControlData.h"

#define CONSTRUCT_IA ConstructorHelpers::FObjectFinder<UInputAction>

ATACCharacterPlayer::ATACCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTACCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
		// Input
	struct FConstructorStatics
	{
		CONSTRUCT_IA InputActionFireRef;
		CONSTRUCT_IA InputActionAimRef;
		CONSTRUCT_IA InputActionLookRef;
		CONSTRUCT_IA InputActionMoveRef;
		CONSTRUCT_IA InputActionCrouchRef;
		CONSTRUCT_IA InputActionSneakRef;
		CONSTRUCT_IA InputActionSprintRef;
		CONSTRUCT_IA InputActionJumpRef;
		CONSTRUCT_IA InputActionProneRef;
		CONSTRUCT_IA InputActionMeleeRef;
		CONSTRUCT_IA InputActionChangeFireModeRef;
		CONSTRUCT_IA InputActionLeaningRef;
		FConstructorStatics()
			: InputActionFireRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Fire.IA_Fire'"))
			, InputActionAimRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Aim.IA_Aim'"))
			, InputActionLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Look.IA_Look'"))
			, InputActionMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Move.IA_Move'"))
			, InputActionCrouchRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Crouch.IA_Crouch'"))
			, InputActionSneakRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Sneak.IA_Sneak'"))
			, InputActionSprintRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Sprint.IA_Sprint'"))
			, InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Jump.IA_Jump'"))
			, InputActionProneRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Prone.IA_Prone'"))
			, InputActionMeleeRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Melee.IA_Melee'"))
			, InputActionChangeFireModeRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_ChangeFireMode.IA_ChangeFireMode'"))
			, InputActionLeaningRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Leaning.IA_Leaning'"))
		{}
	};
	static FConstructorStatics ConstructorStatics;
	if(nullptr != ConstructorStatics.InputActionFireRef.Object) FireAction = ConstructorStatics.InputActionFireRef.Object;
	if(nullptr != ConstructorStatics.InputActionAimRef.Object) AimAction = ConstructorStatics.InputActionAimRef.Object;
	if(nullptr != ConstructorStatics.InputActionLookRef.Object) LookAction = ConstructorStatics.InputActionLookRef.Object;
	if(nullptr != ConstructorStatics.InputActionMoveRef.Object) MoveAction = ConstructorStatics.InputActionMoveRef.Object;
	if(nullptr != ConstructorStatics.InputActionCrouchRef.Object) CrouchAction = ConstructorStatics.InputActionCrouchRef.Object;
	if(nullptr != ConstructorStatics.InputActionSneakRef.Object) SneakAction = ConstructorStatics.InputActionSneakRef.Object;
	if(nullptr != ConstructorStatics.InputActionSprintRef.Object) SprintAction = ConstructorStatics.InputActionSprintRef.Object;
	if(nullptr != ConstructorStatics.InputActionJumpRef.Object) JumpAction = ConstructorStatics.InputActionJumpRef.Object;
	if(nullptr != ConstructorStatics.InputActionProneRef.Object) ProneAction = ConstructorStatics.InputActionProneRef.Object;
	if(nullptr != ConstructorStatics.InputActionMeleeRef.Object) MeleeAction = ConstructorStatics.InputActionMeleeRef.Object;
	if(nullptr != ConstructorStatics.InputActionChangeFireModeRef.Object) ChangeFireModeAction = ConstructorStatics.InputActionChangeFireModeRef.Object;
	if(nullptr != ConstructorStatics.InputActionLeaningRef.Object) LeaningAction = ConstructorStatics.InputActionLeaningRef.Object;

	//	Capsule Section
	GetCapsuleComponent()->InitCapsuleSize(42.f, 86.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	
	//	Camera Section
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0.f,0.f,80.f));
	Camera->bUsePawnControlRotation = true;
	
	// Arm Mesh Section
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arm"));
	ArmMesh->SetOnlyOwnerSee(true);
	ArmMesh->SetOwnerNoSee(false);
	ArmMesh->SetupAttachment(Camera);
	ArmMesh->bCastDynamicShadow = false;
	ArmMesh->CastShadow = false;
	ArmMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	ArmMesh->SetRelativeLocation(FVector(0.f,0.f,-160.f));
	ArmMesh->SetRelativeRotation(FRotator(0.f,-90.f,0.f));

	//	Character Mesh Section
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -86.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetMesh()->SetOnlyOwnerSee(false);
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	//	Pawn Section
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//	Movement Section
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 45.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	
}

void ATACCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
	SetCharacterControl();
}

void ATACCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ATACCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Fire);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Aim);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Look);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Move);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Crouch);
	EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Prone);
	EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Sneak);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Sprint);
	EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Melee);
	EnhancedInputComponent->BindAction(LeaningAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Leaning);
	EnhancedInputComponent->BindAction(ChangeFireModeAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ChangeFireMode);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
}

void ATACCharacterPlayer::SetCharacterControl()
{
	if (!IsLocallyControlled())
	{
		return;
	}
	
	SetCharacterControlData(CurrentControlData);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = CurrentControlData->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}
}

void ATACCharacterPlayer::SetCharacterControlData(const UTACControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);
}


void ATACCharacterPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void ATACCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ATACCharacterPlayer::Fire(const FInputActionValue& Value)
{
	
}

void ATACCharacterPlayer::Aim(const FInputActionValue& Value)
{
	
}

void ATACCharacterPlayer::Sprint(const FInputActionValue& Value)
{
	
}

void ATACCharacterPlayer::Sneak(const FInputActionValue& Value)
{
	
}

void ATACCharacterPlayer::Crouch()
{
	
}

void ATACCharacterPlayer::Prone()
{
	
}

void ATACCharacterPlayer::Leaning(const FInputActionValue& Value)
{
	
}


void ATACCharacterPlayer::Melee()
{
	
}

void ATACCharacterPlayer::ChangeFireMode()
{
	
}
