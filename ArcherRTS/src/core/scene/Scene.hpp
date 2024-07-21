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
	};

	struct ModelComponent
	{
		Color color;
	};

	struct Team1Component {};
	struct Team2Component {};

	struct ProjectileComponent
	{

	};

	struct UnitComponent
	{
		int32_t health;
		Vector3 enemyPosition;
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
			if (unit.state == UnitComponent::MARCH)
			{
				transform.position = Vector3Add(transform.position, Vector3Scale(transform.velocity, 10 * dt));
				for (auto entity_ : team2)
				{
					auto& unit_ = m_registry.get<UnitComponent>(entity_);
					auto& transform_ = m_registry.get<TransformComponent>(entity_);

					float distance = getDistanceBetweenUnits(entity, entity_);
					if (distance <= Constants::g_attackDistance)
					{
						unit.enemyPosition = transform_.position;
						unit.state = UnitComponent::ATTACK;
					}
				}
			}
			else
			{
				unit.attackTimer += dt;
				if (unit.attackTimer >= Constants::g_attackTimePoint)
				{
					auto unitColor = m_registry.get<ModelComponent>(entity);
					auto projectile = m_registry.create();

					Vector3 velocity = getProjectileVelocity(entity);

					auto transform_ = m_registry.emplace<TransformComponent>(projectile, transform.position, velocity);
					
					m_registry.emplace<ModelComponent>(projectile, unitColor);
					m_registry.emplace<ProjectileComponent>(projectile);
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

		m_registry.emplace<TransformComponent>(entity, point, baseVelocity);
		m_registry.emplace<ModelComponent>(entity, color);
		m_registry.emplace<UnitComponent>(entity, Constants::g_health, point, 0.0f, UnitComponent::MARCH);
		m_registry.emplace<T>(entity);

		counter++;

		return true;
	}
}
