#include "Boss.h"
#include "SceneManager.h"
#include "Player.h"
#include "Audio.h"

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

	anim = animconfig.child("bossenemyanimations").child("atackanim");

	atackanim.loop = anim.attribute("loop").as_bool();
	atackanim.speed = anim.attribute("speed").as_float();

	for (pugi::xml_node animNode = anim.child("atack"); animNode != NULL; animNode = animNode.next_sibling("atack")) {
		atackanim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	type = EntityType::BOSS;

	texturePatha = parameters.attribute("texturepatha").as_string();

	fxdark = app->audio->LoadFx(parameters.attribute("fxpathdark").as_string());
	fxatack = app->audio->LoadFx(parameters.attribute("fxpathatack").as_string());

	Enemy::Awake();

	return true;
}

bool Boss::Start()
{
	distChase = 30;
	currentAnimation = &flyinganim;

	// L07 DONE 5: Add physics to the player - initialize physics body
	if (pbody == NULL) {
		pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2 - 10, bodyType::DYNAMIC);
		// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		pbody->listener = this;
		// L07 DONE 7: Assign collider type
		pbody->ctype = ColliderType::ENEMY;
		pbody->body->SetGravityScale(0);
	}
	pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y + 1)), 0);
	inpos = pbody->body->GetTransform();

	if (patack == NULL) {
		patack = app->physics->CreateRectangleSensor(-10, -10, 30, 40, bodyType::DYNAMIC);
		// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
		patack->listener = this;
		// L07 DONE 7: Assign collider type
		patack->ctype = ColliderType::DEATH;
		patack->body->SetGravityScale(0);
	}

	chaseVelovity = 0.3;
	ActualVelocity = chaseVelovity;
	texturea = app->tex->Load(texturePatha);
	pawnsdead = 0;
	p1 = 0;
	p2 = 0;
	p3 = 0;
	fxatackplayed = false;

	Enemy::Start();

	return true;
}

bool Boss::Update(float dt)
{

	if (app->sceneManager->currentScene->GetPlayer()->fight) {
		if (spawnp) {
			for (size_t i = 0; i < 3; i++)
			{
				pawn[i]->active = true;
				pawn[i]->dead = false;
				pawn[i]->hit = false;
				pawn[i]->position = initPosition;
				pawn[i]->pbody->body->SetTransform(b2Vec2(inpos.p.x + i*2, inpos.p.y - 2), 0);
			}
			spawnp = false;
			atacking = false;
			waiting = true;
			p1 = 0;
			p2 = 0;
			p3 = 0;
		}


		if (waiting) {
			for (size_t i = 0; i < 3; i++)
			{
				if (pawn[i]->dead) {
					if (i == 0 && p1 == 0) {
						p1 = 1;
						pawnsdead++;
					}
					else if (i == 1 && p2 == 0) {
						p2 = 1;
						pawnsdead++;
					}
					else if (i == 2 && p3 == 0) {
						p3 = 1;
						pawnsdead++;
					}
				}
			}
			if (pawnsdead == 3) {
				for (size_t i = 0; i < 3; i++)
				{
					pawn[i]->active = false;
					pawn[i]->pbody->body->SetTransform(b2Vec2(-100, -100), 0);
				}
				app->sceneManager->currentScene->GetPlayer()->dark = false;
				waiting = false;
				pbody->body->SetTransform(b2Vec2(inpos.p.x, inpos.p.y), 0);
				app->sceneManager->currentScene->GetPlayer()->bossing = false;
			}
			else {
				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
				pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0)), 0);
				return true;
			}
		}

		if (canChase(distChase) && !atacking) {
			if (!app->sceneManager->currentScene->GetPlayer()->dark) {
				dest = iPoint(PTileX, PTileY);
				moveToPlayer(dt);
			}
			else {
				pbody->body->SetLinearVelocity(b2Vec2(0, 0));
				pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0)), 0);
			}
		}
		else {
			velocity = b2Vec2(0, 0);
		}

		if (atacking) {
			if (!fxatackplayed) {
				app->audio->PlayFx(fxatack);
				fxatackplayed = true;
			}
			if (currentAnimation->GetCurrentFrameInt() == 13) {
				if (right) {
					patack->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x + 1), PIXEL_TO_METERS(position.y + 2.5)), 0);
				}
				else {
					patack->body->SetTransform(b2Vec2(PIXEL_TO_METERS(position.x + 3), PIXEL_TO_METERS(position.y + 2.5)), 0);
				}
			}
			if (atackanim.HasFinished()) {
				fxatackplayed = false;
				currentAnimation = &flyinganim;
				patack->body->SetTransform(b2Vec2(PIXEL_TO_METERS(-10), PIXEL_TO_METERS(-10)), 0);
				atackanim.Reset();
				atacking = false;
			}
		}
	}

	if (!app->sceneManager->currentScene->settings) {
		if (!hit) {
			pbody->body->SetLinearVelocity(velocity);
			b2Transform pbodyPos = pbody->body->GetTransform();
			position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
			position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2 + 10);
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

	}

	if (velocity.x < 0) {
		right = true;
	}
	if (velocity.x > 0) {
		right = false;
	}

	return true;
}

