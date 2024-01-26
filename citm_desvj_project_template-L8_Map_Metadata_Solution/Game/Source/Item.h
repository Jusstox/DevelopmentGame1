#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Physics.h"

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

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void Respawn();

	void MoveAway();

public:

	bool isPicked = false;

private:

	//L07 DONE 4: Add a physics to an item
	PhysBody* pbody = NULL;
	b2Transform inpos;
	SDL_Texture* texture;
	const char* texturePath;
	uint texW, texH;
	bool hit;

};

#endif // __ITEM_H__