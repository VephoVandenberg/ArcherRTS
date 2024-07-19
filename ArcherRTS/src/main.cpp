#include "core/Game.hpp"

constexpr int g_screenWidth = 1200;
constexpr int g_screenHeight = 800;

int main(int argc, char** argv)
{
	Core::Game* game = new Core::Game(g_screenWidth, g_screenHeight);
	game->run();
	delete game;

	return 0;
}