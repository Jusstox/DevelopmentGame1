#pragma once
#include "Enemy.h"
class EnemyFly :  public Enemy
{
public:
	EnemyFly();
	bool Awake();

	bool Start();

	bool Update(float dt);

	Animation flyinganim;
};

