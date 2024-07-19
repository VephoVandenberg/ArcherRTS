#pragma once

namespace Constants
{
	constexpr float g_spawnPoint = 0.1f;
	constexpr float g_radius = 0.25f;
	constexpr float g_height = 1.7f;
	constexpr float g_slices = 5;
	constexpr int32_t g_maxSquad = 20;
	constexpr int32_t g_attackDistance = 40;
	constexpr Vector3 g_spawnPointT1 = { 10.0f, g_height * 0.5f, 10.0f };
	constexpr Vector3 g_spawnPointT2 = { 90.0f, g_height * 0.5f, 90.0f };
	constexpr Color g_colorT1 = { 255, 0, 0, 255 };
	constexpr Color g_colorT2 = { 0, 0, 255, 255 };
	constexpr Vector3 g_unitVelocity = { 4.0f, 0.0f, 4.0f };
}