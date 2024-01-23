#pragma once
#include "Enemy.h"
class EnemyFly :  public Enemy
{
public:
	EnemyFly();
	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	bool LoadState(pugi::xml_node& node);

	bool SaveState(pugi::xml_node& node);

	void Respawn();

	Animation flyinganim;
	Animation flyinganimchase;
	Animation dieanim;
};

