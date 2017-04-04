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

struct point_t
{
	int x;
	int y;

	void Set(int mx, int my)
       	{
		x = mx;
		y = my;
	}
	point_t( int myX, int myY )
	{
		Set(myX, myY);
	}
	point_t()
	{}
};

int Distance( point_t a, point_t b )
{
	int dist = (b.y-a.y)*(b.y-a.y) + (b.x-a.x)*(b.x-a.x);
	dist = std::sqrt( dist );
	return dist;
}

struct circle_t
{
	point_t center;
	int radius;

	void FindRadius( point_t point )
	{
		radius = Distance( point, center );
	}

	int FindAngle( int speed )
	{
		double perimeter = 4*M_PI*radius;
		int angle = (double)(speed/perimeter) * 360;
		return angle;
	}

	void Set( int mX, int mY )
	{
		center.Set( mX, mY );
	}
};

struct path_t
{
	int speed;
	int time;
	int angle;
	char type;

	circle_t circle;

	void Set( int ms, int mt, int ma )
	{
		speed = ms;
		time = mt;
		angle = ma;
	}

	path_t( char mType, int s, int t , int a, int x, int y )
	{
		type = mType;
		Set( s, t, a );
		if( type == 'c' )
		{
			circle.Set( x, y );
		}
	}
};

struct bullet_t
{
	point_t start;
	point_t pos;
	int pathIter = 0;
	int lastPathStartTime = 0;
	int remainingTime;
	std::vector< path_t > path;

	void Set( int myX, int myY )
	{
		pos.Set( myX, myY );
	}

	void Reset()
	{
		pos = start;
		lastPathStartTime = 0;
		pathIter = 0;
	}

	bullet_t( int myX, int myY, int time )
	{
		Set( myX, myY );
		start = pos;
		remainingTime = time;
	}
	bullet_t()
	{}
};
struct spawner_t
{
	int interval;
	int localTime = 0;
	int numBulletsToSpawn;
	int spawnedBullets = 0;
	bullet_t bullet;

	void Reset()
	{
		spawnedBullets = 0;
		localTime = 0;
	}

	spawner_t( bullet_t bulletBlueprint, int myInterval, int num )
	{
		bullet = bulletBlueprint;
		interval = myInterval;
		numBulletsToSpawn = num;
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

void DrawCircle( SDL_Renderer *renderer, point_t point, int r )
{
	for( int i=0;i<360;i++ )
	{
		SDL_RenderDrawLine( renderer, point.x+std::sin(i)*r, point.y+std::cos(i)*r, point.x-std::sin(i)*r, point.y-std::cos(i)*r );
	}
//	SDL_RenderDrawLine( renderer, x, y+r, x, y-r );
//	SDL_RenderDrawLine( renderer, x+r, y, x-r, y );
}

void Move( bullet_t &bullt, path_t &pth )
{
	double cos = std::cos( (double)pth.angle*M_PI/180  );
	double sin = std::sin( (double)pth.angle*M_PI/180  );

	int x, y;
	if( pth.type == 'l' )
	{
		x = bullt.pos.x + (int)(pth.speed*cos);
		y = bullt.pos.y + (int)(pth.speed*sin);
	}
	if( pth.type == 'c' )
	{
		x = pth.circle.center.x + ( ( ( bullt.pos.x - pth.circle.center.x )*cos ) - ( ( bullt.pos.y - pth.circle.center.y )*sin ) );
		y = pth.circle.center.y + ( ( ( bullt.pos.x - pth.circle.center.x )*sin ) + ( ( bullt.pos.y - pth.circle.center.y )*cos ) );
	}
	bullt.Set( x, y );
}

std::vector< bullet_t > bullets;
std::vector< spawner_t > spawner;

const int BULLET_RADIUS = 5;

int main()
{

	char type;
	int parameters[50];
	int numParameters = 0;
	bool doneInput = false;

	std::vector< path_t > *pushIn = nullptr;

	while( std::cin )
	{
		std::cin>>type;
		if( type == 'b' )
		{
			int x, y, time;
			std::cin>>x>>y>>time;
			bullet_t newBullet( x, y, time );
			bullets.push_back( newBullet );
			pushIn = &(bullets[ bullets.size()-1 ].path);
		}
		if( type == 's' )
		{
			int x, y;
			int interval, numBullets, time;
			std::cin>>x>>y>>interval>>numBullets>>time;
			bullet_t newBullet( x, y, time );
			spawner_t newSpawner( newBullet, interval, numBullets );
			spawner.push_back( newSpawner );
			pushIn = &(spawner[ spawner.size()-1 ].bullet.path);
		}
		if( type == 'p' )
		{
			char pathType;
			int time, angle, speed;
			int x, y;

			std::cin>>pathType;
			if( pathType == 'c' )
				std::cin>>x>>y;

			std::cin>>speed>>angle>>time;

			path_t newPath( pathType, speed, time, angle, x, y );
			pushIn->push_back( newPath );
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
				for( int i=0;i<spawner.size();i++ )
					spawner[i].Reset();
			}
		}
		if( SDL_GetTicks() - T >= 10 )
		{
			for( int i=0;i<spawner.size();i++ )
			{
				if( spawner[i].numBulletsToSpawn > 0 )
				{
					if( spawner[i].localTime >= spawner[i].interval and spawner[i].spawnedBullets <= spawner[i].numBulletsToSpawn )
					{
						spawner[i].localTime = 0;
						bullets.push_back( spawner[i].bullet );
						spawner[i].spawnedBullets++;
					}
				}
				spawner[i].localTime++;
			}
			for( int i=0;i<bullets.size();i++ )
			{
				Move( bullets[i], bullets[i].path[ bullets[i].pathIter ] );

				if( bullets[i].lastPathStartTime >= bullets[i].path[ bullets[i].pathIter ].time )
				{
					bullets[i].lastPathStartTime = 0;
					bullets[i].pathIter++;
					if( bullets[i].pathIter >= bullets[i].path.size() )
						bullets[i].pathIter = 0;
					if(  bullets[i].path[ bullets[i].pathIter ].type == 'c' )
					{
						path_t *pth = &bullets[i].path[ bullets[i].pathIter ];
						pth->circle.FindRadius( bullets[i].pos );
						pth->angle = pth->circle.FindAngle( pth->speed );
					}
				}
				bullets[i].remainingTime--;
				bullets[i].lastPathStartTime++;
				if( bullets[i].remainingTime <= 0 )
				{
					std::swap( bullets[i], bullets[ bullets.size()-1 ] );
					bullets.pop_back();
					i--;
				}
			}
			time++;
			T = SDL_GetTicks();
		}
		for( int i=0;i<bullets.size();i++ )
			DrawCircle( renderer, bullets[i].pos, BULLET_RADIUS );
/*
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
*/
		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
	SDL_Quit();
	return 0;
}
