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

	// get the animations
	// idle animation
	idleAnim.loop = config.child("animations").child("idleanim").attribute("loop").as_bool();
	idleAnim.speed = config.child("animations").child("idleanim").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("idleanim").child("idle"); animNode != NULL; animNode = animNode.next_sibling("idle")) {
		idleAnim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	// walk animation
	walkAnim.loop = config.child("animations").child("walkanim").attribute("loop").as_bool();
	walkAnim.speed = config.child("animations").child("walkanim").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("walkanim").child("walk"); animNode != NULL; animNode = animNode.next_sibling("walk")) {
		walkAnim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}
	
	// die anim
	dieAnim.loop = config.child("animations").child("dieanim").attribute("loop").as_bool();
	dieAnim.speed = config.child("animations").child("dieanim").attribute("speed").as_float();

	for (pugi::xml_node animNode = config.child("animations").child("dieanim").child("die"); animNode != NULL; animNode = animNode.next_sibling("die")) {
		dieAnim.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	// init jump anim
	jumpAnim1.loop = config.child("animations").child("jumpanim1").attribute("loop").as_bool();

	for (pugi::xml_node animNode = config.child("animations").child("jumpanim1").child("jump"); animNode != NULL; animNode = animNode.next_sibling("jump")) {
		jumpAnim1.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	// final jump anim
	jumpAnim2.loop = config.child("animations").child("jumpanim2").attribute("loop").as_bool();

	for (pugi::xml_node animNode = config.child("animations").child("jumpanim2").child("jump"); animNode != NULL; animNode = animNode.next_sibling("jump")) {
		jumpAnim2.PushBack({ animNode.attribute("x").as_int(), animNode.attribute("y").as_int() ,animNode.attribute("w").as_int() ,animNode.attribute("h").as_int() });
	}

	blendFadeIN.PushBack({ 0,0,600,600 });
	blendFadeIN.loop = false;
	blendFadeIN.speed = 0.3f;

	blendFadeOut.PushBack({ 0,0,600,600 });
	blendFadeOut.loop = false;
	blendFadeOut.speed = 0.3f;

	return true;
}

bool Player::Start() {

	texture = app->tex->Load(config.attribute("texturePath").as_string());
	blendTexture = app->tex->Load(config.attribute("textureblendPath").as_string());
	dark = false;
	state = IDLE;
	// L07 DONE 5: Add physics to the player - initialize physics body
	app->tex->GetSize(texture, texW, texH);
	currentAnimation = &idleAnim;
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
			dark = false;
		}
	}

	if (!death) {

		if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			velocity.x = -0.5 * dt;
			flip = true;
			if (state == IDLE) {
				state = WALIKING;
			}
		}

		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			velocity.x = 0.5 * dt;
			flip = false;
			if (state == IDLE) {
				state = WALIKING;
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
				jumps--;
				remainingJumpSteps = maxremainingJumpSteps;
				jumpForceReduce = 0;
			}
		}

		if (remainingJumpSteps > 0) {
			force = pbody->body->GetMass() * 10 / (1 / 30.0);
			force /= 2.7f;
			force -= jumpForceReduce;
			jumpForceReduce = maxremainingJumpSteps - remainingJumpSteps;
			pbody->body->ApplyForceToCenter(b2Vec2(0, -(force * dt)), true);
			remainingJumpSteps--;
			if (jumpForceReduce > force) {
				remainingJumpSteps = 0;
				jumpForceReduce = 0;
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
		blend = true;
		app->render->DrawTexturePR(blendTexture, position.x - 300, position.y - 300, &blendFadeIN.GetCurrentFrame());
		blendFadeIN.Update();
		blendFadeOut.Reset();
	}
	else if(blend){
		app->render->DrawTexturePR(blendTexture, position.x - 300, position.y - 300, &blendFadeOut.GetCurrentFrame());
		blendFadeOut.Update();
		blendFadeIN.Reset();
		if (blendFadeOut.HasFinished()) {
			blend = false;
		}
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
	b2ContactEdge* contact = pbody->body->GetContactList();
	b2Vec2 contactPonts = contact->contact->GetManifold()->localNormal;
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		if (contactPonts.y == -1) {
			if (state == JUMP2) {
				jumpAnim2.Reset();
				state = IDLE;
			}
			jumps = 2;
		}
		if (contactPonts.y == 1) {
			force = 0;
			remainingJumpSteps = 0;
			jumpForceReduce = 0;
		}
		break;
	case ColliderType::ITEM:
		if (physB->ctype == ColliderType::ITEM) {
			app->entityManager->DestroyEntity(physB->listener);
		}
	//	app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::VICTORY:
		app->audio->PlayFx(victory);
		break;
	case ColliderType::DEATH:
		if (!godmode) {
			death = true;
		}
		break;
	case ColliderType::DARK:
		dark = true;
		break;
	case ColliderType::OUTSIDE:
		dark = false;
		break;
	case ColliderType::ENEMY:
		/*LOG("%f", contactPonts.y);
		if (contactPonts.y <= 0) {
			if (state == JUMP2) {
				jumpAnim2.Reset();
				state = IDLE;
			}
			jumps = 2;
		}
		else if(!godmode){
			death = true;
		} */
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

	dark = false;
}

int Player::getPlayerTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) /app->map->getTileWidth();
}

int Player::getPlayerTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->getTileHieght();
}
