#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"   

AMineItem::AMineItem()
{
    ItemType = "Mine";
    ExplosionDelay = 5.0f;
    ExplosionDamage = 30;
    ExplosionRadius = 300.0f;
    bHasExploded = false;

    ExplosionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
    ExplosionCollision->InitSphereRadius(ExplosionRadius);
    ExplosionCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    ExplosionCollision->SetupAttachment(Scene);
}
void AMineItem::ActivateItem(AActor* Activator)
{
    // 게임 월드 -> 타이머 매니저
    // 타이머 핸들러
    if (bHasExploded) return;

    Super::ActivateItem(Activator);

    GetWorld()->GetTimerManager().SetTimer(
        ExplosionTimerHandle,
        this,
        &AMineItem::Explode,
        ExplosionDelay,
        false
    );
    bHasExploded = true;
}

void AMineItem::Explode()
{
    UParticleSystemComponent* Particle = nullptr;
    if(BoomParticle)
    {
        Particle = UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            BoomParticle,
            GetActorLocation(),
            GetActorRotation(),
            false
        );
    }
    if(BoomSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            BoomSound,
            GetActorLocation()
        );
    }
    TArray<AActor*> OverlappingActors;
    ExplosionCollision->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (Actor && Actor->ActorHasTag("Player"))
        {
            UGameplayStatics::ApplyDamage(
                Actor,
                ExplosionDamage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
        }

    }
    DestroyItem();
    if (Particle)
    {
        FTimerHandle DestroyParticleHandle;
        TWeakObjectPtr<UActorComponent> WeakParticle = Particle;

        GetWorld()->GetTimerManager().SetTimer(
            DestroyParticleHandle,
            [WeakParticle]()
            {
                if (WeakParticle.IsValid())
                {
                    WeakParticle->DestroyComponent();
                }
            },
            2.0f,
            false
        );
    }
}