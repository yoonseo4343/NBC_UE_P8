#include "PoisonItem.h"
#include "SpartaCharacter.h"

APoisonItem::APoisonItem()
{
    PoisonDuration = 5.0f;
    PoisonDamage = 2.0f;
    ItemType = "Poison";
}

void APoisonItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    if (Activator && Activator->ActorHasTag("Player"))
    {
        if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
        {
            PlayerCharacter->DrinkPoison(PoisonDuration,PoisonDamage);
        }
        DestroyItem();
    }
}