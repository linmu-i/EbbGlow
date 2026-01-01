#pragma once

#include <EbbGlow/Core/World.h>
#include <EbbGlow/VisualNovel/VisualNovel/VisualNovel.h>

namespace ebbglow::visualnovel
{
	class MusicManager : public core::SystemBase
	{
	private:
		const VisualNovelConfig& cfg;

		rsc::SharedMusic bgm;
		rsc::SharedSound voice;

	public:
		MusicManager(const VisualNovelConfig& config) : cfg(config) {}

		void update() override;
		void SetBgm(const rsc::SharedMusic& music, float volume) noexcept;
		void SetVoice(const rsc::SharedSound& sound, float volume) noexcept;
		void StopBgm() noexcept;
		void StopVoice() noexcept;
		void PauseBgm() noexcept;
		void PauseVoice() noexcept;
		void ResumeBgm() noexcept;
		void ResumeVoice() noexcept;
	};
}