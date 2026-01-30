#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowingItem.generated.h"

UCLASS()
class SPARTAPJ_API ASlowingItem : public ABaseItem
{
	GENERATED_BODY()
public:
	ASlowingItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
	float SlowingDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Item")
	float SpeedMultiplier;

	virtual void ActivateItem(AActor* Activator) override;
};
