#pragma once
#include "engine.h"
#include "window.h"

class Game : public Window
{
	Engine engine;

public:
	Game();
	void render() override;
	void start();
};