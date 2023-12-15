#include "Enemy.h"
#include "Log.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy()
{
}

bool Enemy::Awake()
{
	return true;
}

bool Enemy::Start()
{
	return true;
}

bool Enemy::Update(float dt)
{
	LOG("Adeu");
	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
}
