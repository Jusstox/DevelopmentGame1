#include "Boss.h"
#include "SceneManager.h"

Boss::Boss()
{
}

bool Boss::Awake()
{
	pugi::xml_node anim = animconfig.child("bossenemyanimations").child("flyanim");

	flyinganim.loop = anim.attribute("loop").as_bool();
	flyinganim.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("fly"); animNode != NULL; animNode = animNode.next_sibling("fly")) {
		flyinganim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	Enemy::Awake();

	return true;
}

bool Boss::Start()
{
	distChase = 20;
	currentAnimation = &flyinganim;

	// L07 DONE 5: Add physics to the player - initialize physics body
	if (pbody == NULL) {
		pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2, bodyType::DYNAMIC);
		// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		pbody->listener = this;
		// L07 DONE 7: Assign collider type
		pbody->ctype = ColliderType::ENEMY;
	}

	chaseVelovity = 0.12;

	Enemy::Start();

	return true;
}

bool Boss::Update(float dt)
{
	if (!app->sceneManager->currentScene->settings) {
		if (!hit) {
			pbody->body->SetLinearVelocity(velocity);
			b2Transform pbodyPos = pbody->body->GetTransform();
			position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
			position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2 + 30);
		}
		else {
			pbody->body->SetLinearVelocity(b2Vec2(0, 0));
			pbody->body->SetTransform(b2Vec2(-10, 1), 0);
		}

		currentAnimation->Update(dt);
	}
	else {
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	}

	Draw();

	if (dead)
	{
		//pendingToDelete = true;
		position.x = 0;
		position.y = 0;
		// mirar de borrar texture i memory leaks
		//app->entityManager->DestroyEntity(this);
	}
	return true;
}

void Boss::moveToPlayer(float dt)
{
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB)
{
}

bool Boss::LoadState(pugi::xml_node& node)
{
	return true;
}

bool Boss::SaveState(pugi::xml_node& node)
{
	return true;
}

void Boss::Respawn()
{
	currentAnimation = &flyinganim;
	Enemy::Respawn();
}
