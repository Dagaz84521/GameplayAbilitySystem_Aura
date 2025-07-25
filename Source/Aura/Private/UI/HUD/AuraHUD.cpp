//  Copyright Dagaz84521


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParam& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParam(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass is not set in AuraHUD!"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetController is not set in AuraHUD!"));
	
	UUserWidget* Widget = CreateWidget(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParam WidgetControllerParam(PC, PS, ASC, AS);
	
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParam);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	WidgetController->SetWidgetControllerParam(WidgetControllerParam);
	Widget->AddToViewport();
	
}

void AAuraHUD::BeginPlay()
{
	Super::BeginPlay();
}

