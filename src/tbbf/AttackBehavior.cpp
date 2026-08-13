#include <tbbf/AttackBehavior.h>
#include <tbbf/TbbfMacroInstance.h>

bool AttackBehavior::HasTool(const FrameView& currentFrame) {
	static constexpr ColorRgba toolPixelColor{ 0x1F, 0x1F, 0x1F };

	std::optional<ColorRgba> color = PixelAnalyzer::GetPixelColor(currentFrame, POINT{ currentFrame.width / 2, currentFrame.height - 15 });
	return color == toolPixelColor;
}
TickStatus AttackBehavior::TickTbbf(TbbfMacroInstance* instance, TbbfCustomContext* context, const FrameView& currentFrame) {
	if (context->playerStatus != PlayerStatus::Deployed)
		return TickStatus::Skipped;

	if (!HasTool(currentFrame))
		return TickStatus::Skipped;

	if (!context->toolEquipped) {
		instance->SendKey(0x31);
		context->toolEquipped = true;
	}
	instance->ClickClient(POINT{ currentFrame.width / 2, currentFrame.height / 2 - 10 }, 10);
	if (context->waveNumber > 36)
		instance->SendKey(0x47);
	return TickStatus::Skipped;
}