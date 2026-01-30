#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PoisonItem.generated.h"

UCLASS()
class SPARTAPJ_API APoisonItem : public ABaseItem
{
	GENERATED_BODY()
public:
	APoisonItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
    float PoisonDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
	float PoisonDamage;

	virtual void ActivateItem(AActor* Activator) override;

    void SetPoisonDamage(float DamageAmount) { PoisonDamage = DamageAmount; }
};
