// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/EnemySpawner.h"
#include "Characters/Enemy/EnemyCharacter.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	// 스포너의 위치를 잡기 위한 루트 컴포넌트
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 에디터에서 화살표 등으로 방향을 알기 쉽게 하려면 추가 가능 (선택 사항)
	// ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	// ArrowComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 바로 적 소환
	RespawnEnemy();
}

void AEnemySpawner::RespawnEnemy()
{
	// Case 1: 이미 생성된 적이 있는 경우 (죽어서 숨겨져 있거나, 살아있는 경우)
	if (SpawnedEnemy && IsValid(SpawnedEnemy))
	{
		// 적을 파괴하지 않고 상태만 리셋하여 재사용 (오브젝트 풀링 효과)
		// AEnemyCharacter에 구현한 ResetEnemy() 호출
		SpawnedEnemy->ResetEnemy();

	}
	// Case 2: 아직 적이 생성되지 않은 경우 (게임 시작 시)
	else if (EnemyClass)
	{
		FActorSpawnParameters SpawnParams;
		// 바닥에 살짝 겹쳐도 무조건 소환되도록 설정
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// 적 생성 및 변수에 저장
		SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(
			EnemyClass,
			GetActorLocation(),
			GetActorRotation(),
			SpawnParams
		);

		// 생성된 적에게 자신의 홈 위치(HomeLocation)를 명확히 갱신해주고 싶다면 여기서 설정 가능
		if (SpawnedEnemy)
		{
			SpawnedEnemy->HomeLocation = GetActorLocation();
		}
	}
}





