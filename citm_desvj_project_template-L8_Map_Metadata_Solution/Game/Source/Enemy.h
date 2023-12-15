#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "App.h"
#include "Animation.h"

enum class EnemyType
{
	WALKER,
	FLY,
	UNKNOWN
};

class Enemy : public Entity
{
public:
	Enemy();

	virtual ~Enemy();

	virtual bool Awake();

	virtual bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);
protected:
	SDL_Rect* tecture;
	int lives;
};

#endif // __ENEMY_H__