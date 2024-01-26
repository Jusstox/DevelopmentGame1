#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Player.h"
#include "Level1.h"
#include "Level2.h"
#include "SceneManager.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name.Create("item");
}

Item::~Item() {}

bool Item::Awake() {

	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();

	return true;
}

bool Item::Start() {

	//initilize textures
	texture = app->tex->Load(texturePath);
	
	// L07 DONE 4: Add a physics to an item - initialize the physics body
	app->tex->GetSize(texture, texW, texH);
	if (pbody == NULL) {
		pbody = app->physics->CreateCircle(position.x + texH / 2, position.y + texH / 2, texH / 2, bodyType::DYNAMIC);

		// L07 DONE 7: Assign collider type
		pbody->ctype = ColliderType::ITEM;

		pbody->listener = this;
		inpos = pbody->body->GetTransform();
	}
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	hit = false;

	return true;
}

bool Item::Update(float dt)
{
	// L07 DONE 4: Add a physics to an item - update the position of the object from the physics.  

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2;
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2;

	if (!app->sceneManager->currentScene->GetPlayer()->dark) {
		app->render->DrawTexture(texture, position.x, position.y);
	}
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));

	if (hit) {
		pbody->body->SetTransform(b2Vec2(-50, -50), 0);
		hit = false;
		active = false;
	}

	return true;
}

bool Item::CleanUp()
{
	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		hit = true;
		break;
	default:
		break;
	}
}

void Item::Respawn()
{
	pbody->body->SetTransform(b2Vec2(inpos.p.x, inpos.p.y), 0);
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
}

void Item::MoveAway()
{
	if (pbody != NULL) {
		pbody->body->SetTransform(b2Vec2(-50, -50), 0);
		pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	}
}
