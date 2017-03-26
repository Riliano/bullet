#include<iostream>
#include<SDL2/SDL.h>
#include<cmath>
#include<vector>
#include<thread>
#include<sstream>

SDL_Window *window;
SDL_Event e;
int scrWdth = 640;
int scrHght = 480;

struct path_t
{
	int speed;
	int time;
	double angle;
};

struct bullet_t
{
	int startX;
	int startY;
	int x;
	int y;
	std::vector< path_t > path;

	void Reset()
	{
		x = startX;
		y = startY;
	}

	bullet_t( int myX, int myY )
	{
		startX = myX;
		startY = myY;
		Reset();
	}
};

void Input( char *type, int *info, int *infoSize, bool *done )
{
	std::string input;
	std::getline( std::cin, input );
	std::stringstream ss( input );

	ss>>*type;
	int i=0;
	while( ss )
	{
		ss>>info[i];
		i++;
	}
	*infoSize = i-1;
	*done = true;
}

std::vector< bullet_t > bullets;

int main()
{
	window = SDL_CreateWindow( "Bullet", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scrWdth, scrHght, SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	char type;
	int parameters[50];
	int numParameters = 0;
	bool doneInput = false;
	std::thread input( Input, &type, parameters, &numParameters, &doneInput );

	while( true )
	{
		while( SDL_PollEvent( &e ) )
		{
			if( e.type == SDL_QUIT )
			{
				//input.join();
				SDL_Quit();
				return 0;
			}
		}
		if( doneInput )
		{
			input.join();

			if( type == 'q' )
			{
				SDL_Quit();
				return 0;
			}
			if( type == 'b' )
			{
				bullets.push_back( { parameters[0], parameters[1] } );
			}
			if( type == 'a' )
			{
				bullets[ parameters[0] ].path.push_back( { parameters[1], parameters[2], parameters[3] } );
			}
			if( type == 'r' )
			{
				for( int i = parameters[1];i<bullets[ parameters[0] ].path.size()-1;i++ )
					bullets[ parameters[0] ].path[i] = bullets[parameters[0]].path[i+1];
				bullets[parameters[0]].path.pop_back();
			}
			if( type == 'd' )
			{
				for( int i = parameters[0];i<bullets.size()-1;i++ )
					bullets[i] = bullets[i+1];
				bullets.pop_back();
			}
			if( type == 'R' )
			{
				for( int i=0;i<bullets.size();i++ )
					bullets[i].Reset();
			}
			if( type == 'p' )
			{
				for( int i=0;i<bullets.size();i++ )
				{
					std::cout<<bullets[i].x<<" "<<bullets[i].y<<"\n";
					for( int j=0;j<bullets[i].path.size();j++ )
						std::cout<<"\t"<<bullets[i].path[j].angle<<" "<<bullets[i].path[j].speed<<" "<<bullets[i].path[j].time<<"\n";
				}
			}

			doneInput = false;
			input = std::thread( Input, &type, parameters, &numParameters, &doneInput );
		}
		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
	SDL_Quit();
	return 0;
}
