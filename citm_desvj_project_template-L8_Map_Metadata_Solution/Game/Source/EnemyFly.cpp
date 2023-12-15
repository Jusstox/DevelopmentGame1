#include "EnemyFly.h"
#include "Log.h"

EnemyFly::EnemyFly() :Enemy()
{

}

bool EnemyFly::Update(float dt)
{
	Enemy::Update(dt);
	LOG("Hola");
	return true;
}
