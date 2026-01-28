#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "HealingItem.generated.h"


UCLASS()
class SPARTAPJ_API AHealingItem : public ABaseItem
{
	GENERATED_BODY()
public:
	AHealingItem();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,category="Item")
	int32 HealAmount;

	virtual void ActivateItem(AActor* Activator) override;
	
};
