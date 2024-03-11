// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/TACCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OnlineSubsystem.h"
#include "TAC13.h"
#include "TACCharacterMovementComponent.h"
#include "TACCharacterStatComponent.h"
#include "Animation/TACAnimInstance.h"
#include "Animation/TACArmAnimInstance.h"
#include "Animation/TACBodyAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Engine/DamageEvents.h"
#include "Game/TACGameMode.h"
#include "Game/TACPlayerState.h"
#include "GameData/NetObject.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/TACHUDWidget.h"
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
		CONSTRUCT_IA InputActionReloadRef;
		CONSTRUCT_IA InputActionScoreboardRef;
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
			, InputActionReloadRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Reload.IA_Reload'"))
			, InputActionScoreboardRef(TEXT("/Script/EnhancedInput.InputAction'/Game/_TAC/Input/Actions/IA_Scoreboard.IA_Scoreboard'"))
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
	if(nullptr != ConstructorStatics.InputActionReloadRef.Object) ReloadAction = ConstructorStatics.InputActionReloadRef.Object;
	if(nullptr != ConstructorStatics.InputActionScoreboardRef.Object) ScoreboardAction = ConstructorStatics.InputActionScoreboardRef.Object;

	
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
	
	FireMontage = FireCosmeticMontage;

	
	//	Pawn Section
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	bCanFire = true;

	bReplicateUsingRegisteredSubObjectList = true;
}

void ATACCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
	}
	
	if(IsLocallyControlled())
	{
		FireMontage = FireArmMontage;
	}
	ArmAnimInstance = Cast<UTACArmAnimInstance>(ArmMesh->GetAnimInstance());
	BodyAnimInstance = Cast<UTACBodyAnimInstance>(GetMesh()->GetAnimInstance());
	SetCharacterControl(PlayControlData);
	if(HasAuthority())
	{
		SomeNetObject = NewObject<UNetObject>();
		AddReplicatedSubObject(SomeNetObject);
		SpawnWeapon("VAL");
		SpawnWeapon("KA47");
		SpawnWeapon("AR4");
		EquipWeapon(0);
	}

	
}

void ATACCharacterPlayer::Destroyed()
{
	Super::Destroyed();
	if(CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		for(auto iter : OwnWeapons) iter->Destroy();		
	}
	RemoveReplicatedSubObject(SomeNetObject);
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
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ReloadingWeapon);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ShowScoreboard);
}

void ATACCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ATACCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATACCharacterPlayer, bCanFire);
	DOREPLIFETIME_CONDITION(ATACCharacterPlayer, CurrentWeapon, COND_None);
	DOREPLIFETIME_CONDITION(ATACCharacterPlayer, OwnWeapons, COND_None);
	DOREPLIFETIME(ATACCharacterPlayer, SomeNetObject);
}

void ATACCharacterPlayer::GetOnlineSubsystem()
{
	// Receive OnlineSubsystem
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		// Receive OnlineSubsystem Interface
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Orange,
			FString::Printf(TEXT("Subsystem Name : %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}

}

#pragma region HIT

float ATACCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return ActualDamage;
}

void ATACCharacterPlayer::SetDead()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::SetDead();
	
	if(HasAuthority())
	Cast<ATACGameMode>(GetWorld()->GetAuthGameMode())->AddPlayerScore(RecentAttacker, Cast<ATACPlayerState>(Controller->PlayerState));
	
	SetCharacterControl(SpectateControlData);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("DeadPawn"));
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetOwnerNoSee(false);
	ArmMesh->SetOwnerNoSee(true);
	CurrentWeapon->SetActorHiddenInGame(true);
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ATACCharacterPlayer::RespawnCharacter, RespawnDelayTime, false);
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("End"));
}

void ATACCharacterPlayer::RespawnCharacter()
{
	Super::RespawnCharacter();
	//SetActorLocation({}) 리스폰 위치 지정
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetMesh()->AttachToComponent(GetCapsuleComponent(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	if(HasAuthority())
	{
		Stat->SetHP(100);
		for(auto iter : OwnWeapons)
		{
			iter->ResetWeaponData();
		}
	}
	GetMesh()->SetOwnerNoSee(true);
	ArmMesh->SetOwnerNoSee(false);
	CurrentWeapon->SetActorHiddenInGame(false);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	SetCharacterControl(PlayControlData);
}


void ATACCharacterPlayer::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	const AActor* HitActor = HitResult.GetActor();
	if(::IsValid(HitActor))
	{
		const float DotValue = GetDotProductTo(HitActor);
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

void ATACCharacterPlayer::SetCharacterControl(TObjectPtr<UTACControlData> InCharacterControlData)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	CurrentControlData = InCharacterControlData;
	
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
	if(bCanFire && IsCanFire())
	{
		if(!HasAuthority())
		{
			bCanFire = false;
			GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATACCharacterPlayer::ResetFire, FireTime, false);
			PlayFireAnimation();
		}
		ServerRPCFire(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		SetRecoilPoint();
	}
}

void ATACCharacterPlayer::OnRep_CanFire()
{
	
}

void ATACCharacterPlayer::ResetFire()
{
	bCanFire = true;
}

bool ATACCharacterPlayer::IsCanFire()
{
	if(bIsSprinting || CurrentWeapon->GetCurrentAmmo() <= 0) return false;
	return true;
}


void ATACCharacterPlayer::FireHitCheck()
{
	if(IsLocallyControlled())
	{
		FHitResult OutHitResult;
		const FVector SpreadVector = bIsADS ? FVector::Zero() : FMath::VRand() * 0.05f;
		const FVector Start = FirstCamera->GetComponentLocation();
		const FVector End = Start + (FirstCamera->GetForwardVector() + SpreadVector) * 10000.0f;
		const TArray<AActor*> ActorsToIgnore;
		const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility),
			true, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHitResult, true, FLinearColor::Yellow, FLinearColor::Green, 2.0f);
		const float HitCheckTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		if(bHit)
		{
			if(!HasAuthority())
			{
				ServerRPCNotifyHit(OutHitResult, HitCheckTime);
			}
			else
			{
				FireHitConfirm(OutHitResult);
			}
		}
	}
}

