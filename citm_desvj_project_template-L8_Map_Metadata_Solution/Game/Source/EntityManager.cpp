#include "EntityManager.h"
#include "Player.h"
#include "EnemyFly.h"
#include "EnemySlime.h"
#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L03: DONE 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::ENEMYFLY:
		entity = new EnemyFly();
		break;
	case EntityType::ENEMYSLIME:
		entity = new EnemySlime();
		break;
	default:
		break;
	}

	entities.Add(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::LoadState(pugi::xml_node node)
{
	bool ret = true;

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	pugi::xml_node EnemyNode = node.child("Enemy");
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		if (item->data->type == EntityType::ENEMY) {
			item->data->LoadState(EnemyNode);
		}
		else {
			item->data->LoadState(node);
		}
	}

	return ret;
}

bool EntityManager::SaveState(pugi::xml_node node)
{
	bool ret = true;

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		item->data->SaveState(node);		
	}

	return ret;
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;
	
	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) {
			continue;
		}

		if (pEntity->BodyPendingToDelete) {
			app->physics->world->DestroyBody(item->data->pbody->body);
			pEntity->BodyPendingToDelete = false;
		}
		
		ret = item->data->Update(dt);

		//mirar tema borra pk al carregar si he guardat 9 entities comen�o amb 10 i es lia
		/*if (pEntity->pendingToDelete) {
			DestroyEntity(pEntity);
		}*/
	}

	return ret;
}