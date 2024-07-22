#pragma once

#include <memory>
#include <entt.hpp>
#include <raylib.h>
#include <raymath.h>

#include "SceneConfig.hpp"

namespace Core
{
	struct TransformComponent
	{
		Vector3 position;
		Vector3 velocity;
		BoundingBox box;
	};

	struct ModelComponent
	{
		Color color;
	};

	struct Team1Component {};
	struct Team2Component {};

	struct ProjectileComponent
	{
		entt::entity target;
	};

	struct EnemyComponent
	{
		entt::entity enemy;
	};

	struct UnitComponent
	{
		int32_t health;
		Vector3 enemyPosition;
		float enemyDistance;
		float attackTimer = 0.0f;
		enum State
		{
			MARCH,
			ATTACK
		} state = MARCH;
	};

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		void updateSystem(const float dt);
		void renderingSystem() const;
	private:
		void spawnSquadsSystem(const float dt);

		template<typename T1, typename T2>
		void battleSystem(const float dt);

		template<typename T>
		bool spawnUnitSystem(const Vector3& spawnPoint, const Vector3& offset, const Vector3& velocity, const Color& color, int32_t& counter);

		float getDistanceBetweenUnits(const entt::entity e1, const entt::entity e2) const;
		Vector3 getProjectileVelocity(const entt::entity shooter) const;
		bool projectileInBounds(const Vector3& projectilePos, const BoundingBox& box);

		int32_t m_team1Counter = 0;
		int32_t m_team2Counter = 0;

		entt::registry m_registry;

		float m_timer = 0.0f;
	};

	template<typename T1, typename T2>
	void Scene::battleSystem(const float dt)
	{
		auto team1 = m_registry.group<T1>(entt::get<TransformComponent, UnitComponent>);
		auto team2 = m_registry.group<T2>(entt::get<TransformComponent, UnitComponent>);

		for (auto [entity, transform, unit] : team1.each())
		{
			if (unit.health <= 0.0f)
			{
				m_registry.destroy(entity);
				continue;
			}

			if (unit.state == UnitComponent::MARCH)
			{
				transform.position = Vector3Add(transform.position, Vector3Scale(transform.velocity, 10 * dt));
				transform.box.min = Vector3Add(transform.box.min, Vector3Scale(transform.velocity, 10 * dt));
				transform.box.max = Vector3Add(transform.box.max, Vector3Scale(transform.velocity, 10 * dt));
				

				for (auto entity_ : team2)
				{
					auto& unit_ = m_registry.get<UnitComponent>(entity_);
					auto& transform_ = m_registry.get<TransformComponent>(entity_);

					float distance = getDistanceBetweenUnits(entity, entity_);
					if (distance <= Constants::g_attackDistance)
					{
						unit.enemyPosition = transform_.position;
						unit.state = UnitComponent::ATTACK;
						unit.enemyDistance = distance;

						if (m_registry.all_of<EnemyComponent>(entity))
						{
							auto& enemyComp = m_registry.get<EnemyComponent>(entity);
							enemyComp.enemy = entity_;
						}
						else
						{
							m_registry.emplace<EnemyComponent>(entity, entity_);
						}
					}
					else if (distance < unit.enemyDistance)
					{
						unit.enemyDistance = distance;
						transform.velocity = 
							Vector3Scale(Vector3Normalize(Vector3Subtract(transform_.position, transform.position)), 0.5f);
					}
				}
			}
			else
			{
				auto& eComp = m_registry.get<EnemyComponent>(entity);
				if (!m_registry.valid(eComp.enemy))
				{
					m_registry.erase<EnemyComponent>(entity);
					unit.state = UnitComponent::MARCH;
					unit.attackTimer = 0.0f;
					continue;
				}

				unit.attackTimer += dt;
				if (unit.attackTimer >= Constants::g_attackTimePoint)
				{
					auto projectile = m_registry.create();

					auto& unitColor = m_registry.get<ModelComponent>(entity); 

					Vector3 velocity = getProjectileVelocity(entity);

					m_registry.emplace<ModelComponent>(projectile, unitColor);
					m_registry.emplace<TransformComponent>(projectile, transform.position, velocity);
					m_registry.emplace<ProjectileComponent>(projectile, m_registry.get<EnemyComponent>(entity).enemy);
					
					unit.attackTimer = 0.0f;
				}
			}
		}
	}

	template<typename T>
	bool Scene::spawnUnitSystem(const Vector3& spawnPoint, const Vector3& offset, const Vector3& baseVelocity,
		const Color& color, int32_t& counter)
	{
		if (counter >= Constants::g_maxSquad || m_timer < Constants::g_spawnTimePoint)
		{
			return false;
		}

		auto entity = m_registry.create();

		Vector3 point = Vector3Add(spawnPoint, offset);

		BoundingBox box = {
			Vector3Subtract(point, {Constants::g_radius, Constants::g_height * 0.5f, Constants::g_radius}),
			Vector3Add(point, {Constants::g_radius, Constants::g_height * 0.5f, Constants::g_radius})
		};

		m_registry.emplace<TransformComponent>(entity, point, baseVelocity, box);
		m_registry.emplace<ModelComponent>(entity, color);
		m_registry.emplace<UnitComponent>(entity, Constants::g_health, point, std::numeric_limits<float>::infinity(), 0.0f, UnitComponent::MARCH);
		m_registry.emplace<T>(entity);

		counter++;

		return true;
	}
}
