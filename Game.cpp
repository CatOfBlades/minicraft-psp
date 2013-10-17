#define HEIGHT 90
#define WIDTH  160
#define SCALE 3

#define SCALED_WIDTH  480
#define SCALED_HEIGHT 270

#include <stdlib.h>
#include <ctime>
#include <math.h>

#include <oslib/oslib.h>
#include <sstream>

#include "Font.h"
#include "Color.h"
#include "Game.h"

Game::Game()
//xScroll(0),
//yScroll(0)
{
	image = oslCreateImage(WIDTH * SCALE, HEIGHT *SCALE, OSL_IN_RAM, OSL_PF_8888);
	//renderImage = oslCreateImage(WIDTH * SCALE, HEIGHT * SCALE, OSL_IN_RAM, OSL_PF_8888);

	oslClearImage(image,RGBA(0,128,128,200));
	colors = new int[256];
	pixels = (int*)image->data;
	input = new InputHandler();
	run();
}

Game::~Game() {
	delete[] colors;
}

void Game::render()
{
	//sceGuStart(GU_DIRECT,list);

	oslLockImage(image);
	//BufferStrategy bs = getBufferStrategy();
	//if (bs == null) {
	//	createBufferStrategy(3);
	//	requestFocus();
	//	return;
	//}


	int xScroll = player->x - screen->w / 2;
	int yScroll = player->y - (screen->h + 8) / 2;
	if (xScroll < 16) xScroll = 16;
	if (yScroll < 16) yScroll = 16;
	if (xScroll > level->w * 16 - screen->w - 16) xScroll = level->w * 16 - screen->w - 16;
	if (yScroll > level->h * 16 - screen->h - 16) yScroll = level->h * 16 - screen->h - 16;
	//if (currentLevel > 3) {
	int col = Color::get(20, 20, 121, 121);
		for (int y = 0; y < 14; y++)
			for (int x = 0; x < 24; x++) {
				screen->render(x * 8 - ((xScroll / 4) & 7), y * 8 - ((yScroll / 4) & 7), 0, col, 0);
			}
	//}
	//oslDebug("%d %d",player->x, player->y);

	level->renderBackground(screen, xScroll, yScroll);
	level->renderSprites(screen, xScroll, yScroll);

	//if (currentLevel < 3) {
//		lightScreen->clear(0);
//		level->renderLight(lightScreen, xScroll, yScroll);
//		screen->overlay(lightScreen, xScroll, yScroll);
	//}

//	renderGui();

//	if (!hasFocus()) renderFocusNagger();


	//memset((void*)pixels, RGBA(128,255,255,255), screen->w * 4);
//	for (int y = 0; y < screen->h; ++y)
//	{
////		memcpy((int*)(pixels + y * image->realSizeX),
////			   (int*)(screen->pixels + y * screen->w),
////			    screen->w * 4);
////	}
//		for (int x = 0; x < screen->w; ++x)
//		{
//			int cc = screen->pixels[x + y * screen->w];
//			if (cc < 255) pixels[x + y * image->realSizeX] = colors[cc];
//		}
//	}
	std::stringstream ss;
	ss << oslBenchmarkTest(OSL_BENCH_GET);
	Font::draw(ss.str(),screen,0,0,Color:: get(5, 555, 555, 555));

	//nearest neighbor to scale
	//and to move from screen to image
	//somewhat optimized
	int x_ratio = (int)((screen->w<<16)/(SCALED_WIDTH)) +1;
	int y_ratio = (int)((screen->h<<16)/(SCALED_HEIGHT)) +1;
	int x = -1, y = -1, prevX = -1;
	int cc;
	int * scrPixels = screen->pixels;
	for (int i = 0; i<SCALED_HEIGHT; ++i)
	{
		y = ((i*y_ratio)>>16);
		int yByScreenWidth = y * screen->w;
		int iByImageWidth = i * image->realSizeX;
		for (int j = 0; j<SCALED_WIDTH; ++j)
		{
			x = ((j*x_ratio)>>16);
			if (x != prevX)
			{
				cc = colors[scrPixels[yByScreenWidth+x]];
				prevX = x;
			}
			if (cc < 255)
			{
				pixels[iByImageWidth+j] = cc;
			}
		}
	}

	oslUnlockImage(image);

	oslStartDrawing();
	oslDrawImageSimpleXY(image,0,0);
	oslEndDrawing();

}

void Game::init()
{
		int pp = 0;
		for (int r = 0; r < 6; r++) {
			for (int g = 0; g < 6; g++) {
				for (int b = 0; b < 6; b++) {
					int rr = (r * 255 / 5);
					int gg = (g * 255 / 5);
					int bb = (b * 255 / 5);
					int mid = (rr * 30 + gg * 59 + bb * 11) / 100;

					int r1 = ((rr + mid * 1) / 2) * 230 / 255 + 10;
					int g1 = ((gg + mid * 1) / 2) * 230 / 255 + 10;
					int b1 = ((bb + mid * 1) / 2) * 230 / 255 + 10;
					colors[pp++] = 0xff << 24 | b1 << 16 | g1 << 8 | r1;
				}
			}
		}
		//oslDebug("color %X",Color::get(555,222,141,444));
		//try {
		/*OSL_IMAGE * */spriteSheet = oslLoadImageFileGIF("icons.gif", OSL_IN_RAM | OSL_UNSWIZZLED, OSL_PF_8888);
		screen = new Screen(WIDTH, HEIGHT, new SpriteSheet(spriteSheet));
		lightScreen =  new Screen(WIDTH, HEIGHT, new SpriteSheet(spriteSheet));
		//oslDeleteImage(spriteSheet);

		screen->clear(Color::get(000, 200, 500, 533));
		//screen->clear(Color::get(5, 333, 333, 333));
		//screen->render(0, 96, 3  + 10 * 32, Color::get(5, 333, 333, 333), 0);
		level = new Level(128, 128, 0, NULL);
		player = new Player(this, input);
		player->findStartPos(level);
		level->add(player);

		running = true;
		//resetGame();
		//setMenu(new TitleMenu());
	}

void Game::run()
{
	long lastTime = time(0);
	//double unprocessed = 0;
	//double nsPerTick = 1000000000.0 / 60;
	int frames = 0;
	int ticks = 0;
	//long lastTimer1 = System.currentTimeMillis();
	bool shouldNotRender = false;
	init();

	while (running)
	{
		oslBenchmarkTest(OSL_BENCH_START);
		unsigned long now = time(0);
		//unprocessed += (now - lastTime) / nsPerTick;
		//lastTime = now;
		//bool shouldRender = true;
		//while (unprocessed >= 1) {
			ticks++;
			tick();
		//	unprocessed -= 1;
		//}

		//double negotiation, sorry
		if (!shouldNotRender)
		{
			frames++;
			//oslDebug("about to render");
			render();
			//oslDebug("rendered!");
		}
		if (lastTime - now >= 1)
		{
			lastTime = now;
			//oslPrintf(frames + " fps");
			frames = 0;
			ticks = 0;
		}
		oslEndFrame();
		oslBenchmarkTest(OSL_BENCH_END);
		shouldNotRender = oslSyncFrame();


		input->handleKeys(oslReadKeys());
		if (osl_keys->released.circle)
					oslQuit();
	}

}

void Game::tick()
{
	input->tick();
	level->tick();
	Tile::tickCount++;
}
