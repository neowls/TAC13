// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TACScoreBoardWidget.h"
#include "TACScoreWidget.h"
#include "Components/VerticalBox.h"
#include "Game/Play/TACPlayPlayerState.h"

UTACScoreBoardWidget::UTACScoreBoardWidget(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UTACUserWidget> ScoreWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/_TAC/UI/WBP_Score.WBP_Score_C'"));
	if(ScoreWidgetRef.Class)
	{
		ScoreWidgetClass = ScoreWidgetRef.Class;
	}
}

void UTACScoreBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	for(int i = 0; i < WidgetCount; i++)
	{
		UTACScoreWidget* InWidget = Cast<UTACScoreWidget>(CreateWidget(this, ScoreWidgetClass));
		ScoreVerticalBox->AddChildToVerticalBox(InWidget);
	}
}

void UTACScoreBoardWidget::ClearScoreBoard()
{
	for(auto it : ScoreVerticalBox->GetAllChildren())
	{
		Cast<class UTACScoreWidget>(it)->ReleasePlayerScorePtr();
	}
}

void UTACScoreBoardWidget::UpdateScoreBoard()
{
	int ScoreIndex = 0;
	for(const auto iter : CurrentPlayerList)
	{
		if(iter != nullptr)
		{
			Cast<class UTACScoreWidget>(ScoreVerticalBox->GetChildAt(ScoreIndex))->SetPlayerScoreInfo(iter->GetPlayerScore());
		}
		ScoreIndex++;
	}
}
