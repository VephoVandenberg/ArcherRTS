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
	if (m_team1Counter < g_maxSquad || m_team2Counter < g_maxSquad)
	{
		spawnSquadsSystem(dt);
	}

	battleSystem(dt);
}

void Scene::battleSystem(const float dt)
{
	auto team1 = m_registry.group<Team1Component>(entt::get<TransformComponent, UnitComponent>);
	auto team2 = m_registry.group<Team2Component>(entt::get<TransformComponent, UnitComponent>);

	for (auto entity : team1)
	{
		auto& transform = team1.get<TransformComponent>(entity);
		auto& unit = team1.get<UnitComponent>(entity);

		if (unit.state == UnitComponent::MARCH)
		{
			transform.move(dt);
			for (auto entity_ : team2)
			{
				auto& unit_ = m_registry.get<UnitComponent>(entity_);

				if (getDistanceBetweenUnits(entity, entity_) <= g_attackDistance)
				{
					unit.state = UnitComponent::ATTACK;
					unit_.state = UnitComponent::ATTACK;
				}
			}
		}
		else
		{

		}
	}

	for (auto entity : team2)
	{
		auto& transform = team1.get<TransformComponent>(entity);
		auto& unit = team1.get<UnitComponent>(entity);

		if (unit.state == UnitComponent::MARCH)
		{
			transform.move(dt);
			for (auto entity_ : team1)
			{
				auto& unit_ = m_registry.get<UnitComponent>(entity_);

				if (getDistanceBetweenUnits(entity, entity_) <= g_attackDistance)
				{
					unit.state = UnitComponent::ATTACK;
					unit_.state = UnitComponent::ATTACK;
				}
			}
		}
		else
		{

		}
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

	Vector3 offset1 = { static_cast<float>(GetRandomValue(-5, 5)), 0,	static_cast<float>(GetRandomValue(-5, 5)) };;
	Vector3 offset2 = { static_cast<float>(GetRandomValue(-5, 5)), 0, static_cast<float>(GetRandomValue(-5, 5)) };;

	bool status =
		spawnUnitSystem(_1, g_spawnPointT1, offset1, velocity1, g_colorT1) &&
		spawnUnitSystem(_2, g_spawnPointT2, offset2, velocity2, g_colorT2);

	if (status)
	{
		m_timer = 0.0f;
	}
}

bool Scene::spawnUnitSystem(Team team,
	const Vector3& spawnPoint, const Vector3& offset, const Vector3& baseVelocity, const Color& color)
{
	int32_t& counter = team == _1 ? m_team1Counter : m_team2Counter;

	if (counter >= g_maxSquad || m_timer < g_spawnPoint)
	{
		return false;
	}

	auto entity = m_registry.create();

	m_registry.emplace<TransformComponent>(entity, Vector3Add(spawnPoint, offset), baseVelocity);
	m_registry.emplace<ModelComponent>(entity, color);
	m_registry.emplace<UnitComponent>(entity, 100, 0.0f, UnitComponent::MARCH);

	if (team == _1)
	{
		m_registry.emplace<Team1Component>(entity);
	}
	else
	{
		m_registry.emplace<Team2Component>(entity);
	}

	counter++;

	return true;
}

void Scene::renderingSystem() const
{
	auto view = m_registry.view<TransformComponent, ModelComponent>();

	for (auto [entity, transform, model] : view.each())
	{
		DrawCylinder(
			transform.position,
			g_radius, g_radius,
			g_height, g_slices,
			model.color);
	}
}
