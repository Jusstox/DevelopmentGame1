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

	Animation flyinganim;
	Animation flyinganimchase;
	Animation dieanim;
};

