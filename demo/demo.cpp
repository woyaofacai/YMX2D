// demo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "demo.h"

YmxGame* game;

bool Game_Update(float delta);
void Game_Render(YmxGraphics* g);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	game = CreateGame();
	game->SetAttribute(GAME_WINDOW_WIDTH, 500);
	game->SetAttribute(GAME_WINDOW_HEIGHT, 500);
	game->SetUpdateFunction(Game_Update);
	game->SetRenderFunction(Game_Render);
	if(game->Initialize())
	{
		game->Start();
	}
	game->Release();
	return 0;
}

bool Game_Update(float delta)
{
	return true;
}

void Game_Render(YmxGraphics* g)
{
	g->Clear(0xFFFF00000);
}