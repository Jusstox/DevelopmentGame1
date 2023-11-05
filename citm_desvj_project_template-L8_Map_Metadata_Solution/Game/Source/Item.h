#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;

private:

	//L07 DONE 4: Add a physics to an item
	PhysBody* pbody;
	SDL_Texture* texture;
	const char* texturePath;
	uint texW, texH;

};

#endif // __ITEM_H__