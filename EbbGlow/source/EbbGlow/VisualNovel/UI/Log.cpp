#include <EbbGlow/VisualNovel/UI/Log.h>

namespace ebbglow::visualnovel
{
	void ItemDraw::draw()
	{
		auto& cfg = com.cfg;
		auto texts = utils::TextLineCalculateWithWordWrap(com.text, cfg.textSize, cfg.textSize * 0.1f, com.font, cfg.ScreenWidth * 0.6666667f);
		//if ()
	}

	void LogSystem::update()
	{

	}
}