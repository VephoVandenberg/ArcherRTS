#include "Scene.hpp"

using namespace Core;

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

void Scene::updateSystem(const float dt)
{
	// Spawn units
	if (m_team1.size() < g_maxSquad || m_team2.size() < g_maxSquad)
	{
		spawnArmies(dt);
	}


	auto view = m_registry.view<TransformComponent>();
	for (auto [entity, transform] : view.each())
	{
		transform.move(dt);
	}

}


float Scene::getDistanceBetweenUnits(const entt::entity e1, const entt::entity e2) const
{
	const auto& t1 = m_registry.get<TransformComponent>(e1);
	const auto& t2 = m_registry.get<TransformComponent>(e2);

	return Vector3Distance(t1.position, t2.position);
}

void Scene::spawnArmies(const float dt)
{
	m_timer += dt;

	Vector3 offset1 = { static_cast<float>(GetRandomValue(-5, 5)), 0,	static_cast<float>(GetRandomValue(-5, 5)) };;
	Vector3 offset2 = { static_cast<float>(GetRandomValue(-5, 5)), 0, static_cast<float>(GetRandomValue(-5, 5)) };;

	bool status =
		recruteUnit(m_team1, m_team2, g_spawnPointT1, offset1, g_colorT1) &&
		recruteUnit(m_team2, m_team1, g_spawnPointT2, offset2, g_colorT2);

	if (status)
	{
		m_timer = 0.0f;
	}
}

bool Scene::recruteUnit(std::vector<entt::entity>& team1, std::vector<entt::entity>& enemyTeam,
	const Vector3& spawnPoint, const Vector3& offset, const Color& color)
{
	if (team1.size() >= g_maxSquad || m_timer < g_spawnPoint)
	{
		return false;
	}

	auto entity = m_registry.create();


	auto& transform = m_registry.emplace<TransformComponent>(entity);
	transform.position = Vector3Add(spawnPoint, offset);
	transform.velocity = { 0.0f, 0.0f, 0.0f };

	for (auto enemy : enemyTeam)
	{
		const auto& transform_ = m_registry.get<TransformComponent>(enemy);

		float newDistance = Vector3Distance(transform_.position, spawnPoint);
		if (newDistance < transform.closesEnemyDistance)
		{
			transform.closesEnemyDistance = newDistance;
			transform.velocity = Vector3Add(
				transform.velocity,
				Vector3Scale(Vector3Normalize(Vector3Subtract(transform_.position, transform.position)), 3));
		}
	}

	auto& model = m_registry.emplace<ModelComponent>(entity);
	model.color = color;
	model.height = g_height;
	model.radius = g_radius;
	model.slices = g_slices;

	team1.push_back(entity);

	return true;
}

void Scene::renderingSystem() const
{
	auto view = m_registry.view<TransformComponent, ModelComponent>();

	for (auto [entity, transform, model] : view.each())
	{
		DrawCylinder(
			transform.position,
			model.radius, model.radius,
			model.height, model.slices,
			model.color);
	}
}
