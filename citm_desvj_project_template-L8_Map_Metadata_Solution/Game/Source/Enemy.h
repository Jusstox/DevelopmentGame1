#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "App.h"
#include "Animation.h"
#include "Textures.h"
#include "Physics.h"
#include "List.h"
#include "DynArray.h"

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

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool Draw();

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. 
	virtual void OnCollision(PhysBody* physA, PhysBody* physB);

	int getEnemyTileX();

	int getEnemyTileY();

	const DynArray<iPoint>* SearchWay();

	bool canChase(int dist);

	virtual void moveToPlayer(float dt);

	void Patrol();

	bool dark;

protected:
	const char* texturePath;
	SDL_Texture* texture = nullptr;
	int lives;
	Animation* currentAnimation = nullptr;
	iPoint initPosition;
	SDL_Texture* mouseTileTex = nullptr;
	float ActualVelocity;
	float chaseVelovity;
	float patrolVelocity;
	bool right;
	bool dead;
	bool hit;

	int distChase;
	iPoint dest;

	int TileX;
	int TileY;
	int PTileX;
	int PTileY;

	b2Vec2 velocity;
	
	iPoint Patrol1;
	iPoint Patrol2;
	bool patrol;

	int dieFX = -1;
	
};

#endif // __ENEMY_H__