#include "EnemyFly.h"
#include "Log.h"

EnemyFly::EnemyFly() :Enemy()
{

}

bool EnemyFly::Awake()
{
	flyinganim.PushBack({ 0, 0, 39, 35 });
	flyinganim.PushBack({ 40, 0, 38, 34 });
	flyinganim.PushBack({ 83, 0, 38, 33 });
	flyinganim.PushBack({ 122, 0, 39, 32 });
	flyinganim.PushBack({ 161, 0, 39, 35 });
	flyinganim.PushBack({ 0, 46, 42, 33 });
	flyinganim.loop = true;
	flyinganim.speed = 0.15;
	return true;
}

bool EnemyFly::Start()
{
	position = iPoint(250, 350);
	initPosition = position;
	texture = app->tex->Load("Assets/Textures/Bat.png");
	currentAnimation = &flyinganim;
	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	mouseTileTex = app->tex->Load("Assets/Maps/tileSelection.png");

	distChase = 20;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.1;

	return true;
}

bool EnemyFly::Update(float dt)
{
	b2Vec2 velocity = b2Vec2(0, 0);
	Enemy::Update(dt);
	return true;
}
