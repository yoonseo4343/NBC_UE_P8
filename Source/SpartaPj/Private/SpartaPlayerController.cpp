#include "SpartaPlayerController.h"
#include "SpartaGameState.h"
#include "SpartaGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
ASpartaPlayerController::ASpartaPlayerController()
    :InputMappingContext(nullptr),
    MoveAction(nullptr),
    JumpAction(nullptr),
    LookAction(nullptr),
    SprintAction(nullptr),
    HUDWidgetClass(nullptr),
    HUDWidgetInstance(nullptr),
    MenuWidgetClass(nullptr)
{

}

void ASpartaPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer()) // Get the local player
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) // Get the Enhanced Input Subsystem
        {
            if (InputMappingContext)
            {
                Subsystem->AddMappingContext(InputMappingContext, 0); // Add the input mapping context with priority 0
            }
        }
    }
    FString CurrentMapName = GetWorld()->GetMapName();
    if(CurrentMapName.Contains("MenuLevel"))
    {
        ShowMenu(false);
    }
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
    return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMenu(bool bIsRestart)
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }
    if (MenuWidgetInstance)
    {
        MenuWidgetInstance->RemoveFromParent();
        MenuWidgetInstance = nullptr;
    }
    if(MenuWidgetClass)
    {
        MenuWidgetInstance = CreateWidget<UUserWidget>(this, MenuWidgetClass);
        if (MenuWidgetInstance)
        {
            MenuWidgetInstance->AddToViewport(); // 뷰포트에 랜더링

            bShowMouseCursor = true;
            SetInputMode(FInputModeUIOnly());
        }
        if (UTextBlock* ButtonText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName(TEXT("StartText"))))
        {
            if (bIsRestart)
            {
                ButtonText->SetText(FText::FromString(TEXT("Restart")));
            }
            else
            {
                ButtonText->SetText(FText::FromString(TEXT("Start")));
            }
        }
        if (bIsRestart)
        {
            UFunction* PlayAnimFunc = MenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
            if (PlayAnimFunc)
            {
                MenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr); // 애니메이션 재생
            }
            if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
            {
                if(USpartaGameInstance* SpartaGameInstance=Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
                {
                    TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)));
                }
            }
        }
    }
}

void ASpartaPlayerController::ShowGameHUD()
{
    if (HUDWidgetInstance)
    {
        HUDWidgetInstance->RemoveFromParent();
        HUDWidgetInstance = nullptr;
    }
    if (MenuWidgetInstance)
    {
        MenuWidgetInstance->RemoveFromParent();
        MenuWidgetInstance = nullptr;
    }
    if (HUDWidgetClass)
    {
        HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport(); // 뷰포트에 랜더링

            bShowMouseCursor = false;
            SetInputMode(FInputModeGameOnly());
        }
        ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
        if(SpartaGameState)
        {
            SpartaGameState->UpdateHUD();
        }
    }
}

void ASpartaPlayerController::ShowGameWave2()
{
    if (HUDWidgetInstance == nullptr) return;
    UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("Wave2Anim"));
    if (PlayAnimFunc)
    {
        HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("Wave2Anim Played"));
    }
}

void ASpartaPlayerController::ShowGameWave3()
{
    if (HUDWidgetInstance == nullptr) return;
    UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("Wave3Anim"));
    if (PlayAnimFunc)
    {
        HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
    }
}

void ASpartaPlayerController::ShowLevelUp()
{
    if (HUDWidgetInstance == nullptr) return;
    UFunction* PlayAnimFunc = HUDWidgetInstance->FindFunction(FName("LevelUpAnim"));
    if (PlayAnimFunc)
    {
        HUDWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
    }
}

void ASpartaPlayerController::StartGame()
{
    if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        SpartaGameInstance->CurrentLevelIndex = 0;
        SpartaGameInstance->TotalScore = 0;
    }
    UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
    SetPause(false);
}

