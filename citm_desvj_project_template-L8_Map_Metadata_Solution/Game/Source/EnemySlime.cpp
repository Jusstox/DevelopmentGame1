#include "EnemySlime.h"
#include "SceneManager.h"
#include "Player.h"
#include "Audio.h"
#include "Log.h"

EnemySlime::EnemySlime() :Enemy()
{

}

bool EnemySlime::Awake()
{
	pugi::xml_node anim = animconfig.child("walkenemyanimations").child("idleanim");

	idleanim.loop = anim.attribute("loop").as_bool();
	idleanim.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("idle"); animNode != NULL; animNode = animNode.next_sibling("idle")) {
		idleanim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	anim = animconfig.child("walkenemyanimations").child("walkanimchase");

	walkinganimchase.loop = anim.attribute("loop").as_bool();
	walkinganimchase.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("walk"); animNode != NULL; animNode = animNode.next_sibling("walk")) {
		walkinganimchase.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	anim = animconfig.child("walkenemyanimations").child("dieanim");

	dieanim.loop = anim.attribute("loop").as_bool();
	dieanim.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieanim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	Enemy::Awake();

	return true;
}

bool EnemySlime::Start()
{
	distChase = 10;
	initPosition = position;
	currentAnimation = &walkinganimchase;

	// L07 DONE 5: Add physics to the player - initialize physics body
	if (pbody == NULL) {
		pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2, bodyType::DYNAMIC);
		// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		pbody->listener = this;
		// L07 DONE 7: Assign collider type
		pbody->ctype = ColliderType::ENEMY;
		pbody->body->SetGravityScale(0);
	}

	chaseVelovity = 0.12;
	patrolVelocity = 0.1;

	Enemy::Start();

	return true;
}

bool EnemySlime::Update(float dt)
{
	if (abs(app->sceneManager->currentScene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) > 50) {
		velocity.x = 0;
		velocity.y = 0;
		pbody->body->SetLinearVelocity(velocity);
		return true;
	}
	velocity = b2Vec2(0, 10);
	if (!hit) {
		if (canChase(distChase)) {
			if ((dark && app->sceneManager->currentScene->GetPlayer()->dark) || (!dark && !app->sceneManager->currentScene->GetPlayer()->dark)) {
				dest = iPoint(PTileX, PTileY);
				moveToPlayer(dt);
				if (canmove) {
					currentAnimation = &walkinganimchase;
					ActualVelocity = chaseVelovity;
				}
				else {
					currentAnimation = &idleanim;
				}
			}
		}
		else if(Patrol1.y == getEnemyTileY() && (canmove || app->sceneManager->currentScene->GetPlayer()->godmode)){
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
			canmove = true;
		}
		else if(TileX < path->At(1)->x) {
			velocity.x = ActualVelocity * dt;
			canmove = true;
		}
		else {
			canmove = false;
			currentAnimation = &idleanim;
			velocity.x = 0;
		}
	}
	else if (path->Count() == 1) {
		if (app->sceneManager->currentScene->GetPlayer()->position.x < position.x) {
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
		break;
	case ColliderType::DEATH:
		hit = true;
		BodyPendingToDelete = true;
		velocity.x = 0;
		currentAnimation = &dieanim;
		break;
	case ColliderType::PLATFORM:
		break;
	case ColliderType::SHURIKEN:
		hit = true;
		BodyPendingToDelete = true;
		velocity.x = 0;
		currentAnimation = &dieanim;
		app->audio->PlayFx(dieFX);
		break;
	case ColliderType::DARK:
		dark = true;
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
		if (app->sceneManager->currentScene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
	}
}

bool EnemySlime::LoadState(pugi::xml_node& node)
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
		currentAnimation = &walkinganimchase;
		hit = false;
		dieanim.Reset();
	}
	node = EnemyNode.next_sibling("Enemy");

	return true;
}

bool EnemySlime::SaveState(pugi::xml_node& node)
{
	pugi::xml_node EnemyNode = node.append_child(name.GetString());
	EnemyNode.append_attribute("x").set_value(position.x);
	EnemyNode.append_attribute("y").set_value(position.y);
	EnemyNode.append_attribute("dead").set_value(dead);
	EnemyNode.append_attribute("dark").set_value(dark);
	return true;
}

bool EnemySlime::CleanUp()
{
	app->tex->UnLoad(texture);
	Enemy::CleanUp();
	return true;
}


