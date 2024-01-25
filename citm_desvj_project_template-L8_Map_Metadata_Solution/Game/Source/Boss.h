#pragma once
#include "Enemy.h"
class Boss :  public Enemy
{
public:
	Boss();
	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool LoadState(pugi::xml_node& node);

	bool SaveState(pugi::xml_node& node);

	void Respawn();

	bool Draw();

	const char* texturePatha;
	Animation flyinganim;
	Animation atackanim;
	bool atacking = false;
	int lives = 30;

	b2Transform inpos;
	int p1 = 0;
	int p2 = 0;
	int p3 = 0;
	int pawnsdead = 0;

	int fxdark = -1;
	int fxatack = -1;
	bool fxatackplayed = false;

	PhysBody* patack = NULL;

	bool spawnp = false;
	bool waiting = false;
};

