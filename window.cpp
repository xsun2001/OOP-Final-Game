#include "window.h"
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

void Window::renderPass()
{
	while ( !quit ) {
		while ( SDL_PollEvent( event ) ) {
			auto it = event_handlers.find( event->type );
			if ( it != event_handlers.end() ) {
				( it->second )( event );
			}
			if ( event->type == SDL_QUIT ) {
				quit = true;
			}
		}
		render();
	}
}
Window::Window( int width, int height )
{
	checkSDLError( SDL_Init( SDL_INIT_VIDEO ), "SDL Init Failed" );
	window = SDL_CreateWindow( "The Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
	checkSDLError( window, "Window Init Failed" );
	screen = SDL_GetWindowSurface( window );
	render_thread = std::thread( &Window::renderPass, this );
}
Window::~Window()
{
	quit = true;
	render_thread.join();
	SDL_DestroyWindow( window );
	SDL_Quit();
}
void Window::setEventHandler( SDL_EventType event_type, std::function<void( SDL_Event *const )> handler )
{
	event_handlers[event_type] = std::move( handler );
}
