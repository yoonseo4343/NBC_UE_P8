#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpartaCharacter.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    LevelDuration = 40.0f;
    CurrentLevelIndex = 0;
    MaxLevel = 3;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}
int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (UGameInstance* GameInstance = GetGameInstance()) 
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            SpartaGameInstance->AddToScore(Amount);
        }
    }
}

void ASpartaGameState::StartLevel()
{
    // 레벨 시작, 웨이브 3개로 나눔
    if(APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if(ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex=SpartaGameInstance->CurrentLevelIndex;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Level %d Start!"),CurrentLevelIndex + 1);

    StartWave1();
}

void ASpartaGameState::OnLevelTimeUp()
{
    EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;
    /*UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount, SpawnedCoinCount);
    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        EndLevel();
    }*/
}

void ASpartaGameState::EndLevel()
{
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);
    
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            CurrentLevelIndex++;
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
        }
    }
    if (CurrentLevelIndex >= MaxLevel)
    {
        OnGameOver();
        return;
    }
    if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
    {
        UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
    }
    else
    {
        OnGameOver();
    }
}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                // time
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                }
                // score
                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if(UGameInstance* GameInstance = GetGameInstance())
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance)
                        {
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                        }
                    }
                }
                // level
                if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                {
                    LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Stage %d"), CurrentLevelIndex+1)));
                }
            }
        }
    }
}

void ASpartaGameState::StartWave1()
{
    // 20s
    UE_LOG(LogTemp, Warning, TEXT("Start Wave1"));

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemToSpawn = 40;

    for (int32 i = 0; i < ItemToSpawn; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                /*if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }*/
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &ASpartaGameState::StartWave2,
        10.0f,
        false
    );
}
void ASpartaGameState::StartWave2()
{
    // 20s
    UE_LOG(LogTemp, Warning, TEXT("Start Wave1"));

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemToSpawn = 40;

    for (int32 i = 0; i < ItemToSpawn; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
            }
        }
    }
    RollingCamera();

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &ASpartaGameState::StartWave3,
        10.0f,
        false
    );
}
void ASpartaGameState::StartWave3()
{
    // 30s
    UE_LOG(LogTemp, Warning, TEXT("Start Wave3"));
    
    RollingCamera();

    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemToSpawn = 100;

    for (int32 i = 0; i < ItemToSpawn; i++)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnMine(2.0f-(0.5* CurrentLevelIndex));
            }
        }
    }

    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &ASpartaGameState::OnLevelTimeUp,
        10.0f,
        false
    );
}

void ASpartaGameState::RollingCamera()
{
    // 카메라 회전
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (APawn* Pawn = PC->GetPawn())
        {
            if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(Pawn))
            {
                Character->RollCamera();
            }
        }
    }
}

void ASpartaGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true);
            SpartaPlayerController->ShowMenu(true);
        }
    }
}