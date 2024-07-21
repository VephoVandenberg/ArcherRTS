#include "scene/Scene.hpp"

#include "Game.hpp"

using namespace Core;

constexpr int32_t g_framerate = 60;
constexpr Color g_groundColor = { 0, 255, 0, 255 };
constexpr Vector3 g_groundPos = { 50.0f, 0.0f, 50.0f };

Game::Game(const int32_t width, const int32_t height)
	: m_camera({0})
{
	InitWindow(width, height, "First raylib game");
	SetTargetFPS(g_framerate);

	m_scene = std::make_unique<Scene>();

	m_camera.position = { -15.0f, 90.0f, 115.0f };
	m_camera.target = { 50.0f, 0.0f, 50.0f };
	m_camera.up = { 0.0f, 1.0f, 0.0f };
	m_camera.fovy = 100.0f;
	m_camera.projection = CAMERA_ORTHOGRAPHIC;
}

Game::~Game() noexcept
{
	CloseWindow();
}

void Game::run()
{
	WaitTime(0.05f);

	while (!WindowShouldClose())
	{
		update();
		draw();
	}
}

void Game::update()
{
	float dt = GetFrameTime();
	m_scene->updateSystem(dt);
}

void Game::draw() const
{
	BeginDrawing(); {
		ClearBackground(RAYWHITE);
		BeginMode3D(m_camera); {

			drawGround();
			m_scene->renderingSystem();

		} EndMode3D();

		DrawFPS(10, 10);

	} EndDrawing();
}

void Game::drawGround() const
{
	DrawCube(g_groundPos, 100.0f, 1.0f, 100.0f, g_groundColor);
	DrawCubeWires(g_groundPos, 100.0f, 1.0f, 100.0f, MAROON);
}
