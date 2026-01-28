#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPJ_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpartaCharacter();

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(BlueprintPure,Category="Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	void RollCamera();
	void UpdateRollCamera();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "health")
	float Health;

    virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
    void StartJump(const FInputActionValue& value);
	UFUNCTION()
    void StopJump(const FInputActionValue& value);
	UFUNCTION()
    void Look(const FInputActionValue& value);
	UFUNCTION()
    void StartSprint(const FInputActionValue& value);
	UFUNCTION()
    void StopSprint(const FInputActionValue& value);

	void OnDeath();
	void UpdateOverheadHP();
	FTimerHandle RollTimerHandle;
	FRotator RollTarget;

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};
