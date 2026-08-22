#include <tbbf/behaviors/DeployBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

void DeployBehavior::Deploy(TbbfMacroInstance* instance, TbbfContext* context) {
	instance->SendKey(VK_RETURN);
	instance->ClickClient(context->spawnPosition, 10);
}


TickStatus DeployBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfContext* context, const FrameView& currentFrame) {
	TbbfContext::Decisions& decisions = context->decisions;
	if (decisions.shouldDeploy)
		Deploy(instance, context);

	return TickStatus::Skipped;
}