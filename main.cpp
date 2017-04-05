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

	void Set( int mx, int my )
       	{
		x = mx;
		y = my;
	}
	void Update( int ux, int uy )
	{
		x += ux;
		y += uy;
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

	int angleOffset;

	int lastPathStartTime = 0;
	int remainingTime;

	int pathIter = 0;
	std::vector< path_t > path;
	path_t *curPath = nullptr;

	void NextPath()
	{
		pathIter++;
		if( pathIter >= path.size() )
			pathIter = 0;

		curPath = &( path[pathIter] );

		if(  curPath->type == 'c' )
		{
			curPath->circle.FindRadius( pos );
			curPath->angle = curPath->circle.FindAngle( curPath->speed );
		}
	}
	void Update( point_t offset )
	{
		pos.Update( offset.x, offset.y );
	}
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
	point_t pos;
	int pathIter = 0;
	std::vector< path_t > path;
	path_t *curPath = &path[0];

	int angleOffset = 0;

	int chanePathTime;
	int interval;
	int localTime = 0;
	int numBulletsToSpawn;
	int spawnedBullets = 0;
	bullet_t bulletBlueprint;

	void NextPath()
	{
		pathIter++;
		if( pathIter >= path.size() )
			pathIter = 0;

		curPath = &( path[pathIter] );
	}

	bullet_t CreateBullet()
	{
		bullet_t bullet = bulletBlueprint;
		bullet.Update( pos );
		if( angleOffset != 0 )
			for( int i=0;i<bullet.path.size();i++ )
				bullet.path[i].angle += angleOffset;

		return bullet;
	}

	void Reset()
	{
		spawnedBullets = 0;
		localTime = 0;
	}

	spawner_t( int mx, int my, int myInterval, int num )
	{
		pos.Set( mx, my );

		interval = myInterval;
		numBulletsToSpawn = num;
	}
};

void DrawCircle( SDL_Renderer *renderer, point_t point, int r )
{
	for( int i=0;i<360;i++ )
		SDL_RenderDrawLine( renderer, point.x+std::sin(i)*r, point.y+std::cos(i)*r, point.x-std::sin(i)*r, point.y-std::cos(i)*r );
}

void Move( point_t &pos, path_t pth )
{
	double cos = std::cos( (double)pth.angle*M_PI/180  );
	double sin = std::sin( (double)pth.angle*M_PI/180  );

	if( pth.type == 'l' )
	{
		int updateX = (int)(pth.speed*cos);
		int updateY = (int)(pth.speed*sin);
		pos.Update( updateX, updateY );
	}
	/*
	if( pth.type == 'c' )
	{
		x = pth.circle.center.x + ( ( ( bullt.pos.x - pth.circle.center.x )*cos ) - ( ( bullt.pos.y - pth.circle.center.y )*sin ) );
		y = pth.circle.center.y + ( ( ( bullt.pos.x - pth.circle.center.x )*sin ) + ( ( bullt.pos.y - pth.circle.center.y )*cos ) );
	}
	bullt.Set( x, y );
	*/
}

std::vector< spawner_t > spawner;
std::vector< bullet_t > bullets;

const int BULLET_RADIUS = 5;

int main()
{

	char type;
	int parameters[50];
	int numParameters = 0;
	bool doneInput = false;

	std::vector< path_t > *pushInP = nullptr;

	while( std::cin )
	{
		std::cin>>type;
		if( type == 'b' )
		{
			int x, y, time;
			std::cin>>x>>y>>time;
			bullet_t newBullet( x, y, time );
			spawner[ spawner.size()-1 ].bulletBlueprint = newBullet;
			pushInP = &( spawner[ spawner.size()-1 ].bulletBlueprint.path );
		}
		if( type == 's' )
		{
			int x, y;
			int interval, numBullets;
			std::cin>>x>>y>>interval>>numBullets;
			spawner_t newSpawner( x, y, interval, numBullets );
			spawner.push_back( newSpawner );
			pushInP = &(spawner[ spawner.size()-1 ].path);
		}
		if( type == 'p' )
		{
			char pathType;
			int time, angle, speed;
			int x, y;

			std::cin>>pathType;
			if( pathType == 'c' )
				std::cin>>x>>y>>speed>>angle>>time;
			if( pathType == 'l' )
				std::cin>>speed>>angle>>time;

			path_t newPath( pathType, speed, time, angle, x, y );
			pushInP->push_back( newPath );
		}
		type = 0;
	}
	for( int i=0;i<spawner.size();i++ )
		spawner[i].curPath = &spawner[i].path[0];

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
				if( spawner[i].numBulletsToSpawn > spawner[i].spawnedBullets )
				{
					if( spawner[i].localTime >= spawner[i].interval )
					{
						spawner[i].localTime = 0;
						bullets.push_back( spawner[i].CreateBullet() );
						bullets[ bullets.size()-1 ].curPath = &bullets[ bullets.size()-1 ].path[0];
						spawner[i].spawnedBullets++;
					}
				}
				spawner[i].localTime++;
				if( !spawner[i].path.empty() )
				{
					Move( spawner[i].pos, *spawner[i].curPath );
					spawner[i].chanePathTime--;
					if( spawner[i].chanePathTime <= 0 )
					{
						spawner[i].NextPath();
						spawner[i].chanePathTime = spawner[i].curPath->time;
					}
				}
			}
			for( int i=0;i<bullets.size();i++ )
			{
				Move( bullets[i].pos, *bullets[i].curPath );

				if( bullets[i].lastPathStartTime >= bullets[i].curPath->time )
				{
					bullets[i].lastPathStartTime = 0;
					bullets[i].NextPath();
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

		SDL_RenderPresent( renderer );
		SDL_RenderClear( renderer );
	}
	SDL_Quit();
	return 0;
}
