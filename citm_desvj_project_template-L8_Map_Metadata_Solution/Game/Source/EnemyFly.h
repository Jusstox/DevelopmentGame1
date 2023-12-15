#pragma once
#include "Enemy.h"
class EnemyFly :  public Enemy
{
public:
	EnemyFly();
	bool Update(float dt);
};

