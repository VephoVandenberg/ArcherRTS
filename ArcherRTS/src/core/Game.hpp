#pragma once

#include <memory>
#include <cstdint>
#include <raylib.h>

namespace Core
{
	class Scene;

	class Game final
	{
	public:
		Game(const int32_t width, const int32_t height);
		~Game() noexcept;

		void run();

		Game() = delete;
		Game(const Game&) = delete;
		Game(Game&&) = delete;
		Game& operator=(const Game&) = delete;
		Game& operator=(Game&&) = delete;
	
	private:
		void update();
		void draw() const;
		void drawGround() const;

		std::unique_ptr<Scene> m_scene = nullptr;

		Camera3D m_camera;
	};
}

