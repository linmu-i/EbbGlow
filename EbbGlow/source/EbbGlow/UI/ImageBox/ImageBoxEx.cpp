#include <EbbGlow/UI/ImageBox/ImageBoxEx.h>
#include <EbbGlow/Graphics/Graphics.h>

namespace ebbglow::ui
{
	void ImageBoxExDraw::draw()
	{
		if (imgBox.img.valid()) gfx::DrawTexturePro(imgBox.img, { 0.0f, 0.0f, static_cast<float>(imgBox.img.width()), static_cast<float>(imgBox.img.height()) }, { imgBox.position.x, imgBox.position.y, imgBox.img.width() * imgBox.scale, imgBox.img.height() * imgBox.scale }, imgBox.origin, imgBox.rotation, imgBox.tint);
	}

	void ImageBoxExSystem::update()
	{
		imgBoxes->active()->forEach([this](core::entity id, ImageBoxExCom& act)
			{
				auto& ina = *imgBoxes->inactive()->get(id);
				(*act.layer)[act.layerDepth].push_back(std::make_unique<ImageBoxExDraw>(act));
			});
	}
}