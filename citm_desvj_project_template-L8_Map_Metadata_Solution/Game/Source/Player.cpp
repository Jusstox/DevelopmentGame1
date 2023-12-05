#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Window.h"
#include "Map.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	initPosition = position;
	// idle animation - just one sprite
	idleAnim.PushBack({ 0, 0, 24, 32 });
	idleAnim.PushBack({ 27, 0, 25, 32 });
	idleAnim.PushBack({ 55, 0, 25, 32 });
	idleAnim.PushBack({ 27, 0, 25, 32 });
	idleAnim.loop = true;
	idleAnim.speed = 0.1f;

	walkAnim.PushBack({ 0, 37, 26, 29 });
	walkAnim.PushBack({ 29, 37, 27, 29 });
	walkAnim.PushBack({ 59, 37, 27, 29 });
	walkAnim.loop = true;
	walkAnim.speed = 0.1f;

	dieAnim.PushBack({ 27, 73, 25, 39 });
	dieAnim.PushBack({ 55, 81, 23, 31 });
	dieAnim.PushBack({ 85, 93, 41, 21 });
	dieAnim.loop = false;
	dieAnim.speed = 0.05f;

	jumpAnim1.PushBack({ 0, 115, 26, 31 });
	jumpAnim1.loop = false;

	jumpAnim2.PushBack({ 57, 115, 25, 31 });
	jumpAnim2.PushBack({ 85, 115, 25, 31 });
	jumpAnim2.loop = false;
	jumpAnim2.speed = 0.2f;

	blendFadeIN.PushBack({ 2500,1500,2500,1500 });
	blendFadeIN.PushBack({ 0,1500,2500,1500 });
	blendFadeIN.PushBack({ 2500,0,2500,1500 });
	blendFadeIN.PushBack({ 0,0,2500,1500 });
	blendFadeIN.loop = false;
	blendFadeIN.speed = 0.5f;

	blendFadeOut.PushBack({ 0,0,2500,1500 });
	blendFadeOut.PushBack({ 2500,0,2500,1500 });
	blendFadeOut.PushBack({ 0,1500,2500,1500 });
	blendFadeOut.PushBack({ 2500,1500,2500,1500 });
	blendFadeOut.PushBack({ 0,0,0,0 });
	blendFadeOut.loop = false;
	blendFadeOut.speed = 0.5f;

	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("texturePath").as_string());
	blendTexture = app->tex->Load(config.attribute("textureblendPath").as_string());
	blendFadeOut.setCurrentFrame(4);
	dark = false;
	state = IDLE;
	// L07 DONE 5: Add physics to the player - initialize physics body
	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &dieAnim;
	pbody = app->physics->CreateCircle(position.x, position.y, currentAnimation->GetCurrentFrame().w / 2, bodyType::DYNAMIC);

	// L07 DONE 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L07 DONE 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());
	victory = app->audio->LoadFx(config.attribute("winfxpath").as_string());
	app->audio->PlayMusic(config.attribute("musicpath").as_string());
	death = false;
	flip = false;

	godmode = false;

	return true;
}

