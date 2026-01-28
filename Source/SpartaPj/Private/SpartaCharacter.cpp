#include "SpartaCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "SpartaPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

// Sets default values
ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 2.0;
	SprintSpeed=NormalSpeed*SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    UpdateOverheadHP();
}

// Called to bind functionality to input
void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInput=Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
        if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
    }
}

void ASpartaCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(),MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}
float ASpartaCharacter::GetHealth() const
{
	return Health;
}
void ASpartaCharacter::AddHealth(float Amount)
{
	Health= FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
    UpdateOverheadHP();
}

void ASpartaCharacter::RollCamera()
{
	if (!CameraComp) return;
    UE_LOG(LogTemp, Warning, TEXT("Roll Camera"));


	RollTarget = CameraComp->GetRelativeRotation();
	RollTarget.Roll += 180.f;

	GetWorldTimerManager().SetTimer(
		RollTimerHandle,
        this,
		&ASpartaCharacter::UpdateRollCamera,
		0.016f,
		true
	);
}
void ASpartaCharacter::UpdateRollCamera()
{
	if (!CameraComp || !GetWorld())
	{
		GetWorldTimerManager().ClearTimer(RollTimerHandle);
		return;
	}

	FRotator Cur = CameraComp->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(
		Cur,
		RollTarget,
		GetWorld()->GetDeltaSeconds(),
		6.f
	);

	CameraComp->SetRelativeRotation(NewRot);

	if (FMath::Abs(NewRot.Roll - RollTarget.Roll) < 0.5f)
	{
		CameraComp->SetRelativeRotation(RollTarget);
		GetWorldTimerManager().ClearTimer(RollTimerHandle);

	}
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();


	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* GameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (GameState)
	{
		GameState->OnGameOver();
	}
	UE_LOG(LogTemp, Warning, TEXT("Death.."));
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

    UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
    if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
        HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}