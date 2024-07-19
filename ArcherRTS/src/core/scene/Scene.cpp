#include "Scene.hpp"

using namespace Core;

void Scene::updateSystem(const float dt)
{
	// Spawn units
	if (m_team1Counter < Constants::g_maxSquad || m_team2Counter < Constants::g_maxSquad)
	{
		spawnSquadsSystem(dt);
	}

	battleSystem<Team1Component, Team2Component>(dt);
	battleSystem<Team2Component, Team1Component>(dt);
}

float Scene::getDistanceBetweenUnits(const entt::entity e1, const entt::entity e2) const
{
	const auto& t1 = m_registry.get<TransformComponent>(e1);
	const auto& t2 = m_registry.get<TransformComponent>(e2);

	return Vector3Distance(t1.position, t2.position);
}

void Scene::spawnSquadsSystem(const float dt)
{
	m_timer += dt;

	Vector3 velocity1 = { 1, 0,	1 };
	Vector3 velocity2 = {-1, 0,	-1 };

	Vector3 offset1 = { static_cast<float>(GetRandomValue(-5, 5)), 0,	static_cast<float>(GetRandomValue(-5, 5)) };
	Vector3 offset2 = { static_cast<float>(GetRandomValue(-5, 5)), 0, static_cast<float>(GetRandomValue(-5, 5)) };

	bool status =
		spawnUnitSystem<Team1Component>(Constants::g_spawnPointT1, offset1, velocity1, Constants::g_colorT1, m_team1Counter) &&
		spawnUnitSystem<Team2Component>(Constants::g_spawnPointT2, offset2, velocity2, Constants::g_colorT2, m_team2Counter);

	if (status)
	{
		m_timer = 0.0f;
	}
}

void Scene::renderingSystem() const
{
	auto view = m_registry.view<TransformComponent, ModelComponent>();

	for (auto [entity, transform, model] : view.each())
	{
		DrawCylinder(
			transform.position,
			Constants::g_radius, Constants::g_radius,
			Constants::g_height, Constants::g_slices,
			model.color);
	}
}
