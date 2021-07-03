#pragma once
#include "SDL.h"
#include <SDL2_framerate.h>
#include <cstdio>
#include <map>
#include <string>
#include <thread>

void checkSDLError( int return_code, const std::string &error_msg );
void checkSDLError( void *pointer, const std::string &error_msg );
void checkSDLError( bool ok, const std::string &error_msg );

class Window
{
protected:
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	SDL_Event event{};
	std::map<Uint32, std::function<void( SDL_Event *const )>> event_handlers;
	volatile bool quit = false;
	FPSmanager fps_manager{};
	void renderLoop();

public:
	Window( int width, int height );
	~Window();
	void setEventHandler( SDL_EventType event_type, std::function<void( SDL_Event * )> handler );
	virtual void render() = 0;
	void show();
};