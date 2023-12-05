// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TACCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TAC13.h"
#include "TACCharacterMovementComponent.h"
#include "Animation/TACAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon/TACWeapon.h"

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
		CONSTRUCT_IA InputActionChangeWeaponRef;
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
			, InputActionChangeWeaponRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_ChangeWeapon.IA_ChangeWeapon'"))
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
	if(nullptr != ConstructorStatics.InputActionChangeWeaponRef.Object) ChangeWeaponAction = ConstructorStatics.InputActionChangeWeaponRef.Object;
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

	RoleMesh = GetMesh();
	
	//	Pawn Section
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	bCanFire = true;
}

void ATACCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
	}
	
	if(IsLocallyControlled()) RoleMesh = ArmMesh;
	
	ArmAnimInstance = Cast<UTACAnimInstance>(GetArmMesh()->GetAnimInstance());
	BodyAnimInstance = Cast<UTACAnimInstance>(GetMesh()->GetAnimInstance());
	SetCharacterControl();
	if(HasAuthority())
	{
		SpawnWeapon("VAL");
		SpawnWeapon("KA47");
		EquipWeapon(0);
	}
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
	EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Sneak);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::TrySprint);
	EnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Melee);
	EnhancedInputComponent->BindAction(LeaningAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::Leaning);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::TryCrouch);
	EnhancedInputComponent->BindAction(ChangeFireModeAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ChangeFireMode);
	EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ChangeWeapon);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
}

void ATACCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATACCharacterPlayer, bCanFire);
	DOREPLIFETIME_CONDITION(ATACCharacterPlayer, CurrentWeapon, COND_None);
	DOREPLIFETIME_CONDITION(ATACCharacterPlayer, OwnWeapons, COND_None);
}

#pragma region HIT

float ATACCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	TAC_LOG(LogTACNetwork, Log, TEXT("Player %f Damage Taken"), DamageAmount);
	return ActualDamage;
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
			FireHitConfirm(HitResult);
		}
	}
}

bool ATACCharacterPlayer::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	return (HitCheckTime - LastFireStartTime) < AcceptMinCheckTime;
}

void ATACCharacterPlayer::SetCharacterControl()
{
	if (!IsLocallyControlled())
	{
		return;
	}
	
	SetCharacterControlData(CurrentControlData);

	const APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		const UInputMappingContext* NewMappingContext = CurrentControlData->InputMappingContext;
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

#pragma endregion 

#pragma region FIRE

void ATACCharacterPlayer::Fire(const FInputActionValue& Value)
{
	if(bCanFire)
	{
		if(!HasAuthority())
		{
			bCanFire = false;
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATACCharacterPlayer::ResetFire, FireTime, false);
			PlayFireAnimation();
		}
		ServerRPCFire(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	}
}

void ATACCharacterPlayer::OnRep_CanFire()
{
	
}

void ATACCharacterPlayer::ResetFire()
{
	bCanFire = true;
}


void ATACCharacterPlayer::FireHitCheck()
{
	if(IsLocallyControlled())
	{
		//TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
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
				FireHitConfirm(OutHitResult);
			}
		}
		//TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
	}
}

void ATACCharacterPlayer::FireHitConfirm(const FHitResult& HitResult)
{
	if(HasAuthority())
	{
		float PartDamageMultiplier;
		if(HitResult.BoneName == TEXT("head")) PartDamageMultiplier = 2.5f;
		else PartDamageMultiplier = 1.0f;
		const float FireDamage = CurrentWeapon->GetWeaponStat().Damage * PartDamageMultiplier;
		FDamageEvent DamageEvent;
		HitResult.GetActor()->TakeDamage(FireDamage, DamageEvent, GetController(), this);
	}
}

void ATACCharacterPlayer::PlayFireAnimation()
{
	BodyAnimInstance->StopAllMontages(0.0f);
	BodyAnimInstance->Montage_Play(FireCosmeticMontage);

	if(IsLocallyControlled())
	{
		ArmMesh->GetAnimInstance()->StopAllMontages(0.0f);
		ArmMesh->GetAnimInstance()->Montage_Play(FireArmMontage);
	}
}

