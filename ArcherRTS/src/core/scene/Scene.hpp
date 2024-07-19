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
		float closesEnemyDistance = std::numeric_limits<float>::infinity();

		void move(float dt)
		{
			position = Vector3Add(position, Vector3Scale(velocity, dt));
		}
	};

	struct ModelComponent
	{
		float radius;
		float height;
		int32_t slices;
		Color color;
	};

	struct UnitComponent
	{
		int32_t health;
		float damagePercentage;
	};

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		void updateSystem(const float dt);
		void renderingSystem() const;

	private:
		void spawnArmies(const float dt);
		bool recruteUnit(std::vector<entt::entity>& team1, std::vector<entt::entity>& team2,
			const Vector3& spawnPoint, const Vector3& offset, const Color& color);

		float getDistanceBetweenUnits(const entt::entity e1, const entt::entity e2) const;

 		entt::registry m_registry;

		std::vector<entt::entity> m_team1;
		std::vector<entt::entity> m_team2;

		float m_timer = 0.0f;
	};
}

