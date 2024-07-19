#pragma once

#include <memory>
#include <entt.hpp>
#include <raylib.h>
#include <raymath.h>

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
		bool spawnUnitSystem(Team team, const Vector3& spawnPoint, const Vector3& offset, const Vector3& velocity, const Color& color);
		void battleSystem(const float dt);

		float getDistanceBetweenUnits(const entt::entity e1, const entt::entity e2) const;

		int32_t m_team1Counter = 0;
		int32_t m_team2Counter = 0;

 		entt::registry m_registry;

		float m_timer = 0.0f;
	};
}
