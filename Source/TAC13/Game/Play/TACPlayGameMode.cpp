// Fill out your copyright notice in the Description page of Project Settings.


#include "TACPlayGameMode.h"
#include "TAC13.h"
#include "TACPlayGameState.h"
#include "TACPlayPlayerState.h"
#include "Character/TACCharacterPlayer.h"
#include "GameData/TACGameSingleton.h"
#include "Player/TACPlayPlayerController.h"

ATACPlayGameMode::ATACPlayGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/_TAC/Character/BP_TACCharacterPlayer.BP_TACCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	
	PlayerControllerClass = ATACPlayPlayerController::StaticClass();
	GameStateClass = ATACPlayGameState::StaticClass();
	PlayerStateClass = ATACPlayPlayerState::StaticClass();
}


void ATACPlayGameMode::RespawnPlayer(AController* PlayerController)
{
	if (PlayerController)
    {
        // 랜덤 PlayerStart 위치 가져오기
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

        if (PlayerStarts.Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
            AActor* ChosenPlayerStart = PlayerStarts[RandomIndex];

            // 스폰 파라미터 설정
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = PlayerController;
            SpawnParams.Instigator = PlayerController->GetInstigator();
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            // 새로운 캐릭터 스폰
            ATACCharacterPlayer* NewPawn = GetWorld()->SpawnActor<ATACCharacterPlayer>(DefaultPawnClass, ChosenPlayerStart->GetActorLocation(), ChosenPlayerStart->GetActorRotation(), SpawnParams);
            if (NewPawn)
            {
                // 소유권 설정
                PlayerController->Possess(NewPawn);

                // 클라이언트에서 소유권이 올바르게 설정되었는지 확인
                NewPawn->SetOwner(PlayerController);

                TAC_LOG(LogTACNetwork, Warning, TEXT("%s respawned at location: %s"), *PlayerController->GetName(), *NewPawn->GetActorLocation().ToString());

                // 네트워크 소유권 확인 로그 추가
                if (NewPawn->HasAuthority())
                {
                    UE_LOG(LogTACNetwork, Warning, TEXT("NewPawn is controlled by server (Authority)"));
	                if (NewPawn->IsLocallyControlled())
	                {
                		UE_LOG(LogTACNetwork, Warning, TEXT("NewPawn is locally controlled"));
	                }
	                else
	                {
                		UE_LOG(LogTACNetwork, Warning, TEXT("NewPawn is a simulated proxy"));
	                }
                }
            }
        }
    }
}

void ATACPlayGameMode::StartPlay()
{
	Super::StartPlay();
	
}

void ATACPlayGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	TACPlayGameState = Cast<ATACPlayGameState>(GameState); // 게임 스테이트를 사용자 정의 게임 스테이트로 지정
	TACPlayGameState->AuthTACPlayGameMode = this; // 게임 스테이트 내에 게임모드를 사용자 정의 게임모드로 지정
}


void ATACPlayGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}


void ATACPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ATACPlayPlayerState* NewPlayerState = Cast<ATACPlayPlayerState>(NewPlayer->PlayerState);

	if(TACPlayGameState && NewPlayerState)
	{
		TACPlayGameState->TACPlayerList.Add(NewPlayerState);
		TACPlayGameState->OnRep_PlayerList();
		TAC_LOG(LogTACNetwork, Log, TEXT("Is PlayerList Count : %d"), TACPlayGameState->TACPlayerList.Num());
	}
}

AActor* ATACPlayGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	if (PlayerStarts.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
		return PlayerStarts[RandomIndex];
	}
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ATACPlayGameMode::AddPlayerScore(ATACPlayPlayerState* KillPlayerState, ATACPlayPlayerState* DeathPlayerState)
{
	KillPlayerState->AddKillScore();
	DeathPlayerState->AddDeathScore();
}
