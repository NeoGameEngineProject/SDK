#include <Platform.h>
#include <Game.h>

#include <WinMain.hpp>
#include <Log.h>

#include <LuaGameState.h>

int main(int argc, char** argv)
{
	Neo::Game game(1024, 768, "Neo Game");

	Neo::LuaGameState* state = new Neo::LuaGameState();
	if(argc < 2)
		state->load("main.lua");
	else
		state->load(argv[1]);
	
	game.changeState(std::unique_ptr<Neo::LuaGameState>(state));
	return game.run(argc, argv);
}
