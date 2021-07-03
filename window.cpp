#include "window.h"
#include <iostream>
#include <utility>

void checkSDLError( void *pointer, const std::string &error_msg )
{
	checkSDLError( pointer != nullptr, error_msg );
}
void checkSDLError( int return_code, const std::string &error_msg )
{
	checkSDLError( return_code >= 0, error_msg );
}
void checkSDLError( bool ok, const std::string &error_msg )
{
	if ( !ok ) {
		fputs( error_msg.c_str(), stderr );
		fputs( " SDL_Error: ", stderr );
		fputs( SDL_GetError(), stderr );
		fputs( "\n", stderr );
		exit( -1 );
	}
}

void Window::renderLoop()
{
	while ( !quit ) {
		while ( SDL_PollEvent( &event ) ) {
			auto it = event_handlers.find( event.type );
			if ( it != event_handlers.end() ) {
				( it->second )( &event );
			}
			if ( event.type == SDL_QUIT ) {
				quit = true;
			}
		}
		SDL_SetRenderDrawColor( renderer, 0xFF,0xFF,0xFF,0xFF );
		SDL_RenderClear( renderer );
		render();
		SDL_RenderPresent( renderer );
	}
}
Window::Window( int width, int height )
{
	checkSDLError( SDL_Init( SDL_INIT_VIDEO ), "SDL Init Failed" );
	window = SDL_CreateWindow( "The Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0 );
	checkSDLError( window, "Window Init Failed" );
	renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	checkSDLError( renderer, "Renderer Creation Failed" );
	SDL_initFramerate( &fps_manager );
}
Window::~Window()
{
	quit = true;
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	renderer = nullptr;
	window = nullptr;
	SDL_Quit();
}
void Window::setEventHandler( SDL_EventType event_type, std::function<void( SDL_Event *const )> handler )
{
	event_handlers[event_type] = std::move( handler );
}
void Window::show()
{
	SDL_ShowWindow( window );
	renderLoop();
}
