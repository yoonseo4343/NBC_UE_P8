#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UPostProcessComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPJ_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UPostProcessComponent* PostProcessComp;

	UFUNCTION(BlueprintPure,Category="Health")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);


	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void DrinkPoison(float Duration, float Damage);
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void PoisonDamage();
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void PoisonEnd();
	UFUNCTION(BlueprintCallable, Category = "Debuff")
    void Slowed(float Duration, float SpeedMultiplier);
	UFUNCTION(BlueprintCallable, Category = "Debuff")
	void SlowedEnd();

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
	void UpdateHP();
	FTimerHandle RollTimerHandle;
	FTimerHandle BlurTimerHandle;
    FTimerHandle PoisonTimerHandle;
	FTimerHandle SlowedTimerHandle;
	FRotator RollTarget;

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
    bool bIsSlowed;
    float PoisonDamageAmount;
};
