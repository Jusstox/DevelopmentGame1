#pragma once
#include "Enemy.h"
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	bool Awake();

	bool Start();

	bool Update(float dt);

	Animation flyinganim;
	Animation flyinganimchase;
	Animation dieanim;
};

