#include "Enemy.h"
#include "Log.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Scene.h"
#include "Audio.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy()
{
}

bool Enemy::Awake()
{
	position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();
	Patrol1 = { parameters.attribute("dest1X").as_int() ,parameters.attribute("dest1Y").as_int() };
	Patrol2 = { parameters.attribute("dest2X").as_int() , parameters.attribute("dest2Y").as_int() };
	dark = parameters.attribute("dark").as_bool();
	dieFX = app->audio->LoadFx(parameters.attribute("fxpath").as_string());
	return true;
}

bool Enemy::Start()
{
	mouseTileTex = app->tex->Load("Assets/Maps/tileSelection.png");
	dead = false;
	hit = false;
	patrol = true;
	return true;
}

bool Enemy::Update(float dt)
{
	if(!hit) {
		pbody->body->SetLinearVelocity(velocity);
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
		position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2);
	}

	if (!dead) {
		if (app->scene->GetPlayer()->dark || app->scene->GetPlayer()->godmode) {
			if (dark) {
				if (right) {
					app->render->DrawTexturePR(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
				}
				else {
					app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
				}
			}
		}
		if(!app->scene->GetPlayer()->dark || app->scene->GetPlayer()->godmode){
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

	currentAnimation->Update(dt);
	
	if (dead)
	{
		pendingToDelete = true;
		position.x = 0;
		position.y = 0;
		// mirar de borrar texture i memory leaks
		//app->entityManager->DestroyEntity(this);
	}

	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
}

int Enemy::getEnemyTileX()
{
	return (position.x + (currentAnimation->GetCurrentFrame().w / 2)) / app->map->getTileWidth();
}

int Enemy::getEnemyTileY()
{
	return (position.y + (currentAnimation->GetCurrentFrame().h / 2)) / app->map->getTileHieght();
}

const DynArray<iPoint>* Enemy::SearchWay()
{
	iPoint origin = iPoint(TileX, TileY);

	app->map->pathfinding->CreatePath(origin, dest);

	// L13: Get the latest calculated path and draw
	const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
	if (app->physics->debug) {
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			app->render->DrawTexture(mouseTileTex, pos.x, pos.y);
		}
	}

	return path;
}

bool Enemy::canChase(int dist)
{
	bool canChase = false;

	TileX = getEnemyTileX();
	TileY = getEnemyTileY();

	PTileX = app->scene->GetPlayer()->getPlayerTileX();
	PTileY = app->scene->GetPlayer()->getPlayerTileY();

	if ((abs(TileX - PTileX) + abs(TileY - PTileY)) < dist) {
		canChase = true;
	}

	if (canChase && !app->scene->GetPlayer()->godmode) {
		canChase =  true;
	}
	else {
		canChase = false;
	}

	return canChase;
}

void Enemy::moveToPlayer(float dt)
{

}

void Enemy::Patrol()
{
	if (patrol) {
		dest.x = Patrol1.x;
		dest.y = Patrol1.y;
		if (getEnemyTileX() == dest.x && getEnemyTileY() == dest.y) {
			patrol = false;
		}
	}
	else {
		dest.x = Patrol2.x;
		dest.y = Patrol2.y;
		if (getEnemyTileX() == dest.x && getEnemyTileY() == dest.y) {
			patrol = true;
		}
	}
}
