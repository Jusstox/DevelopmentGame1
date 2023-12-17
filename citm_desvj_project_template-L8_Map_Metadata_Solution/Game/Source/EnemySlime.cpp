#include "EnemySlime.h"
#include "Scene.h"
#include "Log.h"

EnemySlime::EnemySlime() :Enemy()
{

}

bool EnemySlime::Awake()
{
	Enemy::Awake();
	idleanim.PushBack({ 0, 0, 16, 14 });
	idleanim.PushBack({ 23, 0, 17, 14 });
	idleanim.PushBack({ 45, 0, 17, 14 });
	idleanim.PushBack({ 23, 0, 17, 14 });
	idleanim.loop = true;
	idleanim.speed = 0.1;

	walkinganimchase.PushBack({ 0, 34, 17, 15 });
	walkinganimchase.PushBack({ 22, 34, 17, 15 });
	walkinganimchase.PushBack({ 44, 34, 19, 15 });
	walkinganimchase.PushBack({ 68, 34, 18, 16 });
	walkinganimchase.PushBack({ 44, 34, 19, 15 });
	walkinganimchase.PushBack({ 91, 34, 17, 15 });
	walkinganimchase.PushBack({ 113, 34, 17, 15 });
	walkinganimchase.loop = true;
	walkinganimchase.speed = 0.1;

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
	dieanim.loop = false;
	dieanim.speed = 0.0918;

	return true;
}

bool EnemySlime::Start()
{
	distChase = 10;
	initPosition = position;
	texture = app->tex->Load(texturePath);
	currentAnimation = &walkinganimchase;

	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w/2, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.12;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool EnemySlime::Update(float dt)
{
	if (abs(app->scene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}
	velocity = b2Vec2(0, 10);
	if (!hit) {
		if (canChase(distChase) && PTileX >= Patrol1.x && PTileX <= Patrol2.x && PTileY <= Patrol1.y) {
			ActualVelocity = chaseVelovity;
			dest = iPoint(PTileX, PTileY);
			moveToPlayer(dt);
			currentAnimation = &walkinganimchase;
		}
		else {
			ActualVelocity = patrolVelocity;
			moveToPoint(dt);
		}
	}
	else {
		if (dieanim.HasFinished()) {
			dead = true;
		}
	}
	Enemy::Update(dt);

	if (velocity.x < 0) {
		right = true;
	}
	if (velocity.x > 0) {
		right = false;
	}

	return true;
}

void EnemySlime::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = SearchWay();
	//check if t has arrivied
	if (path->Count() > 1) {
		//check if it shall move to x
		if (TileX > path->At(1)->x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
}

void EnemySlime::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		hit = true;
		velocity.x = 0;
		currentAnimation = &dieanim;
		break;
	case ColliderType::DEATH:
		hit = true;
		velocity.x = 0;
		currentAnimation = &dieanim;
		break;
	case ColliderType::PLATFORM:
		break;
	default:
		break;
	}
}

void EnemySlime::moveToPoint(float dt)
{
	Enemy::Patrol();
	const DynArray<iPoint>* path = SearchWay();
	//check if t has arrivied
	if (path->Count() > 1) {
		//check if it shall move to x
		if (TileX > path->At(1)->x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
}


