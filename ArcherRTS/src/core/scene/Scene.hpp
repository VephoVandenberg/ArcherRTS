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

		void move(float dt)
		{
			position = Vector3Add(position, Vector3Scale(velocity, dt));
		}
	};

	struct ModelComponent
	{
		Color color;
	};

	struct Team1Component {};
	struct Team2Component {};

	struct UnitComponent
	{
		int32_t health;
		float closestEnemyDistance = std::numeric_limits<float>::infinity();

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
		enum Team
		{
			_1,
			_2
		};

		void spawnSquadsSystem(const float dt);
		
		template<typename T1, typename T2>
		void battleSystem(const float dt);

		template<typename T>
		bool spawnUnitSystem(const Vector3& spawnPoint, const Vector3& offset, const Vector3& velocity, const Color& color, int32_t& counter);


		float getDistanceBetweenUnits(const entt::entity e1, const entt::entity e2) const;

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

					if (getDistanceBetweenUnits(entity, entity_) <= Constants::g_attackDistance)
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

	template<typename T>
	bool Scene::spawnUnitSystem(const Vector3& spawnPoint, const Vector3& offset, const Vector3& baseVelocity, const Color& color, int32_t& counter)
	{
		if (counter >= Constants::g_maxSquad || m_timer < Constants::g_spawnPoint)
		{
			return false;
		}

		auto entity = m_registry.create();

		m_registry.emplace<TransformComponent>(entity, Vector3Add(spawnPoint, offset), baseVelocity);
		m_registry.emplace<ModelComponent>(entity, color);
		m_registry.emplace<UnitComponent>(entity, 100, 0.0f, UnitComponent::MARCH);
		m_registry.emplace<T>(entity);

		counter++;

		return true;
	}
}
