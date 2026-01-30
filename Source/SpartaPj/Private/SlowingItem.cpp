#include "SlowingItem.h"
#include "SpartaCharacter.h"

ASlowingItem::ASlowingItem()
{
	SlowingDuration=5.0f;
    SpeedMultiplier = 0.5f;
    ItemType = "Slowing";
}

void ASlowingItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
        {
            PlayerCharacter->Slowed(SlowingDuration, SpeedMultiplier);
        }
        DestroyItem();
    }
}