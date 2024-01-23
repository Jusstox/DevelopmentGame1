#pragma once
#include "Enemy.h"
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	bool Awake();

	bool Start();

	bool Update(float dt);

	void moveToPlayer(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB);

	void moveToPoint(float dt);

	bool LoadState(pugi::xml_node& node);

	bool SaveState(pugi::xml_node& node);

	void Respawn();

	Animation idleanim;
	Animation walkinganimchase;
	Animation dieanim;

	bool canmove = true;
};