void ATACCharacterPlayer::ServerRPCFire_Implementation(float FireStartTime)
{
	bCanFire = false;

	FireTimeDifference = GetWorld()->GetTimeSeconds() - FireStartTime;
	FireTimeDifference = FMath::Clamp(FireTimeDifference, 0.0f, FireTime - 0.01f);

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATACCharacterPlayer::ResetFire, FireTime - FireTimeDifference, false);

	LastFireStartTime = FireStartTime;
	PlayFireAnimation();

	MulticastRPCFire();
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

#pragma endregion 

#pragma region WEAPON
void ATACCharacterPlayer::SpawnWeapon(const FName WeaponName)
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	ATACWeapon* SpawnedWeapon = GetWorld()->SpawnActor<ATACWeapon>(WeaponToSpawn, Params);
	SpawnedWeapon->LoadWeaponStatData(WeaponName);
	SpawnedWeapon->SetOwner(this);
	SpawnedWeapon->SetActorHiddenInGame(true);
	AttachWeapon(SpawnedWeapon);
	OwnWeapons.Add(SpawnedWeapon);
}

void ATACCharacterPlayer::AttachWeapon(ATACWeapon* TargetWeapon)
{
	TargetWeapon->AttachToComponent(RoleMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));
}


void ATACCharacterPlayer::OnRep_OwnWeapons()
{
	if(OwnWeapons.IsEmpty()) return;
	for(const auto iter : OwnWeapons)
	{
		if(IsValid(iter))
			AttachWeapon(iter);
	}
}


void ATACCharacterPlayer::OnRep_CurrentWeapon()
{
	CurrentWeapon->SetActorHiddenInGame(false);
}


void ATACCharacterPlayer::ChangeWeaponCheck()
{
	const uint8 TargetIndex = IsLocallyControlled() ? LocalWeaponIndex : CurrentWeaponIndex;
	EquipWeapon(TargetIndex);
}


void ATACCharacterPlayer::EquipWeapon(const uint8 Index)
{
	if(!OwnWeapons.IsValidIndex(Index) || CurrentWeapon == OwnWeapons[Index]) return; // Skip InValid Weapon, Same Weapon
	if(CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
	}
	CurrentWeaponIndex = Index;
	CurrentWeapon = OwnWeapons[Index];
	OnRep_CurrentWeapon();
}


void ATACCharacterPlayer::ChangeWeapon(const FInputActionValue& Value)
{
	const int8 ChangeValue = Value.Get<float>();
	const uint8 ChangeIndex = OwnWeapons.IsValidIndex(CurrentWeaponIndex + ChangeValue) ? CurrentWeaponIndex + ChangeValue : (ChangeValue > 0 ? 0 : OwnWeapons.Num() - 1);
	ServerRPCSetCurrentWeapon(ChangeIndex);
	EquipWeapon(ChangeIndex);
}


void ATACCharacterPlayer::ServerRPCSetCurrentWeapon_Implementation(const uint8 Index)
{
	EquipWeapon(Index);
}


void ATACCharacterPlayer::PlayChangeWeaponAnimation()
{
	if(IsLocallyControlled())
	{
		ArmMesh->GetAnimInstance()->StopAllMontages(0.0f);
		ArmMesh->GetAnimInstance()->Montage_Play(ChangeWeaponMontage);
	}
	else
	{
		BodyAnimInstance->StopAllMontages(0.0f);
		BodyAnimInstance->Montage_Play(ChangeWeaponMontage);
	}
}


#pragma endregion 

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

void ATACCharacterPlayer::Aim(const FInputActionValue& Value)
{
	bIsADS = Value.Get<bool>();
	if (TACCharacterMovement)
	{
		if(bIsADS) TACCharacterMovement->bWantsToADS = true;
		else TACCharacterMovement->bWantsToADS = false;
	}
}

void ATACCharacterPlayer::Sneak(const FInputActionValue& Value)
{
	
}

void ATACCharacterPlayer::TryCrouch()
{
	if(bIsSprinting) return;
	if(bIsCrouched) UnCrouch(false);
	else Crouch();
}

void ATACCharacterPlayer::TrySprint(const FInputActionValue& Value)
{
	const bool bSprintInput = Value.Get<bool>();
	if(bSprintInput) Sprint();
	else UnSprint();
}

void ATACCharacterPlayer::Leaning(const FInputActionValue& Value)
{
	
}

void ATACCharacterPlayer::Melee()
{
	
}

void ATACCharacterPlayer::ChangeFireMode()
{
	CurrentWeapon->ChangeFireMode();
}