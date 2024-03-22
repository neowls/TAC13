// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/TACCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TAC13.h"
#include "TACCharacterMovementComponent.h"
#include "TACCharacterStatComponent.h"
#include "Animation/TACAnimInstance.h"
#include "Animation/TACArmAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Game/TACPlayerState.h"
#include "Game/TACPlayGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Input/TACControlData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/TACHUDWidget.h"
#include "Weapon/TACWeapon.h"


ATACCharacterPlayer::ATACCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
		// Input
	InitializeObjectFinder(FireAction,TEXT("/Game/_TAC/Input/Actions/IA_Fire.IA_Fire"));
	InitializeObjectFinder(LookAction,TEXT("/Game/_TAC/Input/Actions/IA_Look.IA_Look"));                    
	InitializeObjectFinder(MoveAction,TEXT("/Game/_TAC/Input/Actions/IA_Move.IA_Move"));        
	InitializeObjectFinder(CrouchAction,TEXT("/Game/_TAC/Input/Actions/IA_Crouch.IA_Crouch"));
	InitializeObjectFinder(SneakAction,TEXT("/Game/_TAC/Input/Actions/IA_Sneak.IA_Sneak"));
	InitializeObjectFinder(SprintAction,TEXT("/Game/_TAC/Input/Actions/IA_Sprint.IA_Sprint"));
	InitializeObjectFinder(JumpAction,TEXT("/Game/_TAC/Input/Actions/IA_Jump.IA_Jump"));
	InitializeObjectFinder(ProneAction,TEXT("/Game/_TAC/Input/Actions/IA_Prone.IA_Prone"));
	InitializeObjectFinder(MeleeAction,TEXT("/Game/_TAC/Input/Actions/IA_Melee.IA_Melee"));
	InitializeObjectFinder(ChangeFireModeAction,TEXT("/Game/_TAC/Input/Actions/IA_ChangeFireMode.IA_ChangeFireMode"));
	InitializeObjectFinder(ChangeWeaponAction,TEXT("/Game/_TAC/Input/Actions/IA_ChangeWeapon.IA_ChangeWeapon"));
	InitializeObjectFinder(LeaningAction,TEXT("/Game/_TAC/Input/Actions/IA_Leaning.IA_Leaning"));              
	InitializeObjectFinder(ReloadAction,TEXT("/Game/_TAC/Input/Actions/IA_Reload.IA_Reload"));
	InitializeObjectFinder(ScoreboardAction,TEXT("/Game/_TAC/Input/Actions/IA_Scoreboard.IA_Scoreboard"));        
	InitializeObjectFinder(AimAction,TEXT("/Game/_TAC/Input/Actions/IA_Aim.IA_Aim"));
	
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

}


void ATACCharacterPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ArmAnimInstance = Cast<UTACArmAnimInstance>(ArmMesh->GetAnimInstance());
}


void ATACCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	if(IsLocallyControlled())
	{
		FireMontage = FireArmMontage;
		ChangeWeaponMontage = ChangeWeaponArmMontage.Get();
	}
	SetCharacterControl(PlayControlData);
	if(HasAuthority())
	{
		SpawnWeapon("VAL");
		SpawnWeapon("KA47");
		SpawnWeapon("AR4");
		ApplyWeaponChange(0);
	}
}

void ATACCharacterPlayer::Destroyed()
{
	Super::Destroyed();
	if(CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		for(const auto iter : OwnWeapons) iter->Destroy();		
	}
}

void ATACCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
	}
	
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
	EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::TryChangeWeapon);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ReloadingWeapon);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Triggered, this, &ATACCharacterPlayer::ShowScoreboard);
}
void ATACCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

#pragma region HIT

