//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "Objects.h"
#include "matrix.h"
#include "vec2.h"
#include <SDL2_gfxPrimitives.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The renderer we render to
SDL_Renderer* renderer = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gXOut = NULL;

vec2<int> mousePosition;

int main(int argc, char* args[])
{

	OBB testOBB(vec2<float>(0, 0), vec2<float>(0, 0), 1, 100, 50, 0.5);
	circle testCircle(vec2<float>(200, 200), vec2<float>(0, 0), 1, 50);
	lineSegment testLineSeg(vec2<float>(0, 400), vec2<float>(40, 450));

	vec2<float> pointCentre(2, 2);
	lineSegment distanceTestLineSeg(vec2<float>(2,1),vec2<float>(3,3));

	vec2<float> poi = line(distanceTestLineSeg).collisionPoint(line(vec2<float>(-2, 1), vec2<float>(2, 2)));
	
	int result;

	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}

				//Get mouse pos
				SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

				//Clear renderer
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderClear(renderer);

				//Move OBB

				testOBB.moveTo(mousePosition.x, mousePosition.y);


				//Draw circle
				testCircle.draw(renderer);


				//Draw obb
				testOBB.draw(renderer);

				//Test collision
				//testOBB.collide(testCircle);
				std::cout <<std::boolalpha<< testOBB.collide(testCircle) << std::endl;

				//Move line segment
				//testLineSeg.moveTo(mousePosition.x, mousePosition.y);
				//Draw line segment
				//testLineSeg.draw(renderer);

				//std::cout << testLineSeg.distanceToPoint(testCircle.pos) << std::endl;

				//Update renderer
				SDL_RenderPresent(renderer);

				
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);

			//Renderer setup
			renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	gXOut = SDL_LoadBMP("Images/x.bmp");
	if (gXOut == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", "Images/x.bmp", SDL_GetError());
		success = false;
	}

	return success;
}

void close()
{
	//Deallocate surface
	SDL_FreeSurface(gXOut);
	gXOut = NULL;

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

