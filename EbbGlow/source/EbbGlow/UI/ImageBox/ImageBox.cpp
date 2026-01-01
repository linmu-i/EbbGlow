#include <EbbGlow/UI/ImageBox/ImageBox.h>
#include <EbbGlow/Graphics/Graphics.h>

namespace ebbglow::ui
{
	void ImageBoxDraw::draw()
	{
		if (imgBox.img.valid()) gfx::DrawTexture(imgBox.img, imgBox.position, imgBox.scale);
	}

	void ImageBoxSystem::update()
	{
		imgBoxes->active()->forEach([this](core::entity id, ImageBoxCom& act)
			{
				auto& ina = *imgBoxes->inactive()->get(id);
				(*layer)[act.layerDepth].push_back(std::make_unique<ImageBoxDraw>(act));
			});
	}
}