void ATACCharacterPlayer::SetDead()
{
	TAC_LOG(LogTACNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::SetDead();
	if(HasAuthority())
	Cast<ATACPlayGameMode>(GetWorld()->GetAuthGameMode())->AddPlayerScore(RecentAttacker, Cast<ATACPlayPlayerState>(Controller->PlayerState));
	
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
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetMesh()->AttachToComponent(GetCapsuleComponent(),FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	if(HasAuthority())
	{
		Stat->SetHP(100);
		for(const auto iter : OwnWeapons)
		{
			iter->InitWeaponStat();
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
			HitConfirm(HitResult);
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
		if (const UInputMappingContext* NewMappingContext = CurrentControlData->InputMappingContext)
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
			PlayMontageAnimation(FireMontage);
		}
		ServerRPCFire(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		SetRecoilPoint();
	}
}

void ATACCharacterPlayer::ResetFire()
{
	bCanFire = true;
}

bool ATACCharacterPlayer::IsCanFire() const
{
	if(bIsSprinting || CurrentWeapon->GetWeaponStat().CurrentAmmo <= 0) return false;
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
				HitConfirm(OutHitResult);
			}
		}
	}
}

void ATACCharacterPlayer::SetRecoilPoint()
{
	// Camera->AddLocalRotation(FRotator(0.5f,0.f,0.f));
	// AddControllerPitchInput(0.1f);
	// FRotator temp = GetControlRotation();
	// temp.Pitch += 1.0f;
	// Controller->SetControlRotation(temp);
	// LOG_SCREEN(5, TEXT("Controller Yaw : %f"), Controller->GetControlRotation().Yaw);
	// LOG_SCREEN(6, TEXT("Controller Pitch : %f"), Controller->GetControlRotation().Pitch);
}


void ATACCharacterPlayer::ServerRPCFire_Implementation(float FireStartTime)
{
	bCanFire = false;
	CurrentWeapon->UseAmmo();

	FireTimeDifference = GetWorld()->GetTimeSeconds() - FireStartTime;
	FireTimeDifference = FMath::Clamp(FireTimeDifference, 0.0f, FireTime - 0.01f);

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATACCharacterPlayer::ResetFire, FireTime - FireTimeDifference, false);

	LastFireStartTime = FireStartTime;
	PlayMontageAnimation(FireMontage);
	MulticastRPCFire();
}

bool ATACCharacterPlayer::ServerRPCFire_Validate(float FireStartTime)
{
	return true;
}

void ATACCharacterPlayer::MulticastRPCFire_Implementation()
{
	if(!IsLocallyControlled() && !HasAuthority())
	{
		PlayMontageAnimation(FireMontage);
	}
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
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

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

void ATACCharacterPlayer::TryChangeWeapon(const FInputActionValue& Value)
{
	const int8 ChangeValue = Value.Get<float>();
	ChangeWeapon(ChangeValue);
}

void ATACCharacterPlayer::ChangeFireMode()
{
	Super::ChangeFireMode();
}

#pragma endregion 

#pragma region WIDGET

void ATACCharacterPlayer::SetupHUDWidget(UTACHUDWidget* InHUDWidget)
{
	if(InHUDWidget)
	{
		InHUDWidget->UpdateHPBar(Stat->GetCurrentHP());
		InHUDWidget->UpdateCurrentAmmo(0);
		InHUDWidget->UpdateOwnAmmo(0);
		InHUDWidget->UpdateWeaponName(TEXT("--"));
		Stat->OnHPChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateHPBar);
		OnCurrentAmmoChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateCurrentAmmo);
		OnOwnAmmoChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateOwnAmmo);
		OnWeaponNameChanged.AddUObject(InHUDWidget, &UTACHUDWidget::UpdateWeaponName);
		OnScoreboardChanged.AddUObject(InHUDWidget, &UTACHUDWidget::ScoreBoardOnOff);
	}
}

void ATACCharacterPlayer::ShowScoreboard(const FInputActionValue& Value)
{
	const uint8 bScoreboardInput = Value.Get<bool>();
	OnScoreboardChanged.Broadcast(bScoreboardInput);
}

#pragma endregion