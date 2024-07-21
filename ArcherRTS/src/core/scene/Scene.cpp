#include <math.h>

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

	auto projectiles = m_registry.view<ProjectileComponent, TransformComponent>();
	for (auto [entity, transform] : projectiles.each())
	{
		transform.velocity.y -= 0.5f * Constants::g_gravity * dt;
		transform.position = Vector3Add(transform.position, Vector3Scale(transform.velocity, dt));
		//transform.position.y -= 0.5f * Constants::g_gravity * dt * dt;
	}
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

	Vector3 offset1 = { static_cast<float>(GetRandomValue(-5, 5)), 0, static_cast<float>(GetRandomValue(-5, 5)) };
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
	auto units = m_registry.view<TransformComponent, ModelComponent>(entt::exclude<ProjectileComponent>);

	for (auto [entity, transform, model] : units.each())
	{
		DrawCylinder(
			transform.position,
			Constants::g_radius, Constants::g_radius,
			Constants::g_height, Constants::g_slices,
			model.color);
	}

	auto projectiles = m_registry.view<ProjectileComponent, TransformComponent, ModelComponent>();
	for (auto [entity, transform, model] : projectiles.each())
	{
		Vector3 end = Vector3Add(transform.position, Vector3Scale(Vector3Normalize(transform.velocity), 0.5f));

		DrawLine3D(transform.position, end, model.color);
	}
}

Vector3 Scene::getProjectileVelocity(const entt::entity shooter) const
{
	auto unit = m_registry.get<UnitComponent>(shooter);
	auto uTransform = m_registry.get<TransformComponent>(shooter);

	float D = Vector3Distance(uTransform.position, unit.enemyPosition);

	float alpha = 0.5f * (asinf((D * Constants::g_gravity) / (Constants::g_projectileSpeed * Constants::g_projectileSpeed)) * RAD2DEG);
	Vector3 dir = Vector3Normalize(Vector3Subtract(unit.enemyPosition, uTransform.position));
	
	Vector3 rotationAxis = Vector3CrossProduct(dir, { 0, 1, 0 });
	dir = Vector3RotateByAxisAngle(dir, rotationAxis, alpha);

	return Vector3Scale(dir, Constants::g_projectileSpeed);
}
