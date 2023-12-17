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

bool EnemyFly::Start()
{
	position = iPoint(250, 350);
	initPosition = position;
	texture = app->tex->Load("Assets/Textures/Bat.png");
	currentAnimation = &flyinganim;
	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 3, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	distChase = 10;

	right = true;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.1;

	Enemy::Start();

	return true;
}

bool EnemyFly::Update(float dt)
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

	if (velocity.x > 0) {
		right = true;
	}
	if (velocity.x < 0) {
		right = false;
	}

	return true;
}

void EnemyFly::moveToPlayer(float dt)
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

void EnemyFly::OnCollision(PhysBody* physA, PhysBody* physB)
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