void ATACCharacterPlayer::SetRecoilPoint()
{
	//Camera->AddLocalRotation(FRotator(0.5f,0.f,0.f));
	//AddControllerPitchInput(0.1f);
	// FRotator temp = GetControlRotation();
	// temp.Pitch += 1.0f;
	// Controller->SetControlRotation(temp);
	// LOG_SCREEN(5, TEXT("Controller Yaw : %f"), Controller->GetControlRotation().Yaw);
	// LOG_SCREEN(6, TEXT("Controller Pitch : %f"), Controller->GetControlRotation().Pitch);
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
		HitResult.GetActor()->TakeDamage(FireDamage, DamageEvent, GetController(), CurrentWeapon);
	}
}

void ATACCharacterPlayer::PlayFireAnimation()
{
	if(IsLocallyControlled())
	{
		ArmAnimInstance->StopAllMontages(0.0f);
		ArmAnimInstance->Montage_Play(FireArmMontage);
		CurrentWeapon->ConsumingAmmo();
		OnCurrentAmmoChanged.Broadcast(CurrentWeapon->GetCurrentAmmo());
	}
	else
	{
		BodyAnimInstance->StopAllMontages(0.0f);
		BodyAnimInstance->Montage_Play(FireMontage);
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
	if(IsLocallyControlled())
	TargetWeapon->AttachToComponent(ArmMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));
	else
	{
		TargetWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Grip"));
	}
}

void ATACCharacterPlayer::DropWeapon()
{
	OwnWeapons.Remove(CurrentWeapon);
	CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	CurrentWeapon->GetMesh()->SetSimulatePhysics(true);
	CurrentWeapon->GetMesh()->SetCollisionProfileName("Item");
	EquipWeapon(0);
}

void ATACCharacterPlayer::ReloadingWeapon()
{
	CurrentWeapon->ReloadingAmmo();
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


void ATACCharacterPlayer::ServerRPCChangeSomeVal_Implementation()
{
	SomeNetObject->SomeVal++;
	TAC_LOG(LogTACNetwork, Log, TEXT("%d"), SomeNetObject->SomeVal);
}

void ATACCharacterPlayer::OnRep_CurrentWeapon()
{
	CurrentWeapon->SetActorHiddenInGame(false);
	OnCurrentAmmoChanged.Broadcast(CurrentWeapon->GetCurrentAmmo());
	OnOwnAmmoChanged.Broadcast(CurrentWeapon->GetOwnAmmo());
	OnWeaponNameChanged.Broadcast(CurrentWeapon->GetWeaponName());
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
	FireTime = 60 / CurrentWeapon->GetWeaponStat().FireRate;
	OnRep_CurrentWeapon();
	ArmAnimInstance->SetNewSight();
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
	
}
void ATACCharacterPlayer::ChangeFireMode()
{
	CurrentWeapon->ChangeFireMode();
	
}

#pragma endregion

#pragma region INPUT


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
	if(SomeNetObject != nullptr)
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("Replicated"));
		ServerRPCChangeSomeVal();
		TAC_LOG(LogTACNetwork, Log, TEXT("%d"), SomeNetObject->SomeVal);
	}
	else
	{
		TAC_LOG(LogTACNetwork, Log, TEXT("Not Replicated"));	
	}
}

#pragma endregion 

#pragma region WIDGET

void ATACCharacterPlayer::SetupHUDWidget(UTACHUDWidget* InHUDWidget)
{
	if(InHUDWidget)
	{
		InHUDWidget->UpdateHPBar(Stat->GetCurrentHP());
		Stat->OnHPChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateHPBar);
		InHUDWidget->UpdateCurrentAmmo(0);
		InHUDWidget->UpdateOwnAmmo(0);
		InHUDWidget->UpdateWeaponName(TEXT("--"));
		OnCurrentAmmoChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateCurrentAmmo);
		OnOwnAmmoChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateOwnAmmo);
		OnWeaponNameChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateWeaponName);
		OnScoreboardChanged.AddUObject(InHUDWidget, &UTACHUDWidget::ScoreBoardOnOff);
	}
}

void ATACCharacterPlayer::ShowScoreboard(const FInputActionValue& Value)
{
	const uint8 bScoreboardInput = Value.Get<bool>();
	TAC_LOG(LogTACNetwork, Log, TEXT("Scoreboard Input : %d"), bScoreboardInput);
	OnScoreboardChanged.Broadcast(bScoreboardInput);
}

#pragma endregion