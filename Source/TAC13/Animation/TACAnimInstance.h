// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TACAnimInstance.generated.h"


UENUM(BlueprintType)
enum class EAnim_CardinalDirection : uint8
{
	Forward,
	Backward,
	Left,
	Right
};

UENUM(BlueprintType)
enum class EAnim_RootYawOffsetMode : uint8
{
	BlendOut,
	Hold,
	Accumulate
};


UCLASS()
class TAC13_API UTACAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UTACAnimInstance();
	FORCEINLINE class ATACCharacterBase* GetOwner() const { return Owner; }
	
protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = TurnInPlace)
	void SetRootYawOffset(float InRootYawOffset);

	UFUNCTION(BlueprintCallable, Category = TurnInPlace, meta = (BlueprintThreadSafe))
	void ProcessTurnYawCurve();

	UFUNCTION(BlueprintCallable, Category = TurnInPlace)
	void UpdateRootYawOffset(float DeltaTime);
	

	UFUNCTION(BlueprintPure, Category = Helper)
	EAnim_CardinalDirection SelectCardinalDirectionFromAngle(float Angle, float DeadZone, EAnim_CardinalDirection CurrentDirection, bool UseCurrentDirection);

	UFUNCTION(BlueprintPure, Category = Helper)
	EAnim_CardinalDirection GetOppositeCardinalDirection(EAnim_CardinalDirection CurrentDirection);
 
	UFUNCTION(BlueprintPure, Category = Helper, meta = (BlueprintThreadSafe))
	bool IsMovingPerpendicularToInitialPivot();

	UFUNCTION(BlueprintPure)
	bool ShouldEnableControlRig();

	UFUNCTION(BlueprintPure)
	class UCharacterMovementComponent* GetCharacterMovementComponent();
	
	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateVelocityData();

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateAccelerationData();
	
	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateCharacterStateData();

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateAimingData();

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateLocationData(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateRotationData();

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateJumpFallData(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateWallDetectionHeuristic();

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateBlendWeightData(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Update)
	void UpdateGroundData();
	
// Value Section
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class ATACCharacterBase> Owner;

//	Rotation Data
	UPROPERTY(BlueprintReadOnly, Category = Rotation)
	FRotator WorldRotation;

	UPROPERTY(BlueprintReadOnly, Category = Rotation)
	float YawDeltaSinceLastUpdate;

	UPROPERTY(BlueprintReadOnly, Category = Rotation)
	float YawDeltaSpeed;

	
//	Location Data	
	UPROPERTY(BlueprintReadOnly, Category = Location)
	FVector WorldLocation;
	
	UPROPERTY(BlueprintReadOnly, Category = Location)
	float DisplacementSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Location)
	float DisplacementSinceLastUpdate;

	UPROPERTY(BlueprintReadOnly, Category = Location)
	float GroundDistance = -1.0f;

	UPROPERTY(BlueprintReadOnly, Category = Location)
	float TimeFalling;

//	Velocity Data
	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	FVector WorldVelocity;

	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	FVector LocalVelocity2D;

	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	uint8 bHasVelocity : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	float LocalVelocityDirectionAngle;

	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	float LocalVelocityDirectionAngleWithOffset;

	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	EAnim_CardinalDirection LocalVelocityDirection;

	UPROPERTY(BlueprintReadWrite, Category = Velocity)
	EAnim_CardinalDirection LocalVelocityDirectionNoOffset;
	
	UPROPERTY(BlueprintReadOnly, Category = Velocity)
	float GroundSpeed;
	
	
//	Acceleration Data
	UPROPERTY(BlueprintReadOnly, Category = Acceleration)
	FVector LocalAcceleration2D;

	UPROPERTY(BlueprintReadOnly, Category = Acceleration)
	FVector PivotDirection2D;

	UPROPERTY(BlueprintReadOnly, Category = Acceleration)
	uint8 bHasAcceleration : 1;


//	Turn In Place Data
	UPROPERTY(BlueprintReadOnly, Category = TurnInPlace)
	float RootYawOffset;

	UPROPERTY(BlueprintReadWrite, Category = TurnInPlace)
	float TurnYawCurveValue;

	UPROPERTY(BlueprintReadOnly, Category = TurnInPlace)
	FVector2D RootYawOffsetAngleClamp;

	UPROPERTY(BlueprintReadOnly, Category = TurnInPlace)
	FVector2D RootYawOffsetAngleClampCrouched;
	
	UPROPERTY(BlueprintReadWrite, Category = TurnInPlace)
	EAnim_RootYawOffsetMode RootYawOffsetMode;
	
	

//	Aiming Data	
	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	uint8 bIsADS : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	float AimYaw;
	
	UPROPERTY(BlueprintReadOnly, Category = Aiming)
	float AimingAlpha;
	

//	State Data
	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsCrouching : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsSprint : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsJumping : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsFalling : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bADSStateChange : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bWasADSLastUpdate : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bIsOnGround : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	uint8 bCrouchStateChange : 1;

	UPROPERTY(BlueprintReadOnly, Category = State)
	float TimeToJumpApex;


//	Locomotion SM Data
	UPROPERTY(BlueprintReadWrite, Category = Locomotion)
	EAnim_CardinalDirection StartDirection;

	UPROPERTY(BlueprintReadWrite, Category = Locomotion)
	EAnim_CardinalDirection PivotInitialDirection;

	UPROPERTY(BlueprintReadWrite, Category = Locomotion)
	EAnim_CardinalDirection CardinalDirectionFromAcceleration;
	
	UPROPERTY(BlueprintReadWrite, Category = Locomotion)
	float LastPivotTime;
	

//	Linked Layer Data
	UPROPERTY(BlueprintReadOnly, Category = LinkedLayerData)
	uint8 bLinkedLayerChanged : 1;
	
	UPROPERTY(BlueprintReadOnly, Category = LinkedLayerData)
	TObjectPtr<UAnimInstance> LastLinkedLayer;
	
	
//	Wall Detection Data
	UPROPERTY(BlueprintReadOnly, Category = WallDetection)
	uint8 bIsRunningIntoWall : 1;

	UPROPERTY(BlueprintReadOnly, Category = WallDetection)
	uint8 bEnableRootYawOffset : 1;

	
//	Blend Weight Data	
	UPROPERTY(BlueprintReadOnly, Category = BlendWeight)
	float UpperbodyDynamicAdditiveWeight;

	
	UPROPERTY(BlueprintReadOnly)
	uint8 bIsFirstUpdate : 1;

	UPROPERTY(BlueprintReadOnly)
	uint8 bUseFootPlacement : 1;

	UPROPERTY(BlueprintReadOnly)
	uint8 bEnableControlRig : 1;

	UPROPERTY(BlueprintReadWrite, Category = Settings)
	float CardinalDirectionDeadZone = 10.0f;
	

//	Montage	
public:
	UPROPERTY(BlueprintReadOnly, Category = Montage)
	TObjectPtr<UAnimMontage> FireMontage;
	
	UPROPERTY(BlueprintReadOnly, Category = Montage)
	TObjectPtr<UAnimMontage> AimFireMontage;
};
