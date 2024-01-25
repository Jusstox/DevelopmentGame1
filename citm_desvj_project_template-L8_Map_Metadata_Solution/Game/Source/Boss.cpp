#include "Boss.h"
#include "SceneManager.h"
#include "Player.h"

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
	ActualVelocity = chaseVelovity;
	texturea = app->tex->Load(texturePatha);

	Enemy::Start();

	return true;
}

bool Boss::Update(float dt)
{

   /* if (canChase(distChase)) {
		dest = iPoint(PTileX, PTileY);
		moveToPlayer(dt);
	}
	else {
		velocity = b2Vec2(0,0);
	}*/

	if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
		atacking = true;
		currentAnimation = &atackanim;
	}

	if (atacking) {
		if (atackanim.HasFinished()) {
			currentAnimation = &flyinganim;
			atackanim.Reset();
		}
	}

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
					if ((TileY + 1) > path->At(2)->y) {
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
			if ((TileY + 1) > path->At(1)->y) {
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
		if (app->sceneManager->currentScene->GetPlayer()->position.x < position.x) {
			velocity.x = -ActualVelocity * dt;
		}
		else {
			velocity.x = ActualVelocity * dt;
		}
		if (app->sceneManager->currentScene->GetPlayer()->position.y < position.y) {
			velocity.y = -ActualVelocity * dt;
		}
		else {
			velocity.y = ActualVelocity * dt;
		}
	}
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
						app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
					}
					else {
						app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
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
						app->render->DrawTexturePR(texturea, position.x, position.y +13, &currentAnimation->GetCurrentFrame());
					}
					else {
						app->render->DrawTexture(texturea, position.x, position.y +13, &currentAnimation->GetCurrentFrame());
					}
				}
			}
		}
	}

	return true;
}