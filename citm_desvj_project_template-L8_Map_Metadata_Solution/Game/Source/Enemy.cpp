#include "Enemy.h"
#include "Log.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Scene.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("Enemy");
}

Enemy::~Enemy()
{
}

bool Enemy::Awake()
{
	return true;
}

bool Enemy::Start()
{
	return true;
}

bool Enemy::Update(float dt)
{
	TileX = getEnemyTileX();
	TileY = getEnemyTileY();

	PTileX = app->scene->GetPlayer()->getPlayerTileX();
	PTileY = app->scene->GetPlayer()->getPlayerTileY();

	if (canChase(distChase) && !app->scene->GetPlayer()->godmode) {
		chasePlayer(dt);
	}
	else {
		velocity.y = 0;
		velocity.x = 0;
	}

	pbody->body->SetLinearVelocity(velocity);
	b2Transform pbodyPos = pbody->body->GetTransform();

	position.x = METERS_TO_PIXELS(pbodyPos.p.x) - (currentAnimation->GetCurrentFrame().w / 2);
	position.y = METERS_TO_PIXELS(pbodyPos.p.y) - (currentAnimation->GetCurrentFrame().h / 2);

	app->render->DrawTexture(texture, position.x, position.y, &currentAnimation->GetCurrentFrame());
	currentAnimation->Update();

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

void Enemy::chasePlayer(float dt)
{
	iPoint origin = iPoint(TileX, TileY);

	iPoint dest = iPoint(PTileX, PTileY);

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

	if (path->Count() > 1) {
		if (TileX != path->At(1)->x) {
			if (TileX > path->At(1)->x) {
				velocity.x = -chaseVelovity * dt;
			}
			else {
				velocity.x = chaseVelovity * dt;
			}
			if (path->Count() > 2) {
				if (path->At(2)->y != TileY) {
					if (TileY > path->At(2)->y) {
						velocity.y = -chaseVelovity * dt;
					}
					else {
						velocity.y = chaseVelovity * dt;
					}
				}
				else {
					velocity.y = 0;
				}
			}
		}
		else {
			if (TileY > path->At(1)->y) {
				velocity.y = -chaseVelovity * dt;
			}
			else {
				velocity.y = chaseVelovity * dt;
			}
			if (path->Count() > 2) {
				if (path->At(2)->x != TileX) {
					if (TileX > path->At(2)->x) {
						velocity.x = -chaseVelovity * dt;
					}
					else {
						velocity.x = chaseVelovity * dt;
					}
				}
				else {
					velocity.x = 0;
				}
			}
		}
	}

}

bool Enemy::canChase(int dist)
{
	if ((abs(TileX - PTileX) + abs(TileY - PTileY)) < dist) {
		return true;
	}

	return false;
}
