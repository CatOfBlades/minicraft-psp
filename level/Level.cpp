/*
 * Level.cpp
 *
 *  Created on: 16 ���. 2013 �.
 *      Author: user
 */

#include <stdlib.h>

#include "Level.h"
#include "LevelGen.h"
#include "tile/Tile.h"
#include "../entity/Player.h"
#include "../entity/Slime.h"
#include "../entity/Zombie.h"
#include "../entity/AirWizard.h"
#include <oslib/oslib.h>

Level::~Level()
{
	// TODO Auto-generated destructor stub
}

//	private Comparator<Entity> spriteSorter = new Comparator<Entity>() {
//		public int compare(Entity e0, Entity e1) {
//			if (e1.y < e0.y) return +1;
//			if (e1.y > e0.y) return -1;
//			return 0;
//		}


Level::Level(int w, int h, int level, Level * parentLevel) :
grassColor(141),
dirtColor(322),
sandColor(550),
monsterDensity(8),
player(NULL),
random(new Random())
{
	if (level < 0) {
		dirtColor = 222;
	}
	this->depth = level;
	this->w = w;
	this->h = h;
	ushort ** maps;

	if (level == 1) {
		dirtColor = 444;
	}

	if (level == 0)
	{

		maps = LevelGen::createAndValidateTopMap(w, h);
	}
	else if (level < 0)
	{
		maps = LevelGen::createAndValidateUndergroundMap(w, h, -level);
		monsterDensity = 4;
	} else {
		maps = LevelGen::createAndValidateSkyMap(w, h); // Sky level
		monsterDensity = 4;
	}
	tiles = maps[0];
	data = maps[1];

	if (parentLevel != NULL) {
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++) {
				if (parentLevel->getTile(x, y) == Tile::stairsDown)
				{
					setTile(x, y, Tile::stairsUp, 0);
					if (level == 0) {
						setTile(x - 1, y, Tile::hardRock, 0);
						setTile(x + 1, y, Tile::hardRock, 0);
						setTile(x, y - 1, Tile::hardRock, 0);
						setTile(x, y + 1, Tile::hardRock, 0);
						setTile(x - 1, y - 1, Tile::hardRock, 0);
						setTile(x - 1, y + 1, Tile::hardRock, 0);
						setTile(x + 1, y - 1, Tile::hardRock, 0);
						setTile(x + 1, y + 1, Tile::hardRock, 0);
					} else {
						setTile(x - 1, y, Tile::dirt, 0);
						setTile(x + 1, y, Tile::dirt, 0);
						setTile(x, y - 1, Tile::dirt, 0);
						setTile(x, y + 1, Tile::dirt, 0);
						setTile(x - 1, y - 1, Tile::dirt, 0);
						setTile(x - 1, y + 1, Tile::dirt, 0);
						setTile(x + 1, y - 1, Tile::dirt, 0);
						setTile(x + 1, y + 1, Tile::dirt, 0);
					}
				}

			}
	}

	entitiesInTiles = new list<Entity*>[w * h];
	//for (int i = 0; i < w * h; i++)
	//{
	//	entitiesInTiles[i] = new list<Entity*>;
	//}

	if (level == 1)
	{
		AirWizard * aw = new AirWizard();
		aw->x = w*8;
		aw->y = h*8;
		add(aw);
	}
}

	void Level::renderBackground(Screen * screen, int xScroll, int yScroll)
	{
		int xo = xScroll >> 4;
		int yo = yScroll >> 4;
		int w = (screen->w + 15) >> 4;
		int h = (screen->h + 15) >> 4;
		screen->setOffset(xScroll, yScroll);
		for (int y = yo; y <= h + yo; ++y)
		{
			for (int x = xo; x <= w + xo; ++x)
			{
				getTile(x, y)->render(screen, this, x, y);
			}
		}
		screen->setOffset(0, 0);
	}

	void Level::renderSprites(Screen * screen, int xScroll, int yScroll) {
		int xo = xScroll >> 4;
		int yo = yScroll >> 4;
		int w = (screen->w + 15) >> 4;
		int h = (screen->h + 15) >> 4;

		screen->setOffset(xScroll, yScroll);
		for (int y = yo; y <= h + yo; ++y)
		{
			if (y < 0 || y >= this->h) continue;
			for (int x = xo; x <= w + xo; ++x)
			{
				if (x < 0 || x >= this->w) continue;
				//rowSprites.splice(rowSprites.end(),entitiesInTiles[x + y * this->w]);
				list<Entity*> &ent = entitiesInTiles[x + y * this->w];
				if (!ent.empty())
				{
					for (list<Entity*>::iterator it = ent.begin(); it != ent.end(); it++ )
					{
						Entity * e = *it;
						e->render(screen);
					}
				}
			}
//			for (list<Entity*>::iterator it = rowSprites.begin(); it != rowSprites.end(); it++ )
//			{
//				Entity * e = *it;
//				e->render(screen);
//			}
			//if (rowSprites.size() > 0) {
			//	sortAndRender(screen, rowSprites);
			//}
			rowSprites.clear();
		}
		screen->setOffset(0, 0);
	}

	void Level::renderLight(Screen * screen, int xScroll, int yScroll) {
		int xo = xScroll >> 4;
		int yo = yScroll >> 4;
		int w = (screen->w + 15) >> 4;
		int h = (screen->h + 15) >> 4;

		screen->setOffset(xScroll, yScroll);
		int r = 4;
		for (int y = yo - r; y <= h + yo + r; y++) {
			for (int x = xo - r; x <= w + xo + r; x++) {
				if (x < 0 || y < 0 || x >= this->w || y >= this->h) continue;
				list<Entity*> &entities = entitiesInTiles[x + y * this->w];
				for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); it++ )
				{
					Entity * e = *it;
					//Entity * e = entities.at(i);
					// e.render(screen);
					int lr = e->getLightRadius();
					if (lr > 0) screen->renderLight(e->x - 1, e->y - 4, lr * 8);
				}
				int lr = getTile(x, y)->getLightRadius(this, x, y);
				if (lr > 0) screen->renderLight(x * 16 + 8, y * 16 + 8, lr * 8);
			}
		}
		screen->setOffset(0, 0);
	}

