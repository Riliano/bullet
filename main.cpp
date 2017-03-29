#include<iostream>
#include<SDL2/SDL.h>
#include<cmath>
#include<vector>
#include<thread>
#include<sstream>
#include<cmath>

SDL_Window *window;
SDL_Event e;
int scrWdth = 640;
int scrHght = 480;

struct path_t
{
	int speed;
	int time;
	int angle;
};

struct bullet_t
{
	int startX;
	int startY;
	int x;
	int y;
	int pathIter = 0;
	int lastPathStartTime = 0;
	std::vector< path_t > path;

	void Set( int myX, int myY )
	{
		x = myX;
		y = myY;
	}

	void Reset()
	{
		x = startX;
		y = startY;
		lastPathStartTime = 0;
		pathIter = 0;
	}

	bullet_t( int myX, int myY )
	{
		startX = myX;
		startY = myY;
		Set( myX, myY );
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

void DrawCircle( SDL_Renderer *renderer, int x, int y, int r )
{
	for( int i=0;i<360;i++ )
	{
		SDL_RenderDrawLine( renderer, x+std::sin(i)*r, y+std::cos(i)*r, x-std::sin(i)*r, y-std::cos(i)*r );
	}
//	SDL_RenderDrawLine( renderer, x, y+r, x, y-r );
//	SDL_RenderDrawLine( renderer, x+r, y, x-r, y );
}

std::vector< bullet_t > bullets;

const int BULLET_RADIUS = 5;

int main()
{

	char type;
	int parameters[50];
	int numParameters = 0;
	bool doneInput = false;

	while( std::cin )
	{
		std::cin>>type;
		if( type == 'b' )
		{
			int x, y;
			std::cin>>x>>y;
			bullet_t newBullet( x, y );
			bullets.push_back( newBullet );
		}
		if( type == 'p' )
		{
			int own;
			std::cin>>own;
			int time, angle, speed;
			std::cin>>speed>>angle>>time;
			bullets[own].path.push_back( {speed, time, angle} );
		}
		type = 0;
	}

//	std::thread input( Input, &type, parameters, &numParameters, &doneInput );

	window = SDL_CreateWindow( "Bullet", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scrWdth, scrHght, SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	long long time = 0;
	long long T = SDL_GetTicks();

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
			if( e.type == SDL_KEYDOWN )
			{
				for( int i=0;i<bullets.size();i++ )
					bullets[i].Reset();
			}
		}
		if( SDL_GetTicks() - T >= 10 )
		{
			for( int i=0;i<bullets.size();i++ )
			{
				double cos = std::cos( (double)bullets[i].path[ bullets[i].pathIter ].angle*M_PI/180  );
				double sin = std::sin( (double)bullets[i].path[ bullets[i].pathIter ].angle*M_PI/180  );

				int newX = bullets[i].path[ bullets[i].pathIter ].speed;
				int newY = 0;

				bullets[i].Set( bullets[i].x + (int)(newX*cos), bullets[i].y + (int)(newX*sin) );
				if( bullets[i].lastPathStartTime >= bullets[i].path[ bullets[i].pathIter ].time )
				{
					bullets[i].lastPathStartTime = 0;
					bullets[i].pathIter++;
					if( bullets[i].pathIter >= bullets[i].path.size() )
						bullets[i].pathIter = 0;
				}
				bullets[i].lastPathStartTime++;
			}
			time++;
			T = SDL_GetTicks();
		}
		for( int i=0;i<bullets.size();i++ )
			DrawCircle( renderer, bullets[i].x, bullets[i].y, BULLET_RADIUS );

		if( doneInput )
		{
//			input.join();

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
//			input = std::thread( Input, &type, parameters, &numParameters, &doneInput );
		}
		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
	SDL_Quit();
	return 0;
}
