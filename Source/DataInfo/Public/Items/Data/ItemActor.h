#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Items/DataTable/FItemData.h"
#include "ItemActor.generated.h"

UCLASS()
class DATAINFO_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

protected:
    /** 월드에 보이는 메시 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComp;

    /** 어떤 아이템인가 (정의 참조) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FDataTableRowHandle ItemRowHandle;

    /** 수량 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 Quantity = 1;

public:
    /** ItemInstance에서 월드 아이템으로 변환 */
    UFUNCTION(BlueprintCallable, Category = "Item")
    void InitializeFromItem(UItemBase* Item);

    /** 월드에서 직접 초기화할 때 */
    UFUNCTION(BlueprintCallable, Category = "Item")
    void InitializeFromRowHandle(const FDataTableRowHandle& InRowHandle, int32 InQuantity);

protected:
    void UpdateMeshFromItemData();
};
