#include "EnemySlime.h"

EnemySlime::EnemySlime() :Enemy()
{

}

bool EnemySlime::Awake()
{
	flyinganim.PushBack({ 0, 0, 39, 35 });
	flyinganim.PushBack({ 40, 0, 38, 34 });
	flyinganim.PushBack({ 83, 0, 38, 33 });
	flyinganim.PushBack({ 122, 0, 39, 32 });
	flyinganim.PushBack({ 161, 0, 39, 35 });
	flyinganim.PushBack({ 0, 46, 42, 33 });
	flyinganim.loop = true;
	flyinganim.speed = 0.15;

	flyinganimchase.PushBack({ 0, 127, 39, 35 });
	flyinganimchase.PushBack({ 40, 127, 38, 34 });
	flyinganimchase.PushBack({ 83, 127, 38, 33 });
	flyinganimchase.PushBack({ 122, 127, 39, 32 });
	flyinganimchase.PushBack({ 161, 127, 39, 35 });
	flyinganimchase.PushBack({ 0, 173, 42, 33 });
	flyinganimchase.loop = true;
	flyinganimchase.speed = 0.15;

	dieanim.PushBack({ 0, 215, 43, 24 });
	dieanim.PushBack({ 47, 92, 40, 21 });
	dieanim.PushBack({ 98, 91, 28, 21 });
	dieanim.PushBack({ 138, 100, 23, 8 });
	dieanim.PushBack({ 171, 101, 6, 5 });
	dieanim.PushBack({ 188, 101, 6, 3 });
	dieanim.loop = false;
	dieanim.speed = 0.1;

	return true;
}

bool EnemySlime::Start()
{
	position = iPoint(250, 350);
	velocity = b2Vec2(0, 0);
	initPosition = position;
	texture = app->tex->Load("Assets/Textures/Slime.png");
	currentAnimation = &flyinganim;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 3, bodyType::DYNAMIC);

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
