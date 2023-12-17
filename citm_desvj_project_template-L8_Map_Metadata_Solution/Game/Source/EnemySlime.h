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

	Animation flyinganim;
	Animation flyinganimchase;
	Animation dieanim;
};

