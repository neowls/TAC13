// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TACCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TAC13.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetSystemLibrary.h"

#define CONSTRUCT_IA ConstructorHelpers::FObjectFinder<UInputAction>

ATACCharacterPlayer::ATACCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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
	
	// Arm Mesh Section
	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arm"));
	ArmMesh->SetOnlyOwnerSee(true);
	ArmMesh->SetOwnerNoSee(false);
	ArmMesh->SetupAttachment(GetCameraComponent());
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

	bCanFire = true;
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
	EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::TryProne);
	EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Sneak);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Sprint);
	EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Melee);
	EnhancedInputComponent->BindAction(LeaningAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Leaning);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::TryCrouch);
	EnhancedInputComponent->BindAction(ChangeFireModeAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ChangeFireMode);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
}

void ATACCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATACCharacterPlayer, bCanFire);
}

void ATACCharacterPlayer::FireHitCheck()
{
	if(IsLocallyControlled())
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
		FHitResult OutHitResult;
		const FVector Start = Camera->GetComponentLocation();
		const FVector End = Start + Camera->GetForwardVector() * 10000.0f;
		const TArray<AActor*> ActorsToIgnore;
		
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility),
			true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true, FLinearColor::Yellow, FLinearColor::Green, 2.0f);
		float HitCheckTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		if(!HasAuthority())
		{
			if(bHit)
			{
				ServerRPCNotifyHit(OutHitResult, HitCheckTime);
			}
			else
			{
				//NotifyMiss
			}
		}
		else
		{
			if(bHit)
			{
				TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Hit"));
				GEngine->AddOnScreenDebugMessage(5, 2.0f, FColor::Yellow, FString::Printf(TEXT("Trace Hit : %s"), *OutHitResult.GetActor()->GetName()));
				GEngine->AddOnScreenDebugMessage(6, 2.0f, FColor::Red, FString::Printf(TEXT("Hit BoneName : %s"), *OutHitResult.BoneName.ToString()));
				FireHitConfirm(OutHitResult.GetActor());
			}
		}
		TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
	}
}

void ATACCharacterPlayer::FireHitConfirm(AActor* HitActor)
{
	if(HasAuthority())
	{
		const float FireDamage = 0.f;
		FDamageEvent DamageEvent;
		HitActor->TakeDamage(FireDamage, DamageEvent, GetController(), this);
	}
}

float ATACCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Player Damage Taken"));
	return ActualDamage;
}

void ATACCharacterPlayer::PlayFireAnimation()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	UAnimInstance* MeshAnimInstance = GetMesh()->GetAnimInstance();
	
	MeshAnimInstance->StopAllMontages(0.0f);
	MeshAnimInstance->Montage_Play(FireCosmeticMontage);

	ArmMesh->GetAnimInstance()->StopAllMontages(0.0f);
	ArmMesh->GetAnimInstance()->Montage_Play(FireArmMontage);
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ATACCharacterPlayer::ServerRPCFire_Implementation(float FireStartTime)
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	bCanFire = false;

	FireTimeDifference = GetWorld()->GetTimeSeconds() - FireStartTime;
	FireTimeDifference = FMath::Clamp(FireTimeDifference, 0.0f, FireTime - 0.01f);

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATACCharacterPlayer::ResetFire, FireTime - FireTimeDifference, false);

	LastFireStartTime = FireStartTime;
	PlayFireAnimation();

	MulticastRPCFire();
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
}

bool ATACCharacterPlayer::ServerRPCFire_Validate(float FireStartTime)
{
	return true;
}

void ATACCharacterPlayer::MulticastRPCFire_Implementation()
{
	if(!IsLocallyControlled())
	{
		PlayFireAnimation();
	}
}

void ATACCharacterPlayer::ClientRPCPlayerAnimation_Implementation(ATACCharacterPlayer* CharacterToPlay)
{
	
}

void ATACCharacterPlayer::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	AActor* HitActor = HitResult.GetActor();
	if(::IsValid(HitActor))
	{
		const float DotValue = GetDotProductTo(HitActor);
		TAC_LOG(LogTACNetwork, Log, TEXT("%f"), DotValue);
		if(DotValue > 0.0f)
		{
			FireHitConfirm(HitActor);
		}
		//TAC_LOG(LogTACNetwork, Warning, TEXT("%s"), TEXT("Hit Test Failed"));
	}
}

bool ATACCharacterPlayer::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	return (HitCheckTime - LastFireStartTime) < AcceptMinCheckTime;
}

void ATACCharacterPlayer::OnRep_CanFire()
{
	
}


void ATACCharacterPlayer::ResetFire()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	bCanFire = true;
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
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
	if(bCanFire)
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
		if(!HasAuthority())
		{
			bCanFire = false;
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATACCharacterPlayer::ResetFire, FireTime, false);
			PlayFireAnimation();
		}
		ServerRPCFire(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
	}
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

void ATACCharacterPlayer::TryCrouch()
{
	if(bIsCrouched) UnCrouch(false);
	else Crouch();
}

void ATACCharacterPlayer::TryProne()
{
	if(bIsProned) UnProne();
	else Prone();
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