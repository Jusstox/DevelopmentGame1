#include "EnemyFly.h"
#include "Scene.h"
#include "Audio.h"

EnemyFly::EnemyFly() :Enemy()
{

}

bool EnemyFly::Awake()
{
	pugi::xml_node anim = animconfig.child("flyenemyanimations").child("flyanim");

	flyinganim.loop = anim.attribute("loop").as_bool();
	flyinganim.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("fly"); animNode != NULL; animNode = animNode.next_sibling("fly")) {
		flyinganim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	anim = animconfig.child("flyenemyanimations").child("flyanimchase");

	flyinganimchase.loop = anim.attribute("loop").as_bool();
	flyinganimchase.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("chase"); animNode != NULL; animNode = animNode.next_sibling("chase")) {
		flyinganimchase.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	anim = animconfig.child("flyenemyanimations").child("dieanim");

	dieanim.loop = anim.attribute("loop").as_bool();
	dieanim.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieanim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	Enemy::Awake();

	return true;
}

bool EnemyFly::Start()
{
	initPosition = position;
	texture = app->tex->Load(texturePath);
	currentAnimation = &flyinganim;
	// L07 DONE 5: Add physics to the player - initialize physics body
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 3, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::ENEMY;

	distChase = 12;

	right = true;

	pbody->body->SetGravityScale(0);

	chaseVelovity = 0.15;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool EnemyFly::Update(float dt)
{
	if (abs(app->scene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}
	velocity = b2Vec2(0, 0);	

	if (!hit) {
		if (canChase(distChase)) {
			if ((dark && app->scene->GetPlayer()->dark) || (!dark && !app->scene->GetPlayer()->dark)) {
				ActualVelocity = chaseVelovity;
				dest = iPoint(PTileX, PTileY);
				moveToPlayer(dt);
				currentAnimation = &flyinganimchase;
			}
			else {
				ActualVelocity = patrolVelocity;
				Enemy::Patrol();
				moveToPlayer(dt);
				currentAnimation = &flyinganim;
			}
		}
		else {
			ActualVelocity = patrolVelocity;
			Enemy::Patrol();
			moveToPlayer(dt);
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
				velocity.x = -ActualVelocity * dt;
			}
			else {
				velocity.x = ActualVelocity * dt;
			}
			if (path->Count() > 2) {
				//if next step move y, go diagonal
				if (path->At(2)->y != TileY) {
					if (TileY > path->At(2)->y) {
						velocity.y = -ActualVelocity / 1.3 * dt;
					}
					else {
						velocity.y = ActualVelocity / 1.3 * dt;
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
				velocity.y = -ActualVelocity * dt;
			}
			else {
				velocity.y = ActualVelocity * dt;
			}
			if (path->Count() > 2) {
				//if next step move x, go diagonal
				if (path->At(2)->x != TileX) {
					if (TileX > path->At(2)->x) {
						velocity.x = -ActualVelocity / 1.3 * dt;
					}
					else {
						velocity.x = ActualVelocity / 1.3 * dt;
					}
					velocity.y /= 1.3;
				}
				else {
					velocity.x = 0;
				}
			}
		}
	}
	else if (path->Count() == 1) {
		if (app->scene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
		if (app->scene->GetPlayer()->position.y < position.y) {
			velocity.y = -ActualVelocity * dt;
		}
		else {
			velocity.y = ActualVelocity * dt;
		}
	}
}

void EnemyFly::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		break;
	case ColliderType::SHURIKEN:
		BodyPendingToDelete = true;
		hit = true;
		velocity.x = 0;
		velocity.y = 0;
		currentAnimation = &dieanim;
		app->audio->PlayFx(dieFX);
		break;
	default:
		break;
	}
}

bool EnemyFly::LoadState(pugi::xml_node& node)
{
	pugi::xml_node EnemyNode = node;
	dead = EnemyNode.attribute("dead").as_bool();
	if (dead) {
		position.x = 0;
		position.y = 0;
		b2Vec2 diePos = b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
		pbody->body->SetTransform(diePos, 0);
		hit = true;
	}
	else {
		position.x = EnemyNode.attribute("x").as_int();
		position.y = EnemyNode.attribute("y").as_int();
		dark = EnemyNode.attribute("dark").as_bool();
		b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y));
		pbody->body->SetTransform(pPosition, 0);
		currentAnimation = &flyinganim;
		hit = false;
		dieanim.Reset();
	}
	node = EnemyNode.next_sibling("Enemy");
	return true;
}

bool EnemyFly::SaveState(pugi::xml_node& node)
{
	pugi::xml_node EnemyNode = node.append_child(name.GetString());
	EnemyNode.append_attribute("x").set_value(position.x);
	EnemyNode.append_attribute("y").set_value(position.y);
	EnemyNode.append_attribute("dead").set_value(dead);
	EnemyNode.append_attribute("dark").set_value(dark);
	return true;
}
