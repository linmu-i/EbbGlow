#pragma once

#include <atomic>

#include <raylib.h>
#include <EbbGlow/Resources/Resources.h>
#include "ResourceCreator.h"

namespace ebbglow::resource
{
	struct ImageBlock
	{
		std::atomic<size_t> refCnt;
		::Image image;
		ImageBlock(size_t refCnt, const ::Image& image) : refCnt(refCnt), image(image) {}
	};

	struct TextureBlock
	{
		std::atomic<size_t> refCnt;
		::Texture texture;
		TextureBlock(size_t refCnt, ::Texture texture) : refCnt(refCnt), texture(texture) {}
	};

	struct FontBlock
	{
		std::atomic<size_t> refCnt;
		::Font font;
		FontBlock(size_t refCnt, ::Font font) : refCnt(refCnt), font(font) {}
	};

	struct MusicBlock
	{
		std::atomic<size_t> refCnt;
		::Music music;
		SharedFile fileData;
		MusicBlock(size_t refCnt, ::Music music) : refCnt(refCnt), music(music), fileData() {}
		MusicBlock(size_t refCnt, ::Music music, const SharedFile fileData) : refCnt(refCnt), music(music), fileData(fileData) {}
	};

	struct ShaderBlock
	{
		std::atomic<size_t> refCnt;
		::Shader shader;
		ShaderBlock(size_t refCnt, ::Shader shader) : refCnt(refCnt), shader(shader) {}
	};

	struct RenderTextureBlock
	{
		std::atomic<size_t> refCnt;
		::RenderTexture renderTexture;

		SharedTexture texture;
		SharedTexture depth;

		RenderTextureBlock(size_t refCnt, ::RenderTexture renderTexture) : refCnt(refCnt), renderTexture(renderTexture)
		{
			texture = ResourceCreator::CreateTexture(renderTexture.texture);
			depth = ResourceCreator::CreateTexture(renderTexture.depth);
		}
	};

	struct SoundBlock
	{
		std::atomic<size_t> refCnt;
		::Sound sound;
		SoundBlock(size_t refCnt, ::Sound sound) : refCnt(refCnt), sound(sound) {}
	};
}