void Boss::moveToPlayer(float dt)
{
	const DynArray<iPoint>* path = SearchWay();
	//check if t has arrivied
	if (path->Count() > 2) {
		//check if it shall move to x
		if (TileX > path->At(1)->x) {
			velocity.x = -ActualVelocity * dt;
		}
		else if (TileX < path->At(1)->x) {
			velocity.x = ActualVelocity * dt;
		}
	}
	else if (path->Count() == 2) {
		atacking = true;
		currentAnimation = &atackanim;
	}
}

void Boss::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::SHURIKEN:
		if (app->sceneManager->currentScene->GetPlayer()->fight) {
			lives--;
			if (lives == 20) {
				app->sceneManager->currentScene->GetPlayer()->dark = true;
				app->sceneManager->currentScene->GetPlayer()->bossing = true;
				spawnp = true;
				waiting = true;
				pawnsdead = 0;
				app->audio->PlayFx(fxdark);
			}
			if (lives == 10) {
				app->sceneManager->currentScene->GetPlayer()->dark = true;
				app->sceneManager->currentScene->GetPlayer()->bossing = true;
				spawnp = true;
				waiting = true;
				pawnsdead = 0;
				app->audio->PlayFx(fxdark);
			}
		}
		break;
	default:
		break;
	}
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
	atackanim.Reset();
	patack->body->SetTransform(b2Vec2(PIXEL_TO_METERS(-10), PIXEL_TO_METERS(-10)), 0);
	waiting = false;
	lives = 30;
	pawnsdead = 0;
	spawnp = false;
	atacking = false;
	Enemy::Respawn();
	Start();
	velocity.x = 0;
	fxatackplayed = false;
}

bool Boss::Draw()
{
	if (currentAnimation == &flyinganim) {
		if (!dead) {
			if ((app->sceneManager->currentScene->GetPlayer()->dark &&
				(abs(app->sceneManager->currentScene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) < 8) &&
				(abs(app->sceneManager->currentScene->GetPlayer()->getPlayerTileY() - getEnemyTileY()) < 8))
				|| app->sceneManager->currentScene->GetPlayer()->godmode) {
				if (dark) {
					if (right) {
						app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
					}
					else {
						app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
					}
				}
			}
			if (!app->sceneManager->currentScene->GetPlayer()->dark || app->sceneManager->currentScene->GetPlayer()->godmode) {
				if (!dark) {
					if (right) {
						app->render->DrawTexturePR(texture, position.x, position.y - 10, &currentAnimation->GetCurrentFrame());
					}
					else {
						app->render->DrawTexture(texture, position.x, position.y - 10, &currentAnimation->GetCurrentFrame());
					}
				}
			}
		}
	}
	else {
		if (!dead) {
			if ((app->sceneManager->currentScene->GetPlayer()->dark &&
				(abs(app->sceneManager->currentScene->GetPlayer()->getPlayerTileX() - getEnemyTileX()) < 8) &&
				(abs(app->sceneManager->currentScene->GetPlayer()->getPlayerTileY() - getEnemyTileY()) < 8))
				|| app->sceneManager->currentScene->GetPlayer()->godmode) {
				if (dark) {
					if (right) {
						app->render->DrawTexturePR(texturea, position.x, position.y, &currentAnimation->GetCurrentFrame());
					}
					else {
						app->render->DrawTexture(texturea, position.x, position.y, &currentAnimation->GetCurrentFrame());
					}
				}
			}
			if (!app->sceneManager->currentScene->GetPlayer()->dark || app->sceneManager->currentScene->GetPlayer()->godmode) {
				if (!dark) {
					if (right) {
						app->render->DrawTexturePR(texturea, position.x, position.y-2, &currentAnimation->GetCurrentFrame());
					}
					else {
						app->render->DrawTexture(texturea, position.x, position.y-2, &currentAnimation->GetCurrentFrame());
					}
				}
			}
		}
	}

	return true;
}