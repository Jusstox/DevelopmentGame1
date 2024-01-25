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
};

