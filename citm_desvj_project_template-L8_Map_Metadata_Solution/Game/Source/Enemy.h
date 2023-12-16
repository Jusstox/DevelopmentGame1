#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "App.h"
#include "Animation.h"
#include "Textures.h"
#include "Physics.h"

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

	int getEnemyTileX();

	int getEnemyTileY();

	void chasePlayer(float dt);

	bool canChase(int dist);
protected:
	SDL_Texture* texture;
	int lives;
	Animation* currentAnimation = nullptr;
	PhysBody* pbody;
	iPoint initPosition;
	SDL_Texture* mouseTileTex;
	float chaseVelovity;

	int distChase;

	int TileX;
	int TileY;
	int PTileX;
	int PTileY;

	b2Vec2 velocity;
};

#endif // __ENEMY_H__