bool Player::Update(float dt)
{
	// L07 DONE 5: Add physics to the player - updated player position using physics

	//L03: DONE 4: render the player texture and modify the position of the player using WSAD keys and render the texture
	b2Vec2 velocity = b2Vec2(0, -GRAVITY_Y);

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN) {
		if (!godmode) {
			godmode = true;
		}
		else {
			godmode = false;
			position.x += (26 / 2);
			position.y += (40 / 2);
			b2Vec2 pPosition = b2Vec2(PIXEL_TO_METERS(position.x) , PIXEL_TO_METERS(position.y));
			pbody->body->SetTransform(pPosition, 0);
		}
	}


	switch (state)
	{
	case IDLE:
		currentAnimation = &idleAnim;
		break;
	case WALIKING:
		currentAnimation = &walkAnim;
		break;
	case DIE:
		currentAnimation = &dieAnim;
		break;
	case JUMP1:
		currentAnimation = &jumpAnim1;
		break;
	case JUMP2:
		currentAnimation = &jumpAnim2;
		break;
	default:
		break;
	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_REPEAT) {
		respawn();
	}

	if (death) {
		state = DIE;
		if (dieAnim.HasFinished()) {
			dieAnim.Reset();
			state = IDLE;
			respawn();
			death = false;
		}
	}

	if (!death) {

		b2ContactEdge* contact = pbody->body->GetContactList();
		if (contact != nullptr) {
			b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
			if (contactPonts.y == -1) {
				if (state == JUMP2) {
					jumpAnim2.Reset();
					state = IDLE;
				}
				jumps = 2;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.5 * dt;
			flip = true;
			if (state == IDLE) {
				state = WALIKING;
			}
			if (contact != nullptr) {
				b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
				if (contactPonts.y != -1) {
					state = JUMP2;
				}
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.5 * dt;
			flip = false;
			if (state == IDLE) {
				state = WALIKING;
			}
			if (contact != nullptr) {
				b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
				if (contactPonts.y != -1) {
					state = JUMP2;
				}
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_IDLE && app->input->GetKey(SDL_SCANCODE_A) == KEY_IDLE) {
			if (state == WALIKING) {
				state = IDLE;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			if (jumps > 0) {
				state = JUMP1;
				if (contact != nullptr) {
					b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
					if (contactPonts.y == 0 && (contactPonts.x == 1 || contactPonts.x == -1)) {
						jumps = 0;
						remainingJumpSteps = maxremainingJumpSteps;
						jumpForceReduce = 0;
					}
					else {
						jumps--;
						remainingJumpSteps = maxremainingJumpSteps;
						jumpForceReduce = 0;
					}
				}
				else {
					jumps = 0;
					remainingJumpSteps = maxremainingJumpSteps;
					jumpForceReduce = 0;
				}
			}
		}

		if (remainingJumpSteps > 0) {
			float force = pbody->body->GetMass() * 10 / (1 / 30.0);
			force /= 2.7f;
			force -= jumpForceReduce;
			jumpForceReduce = maxremainingJumpSteps - remainingJumpSteps;
			pbody->body->ApplyForceToCenter(b2Vec2(0, -(force * dt)), true);
			remainingJumpSteps--;
			if (jumpForceReduce > force) {
				remainingJumpSteps = 0;
				jumpForceReduce = 0;
			}
			if (contact != nullptr) {
				b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
				if (contactPonts.y == 1) {
					force = 0;
					remainingJumpSteps = 0;
					jumpForceReduce = 0;
				}
			}
		}

		if (remainingJumpSteps <= 0) {
			if (state == JUMP1) {
				jumpAnim1.Reset();
				state = JUMP2;
			}
		}
	}

	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		if (!dark) {
			dark = true;
		}
		else {
			dark = false;
		}
	}

	// poner sizes de textura no hardcoded
	if (dark) {
		app->render->DrawTexturePR(blendTexture, position.x - 1250, position.y - 750, &blendFadeIN.GetCurrentFrame());
		blendFadeIN.Update();
		blendFadeOut.Reset();
	}
	else {
		app->render->DrawTexturePR(blendTexture, position.x - 1250, position.y - 750, &blendFadeOut.GetCurrentFrame());
		blendFadeOut.Update();
		blendFadeIN.Reset();
	}

	if (!godmode) {
		pbody->body->SetLinearVelocity(velocity);
		b2Transform pbodyPos = pbody->body->GetTransform();

		//preguntar al profe tema camera (el problema es en y) estaba  &blendFadeOut.GetCurrentFrame().h
		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (26 / 2);
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (40 / 2);
	}
	else {
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
			position.y += -0.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			position.y += 0.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			position.x += -0.5 * dt;
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			position.x += 0.5 * dt;
		}
	}

	LOG("%d", pbody->body->GetTransform().p.y);

	if (flip) {
		app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}else{
		app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	}

	currentAnimation->Update();

	return true;
}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {

	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		if (physB->ctype == ColliderType::ITEM) {
			app->entityManager->DestroyEntity(physB->listener);
		}
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::VICTORY:
		LOG("Collision UNKNOWN");
		app->audio->PlayFx(victory);
		break;
	case ColliderType::DEATH:
		if (!godmode) {
			death = true;
		}
		LOG("Collision UNKNOWN");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::respawn()
{
	b2Vec2 initPos = b2Vec2(PIXEL_TO_METERS(initPosition.x), PIXEL_TO_METERS(initPosition.y));
	pbody->body->SetTransform(initPos, 0);
	position = initPosition;
	app->render->camera.y = ((position.y - currentAnimation->GetCurrentFrame().h / 2) - (app->scene->windowH / 2)) * -1;
	app->render->camera.x = ((position.x - currentAnimation->GetCurrentFrame().w / 2) - (app->scene->windowW / 2)) * -1;

	if (app->render->camera.x >= 0) {
		app->render->camera.x = 0;
	}
	if (app->render->camera.y >= 0) {
		app->render->camera.y = 0;
	}
}

int Player::getPlayerTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) /app->map->getTileWidth();
}

int Player::getPlayerTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->getTileHieght();
}
