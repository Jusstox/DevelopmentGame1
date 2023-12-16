#include "EnemySlime.h"

EnemySlime::EnemySlime() :Enemy()
{

}

bool EnemySlime::Awake()
{
	flyinganim.PushBack({ 0, 0, 16, 14 });
	flyinganim.PushBack({ 23, 0, 17, 14 });
	flyinganim.PushBack({ 45, 0, 17, 14 });
	flyinganim.PushBack({ 23, 0, 17, 14 });
	flyinganim.loop = true;
	flyinganim.speed = 0.1;

	flyinganimchase.PushBack({ 0, 34, 17, 15 });
	flyinganimchase.PushBack({ 22, 34, 17, 15 });
	flyinganimchase.PushBack({ 44, 34, 19, 15 });
	flyinganimchase.PushBack({ 68, 34, 18, 16 });
	flyinganimchase.PushBack({ 44, 34, 19, 15 });
	flyinganimchase.PushBack({ 91, 34, 17, 15 });
	flyinganimchase.PushBack({ 113, 34, 17, 15 });
	flyinganimchase.loop = true;
	flyinganimchase.speed = 0.05;

	dieanim.PushBack({ 0, 99, 17, 15 });
	dieanim.PushBack({ 22, 99, 19, 15 });
	dieanim.PushBack({ 46, 99, 22, 15 });
	dieanim.PushBack({ 73, 99, 26, 15 });
	dieanim.PushBack({ 104, 99, 28, 15 });
	dieanim.PushBack({ 137, 99, 30, 15 });
	dieanim.PushBack({ 171, 99, 32, 15 });
	dieanim.PushBack({ 208, 99, 25, 15 });
	dieanim.PushBack({ 238, 99, 23, 15 });
	dieanim.PushBack({ 266, 99, 23, 15 });
	dieanim.PushBack({ 294, 100, 9, 15 });
	dieanim.PushBack({ 0, 0, 0, 0 });
	dieanim.loop = true;
	dieanim.speed = 0.0918;

	return true;
}

bool EnemySlime::Start()
{
	position = iPoint(250, 350);
	velocity = b2Vec2(0, +10);
	initPosition = position;
	texture = app->tex->Load("Assets/Textures/Slime.png");
	currentAnimation = &flyinganimchase;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.1;

	Enemy::Start();

	return true;
}

bool EnemySlime::Update(float dt)
{
	Enemy::Update(dt);
	return true;
}
