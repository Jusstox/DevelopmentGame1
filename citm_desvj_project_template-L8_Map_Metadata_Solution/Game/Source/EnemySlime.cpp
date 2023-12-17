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
	dieanim.loop = false;
	dieanim.speed = 0.0918;

	return true;
}

bool EnemySlime::Start()
{
	position = iPoint(250, 350);
	velocity = b2Vec2(0, 0);
	distChase = 10;
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
	velocity = b2Vec2(0, 0);

	if (!hit) {
		if (canChase(distChase)) {
			dest = iPoint(PTileX, PTileY);
			moveToPlayer(dt);
			currentAnimation = &flyinganimchase;
		}
		else {
			currentAnimation = &flyinganim;
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
		if (TileX != path->At(1)->x) {
			if (TileX > path->At(1)->x) {
				velocity.x = -chaseVelovity * dt;
			}
			else {
				velocity.x = chaseVelovity * dt;
			}
			if (path->Count() > 2) {
				//if next step move y, go diagonal
				if (path->At(2)->y != TileY) {
					if (TileY > path->At(2)->y) {
						velocity.y = -chaseVelovity / 1.3 * dt;
					}
					else {
						velocity.y = chaseVelovity / 1.3 * dt;
					}
					velocity.x /= 1.3;
				}
				else {
					velocity.y = 0;
				}
			}
		}
		else { //check if it shall move to y
			if (TileY > path->At(1)->y) {
				velocity.y = -chaseVelovity * dt;
			}
			else {
				velocity.y = chaseVelovity * dt;
			}
			if (path->Count() > 2) {
				//if next step move x, go diagonal
				if (path->At(2)->x != TileX) {
					if (TileX > path->At(2)->x) {
						velocity.x = -chaseVelovity / 1.3 * dt;
					}
					else {
						velocity.x = chaseVelovity / 1.3 * dt;
					}
					velocity.y /= 1.3;
				}
				else {
					velocity.x = 0;
				}
			}
		}
	}
}

void EnemySlime::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		hit = true;
		currentAnimation = &dieanim;
		break;
	default:
		break;
	}
}