//void renderLight(Screen * screen, int x, int y, int r)
//{
//	screen->renderLight(x, y, r);
//}

//	void Level::sortAndRender(Screen * screen, List<Entity> list) {
//		Collections.sort(list, spriteSorter);
//		for (int i = 0; i < list.size(); i++) {
//			list.get(i).render(screen);
//		}
//	}

Tile * Level::getTile(int x, int y)
{
	if (x < 0 || y < 0 || x >= w || y >= h) return Tile::rock;
	return Tile::tiles[tiles[x + y * w]];
}

void Level::setTile(int x, int y, Tile * t, int dataVal) {
	if (x < 0 || y < 0 || x >= w || y >= h) return;
	tiles[x + y * w] = t->id;
	data[x + y * w] = (ushort) dataVal;
}

int Level::getData(int x, int y) {
	if (x < 0 || y < 0 || x >= w || y >= h) return 0;
	return data[x + y * w] & 0xff;
}

void Level::setData(int x, int y, int val) {
	if (x < 0 || y < 0 || x >= w || y >= h) return;
	data[x + y * w] = (ushort) val;
}

void Level::add(Entity * entity)
{
	if (entity->instanceOf(PLAYER))
	{
		player = (Player*)entity;
	}
	entity->removed = false;
	entities.push_back(entity);
	entity->init(this);

	insertEntity(entity->x >> 4, entity->y >> 4, entity);
	//setTile(entity->x >> 4, (entity->y >> 4) + 1, Tile::stairsDown,0);
}

void Level::remove(Entity * e)
{
	entities.remove(e);
	int xto = e->x >> 4;
	int yto = e->y >> 4;
	removeEntity(xto, yto, e);
}

void Level::insertEntity(int x, int y, Entity * e) {
	if (x < 0 || y < 0 || x >= w || y >= h) return;
	entitiesInTiles[x + y * w].push_back(e);
}

void Level::removeEntity(int x, int y, Entity * e)
{
	if (x < 0 || y < 0 || x >= w || y >= h) return;
	entitiesInTiles[x + y * w].remove(e);
}

void Level::trySpawn(int count)
{
	for (int i = 0; i < count; i++)
	{
		Mob * mob;

		int minLevel = 1;
		int maxLevel = 1;
		if (depth < 0)
		{
			maxLevel = (-depth) + 1;
		}
		if (depth > 0)
		{
			minLevel = maxLevel = 4;
		}
		int lvl = random->nextInt(maxLevel - minLevel + 1) + minLevel;
		if (random->nextInt(2) == 0)
			mob = new Slime(lvl);
		else
			mob = new Zombie(lvl);

		if (!mob->findStartPos(this))
		{
			delete mob;
		}
		else
		{
			this->add(mob);
		}
	}
}

void Level::tick()
{
	trySpawn(1);

	int tilesTickAttempts = w * h / 50;
	for (int i = 0; i < tilesTickAttempts; ++i)
	{
		int xt = random->nextInt(w);
		int yt = random->nextInt(h);
		getTile(xt, yt)->tick(this, xt, yt);
	}
	for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); it++ )
	{
		Entity * e = *it;
		int xto = e->x >> 4;
		int yto = e->y >> 4;

		e->tick();

		if (e->removed) {
			it = entities.erase(it);
			removeEntity(xto, yto, e);
		} else {
			int xt = e->x >> 4;
			int yt = e->y >> 4;

			if (xto != xt || yto != yt) {
				removeEntity(xto, yto, e);
				insertEntity(xt, yt, e);
			}
		}
	}
}

list<Entity*> * Level::getEntities(int x0, int y0, int x1, int y1)
{
	list<Entity*> * result = new list<Entity*>;
	int xt0 = (x0 >> 4) - 1;
	int yt0 = (y0 >> 4) - 1;
	int xt1 = (x1 >> 4) + 1;
	int yt1 = (y1 >> 4) + 1;
	for (int y = yt0; y <= yt1; ++y)
	{
		if (y < 0 || y >= h) continue;
		int yByWidth = y * this->w;
		for (int x = xt0; x <= xt1; ++x)
		{
			if (x < 0  || x >= w) continue;
			list<Entity*> &entities = entitiesInTiles[x + yByWidth];
			if (entities.empty()) continue;
			for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); it++ )
			{
				Entity * e = *it;
				if (e->intersects(x0, y0, x1, y1)) result->push_back(e);
			}
		}
	}
	return result;
}

bool Level::isRegionEmpty(int x0, int y0, int x1, int y1)
{
	int xt0 = (x0 >> 4) - 1;
	int yt0 = (y0 >> 4) - 1;
	int xt1 = (x1 >> 4) + 1;
	int yt1 = (y1 >> 4) + 1;
	for (int y = yt0; y <= yt1; ++y)
	{
		if (y < 0 || y >= h) continue;
		int yByWidth = y * this->w;
		for (int x = xt0; x <= xt1; ++x)
		{
			if (x < 0  || x >= w) continue;
			list<Entity*> &entities = entitiesInTiles[x + yByWidth];
			if (entities.empty()) continue;
			for (list<Entity*>::iterator it = entities.begin(); it != entities.end(); it++ )
			{
				Entity * e = *it;
				if (e->intersects(x0, y0, x1, y1)) return false;
			}
		}
	}
	return true;
}

