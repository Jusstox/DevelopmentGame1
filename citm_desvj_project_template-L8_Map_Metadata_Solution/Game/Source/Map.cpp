
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "SceneManager.h"
#include "Player.h"
#include "Enemy.h"

#include "Defs.h"
#include "Log.h"

#include <math.h>
#include "SDL_image/include/SDL_image.h"
#include <vector>
#include <iostream>
#include <sstream>

Map::Map() : Module(), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node config)
{
    LOG("Loading Map Parser");
    bool ret = true;

    return ret;
}

bool Map::Start() {
    //Initialize pathfinding 
    pathfinding = new PathFinding();
    active = false;
    return true;
}

bool Map::Update(float dt)
{
    bool ret = true;

    ListItem<MapLayer*>* mapLayer; 
    mapLayer = mapData.layers.start;

    // L06: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()
    //limitar el renderizado
    if (!app->sceneManager->currentScene->GetPlayer()->blend) {
        fisrtX = app->render->GetFirstTileX();
        lastX = app->render->GetLastTileX();

        fisrtY = app->render->GetFirstTileY();
        lastY = app->render->GetLastTileY();
    }

    //mirar de no cojer tiles que no existen

    if (app->sceneManager->currentScene->GetPlayer()->dark) {
        fisrtX = app->sceneManager->currentScene->GetPlayer()->getPlayerTileX() - 7;
        lastX = app->sceneManager->currentScene->GetPlayer()->getPlayerTileX() + 8;
        fisrtY = app->sceneManager->currentScene->GetPlayer()->getPlayerTileY() - 8;
        lastY = app->sceneManager->currentScene->GetPlayer()->getPlayerTileY() + 6;
    }

    if (fisrtX < 0) {
        fisrtX = 0;
    }
    if (lastX > mapData.width) {
        lastX = mapData.width;
    }

    if (fisrtY < 0) {
        fisrtY = 0;
    }
    if (lastY > mapData.height) {
        lastY = mapData.height;
    }

    // iterates the layers in the map
    while (mapLayer != NULL) {
        if (mapLayer->data->name == "dark") {
            if (app->sceneManager->currentScene->GetPlayer()->dark || app->sceneManager->currentScene->GetPlayer()->godmode) {
                mapLayer->data->properties.GetProperty("Draw")->value = false;
            }
            else {
                mapLayer->data->properties.GetProperty("Draw")->value = true;
            }
        }
        //Check if the property Draw exist get the value, if it's true draw the lawyer
        if (mapLayer->data->properties.GetProperty("Draw") != NULL && mapLayer->data->properties.GetProperty("Draw")->value) {
            //iterate all tiles in a layer
            for (int i = fisrtX; i < lastX; i++) {
                for (int j = fisrtY; j < lastY; j++) {
                    //Get the gid from tile
                      int gid = mapLayer->data->Get(i, j);

                    //L08: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    //Get the Rect from the tileSetTexture;
                    TileSet* tileSet = GetTilesetFromTileId(gid);
                    SDL_Rect tileRect = tileSet->GetRect(gid);
                    //SDL_Rect tileRect = mapData.tilesets.start->data->GetRect(gid); // (!!) we are using always the first tileset in the list

                    //Get the screen coordinates from the tile coordinates
                    iPoint mapCoord = MapToWorld(i, j);

                    // L06: DONE 9: Complete the draw function
                    app->render->DrawTexture(tileSet->texture, mapCoord.x, mapCoord.y, &tileRect);

                }
            }
        }


        mapLayer = mapLayer->next;
    }

    return ret;
}

// L08: DONE 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = NULL;

    ListItem<TileSet*>* tileSet;
    tileSet = mapData.tilesets.start;
    while (tileSet != NULL) {
        set = tileSet->data;
        if (gid >= tileSet->data->firstgid && gid < (tileSet->data->tilecount + tileSet->data->firstgid)) break;
        tileSet = tileSet->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // L05: DONE 2: Make sure you clean up any memory allocated from tilesets/map
    ListItem<TileSet*>* tileset;
    tileset = mapData.tilesets.start;
    
    while (tileset != NULL) {
        app->tex->UnLoad(tileset->data->texture);
        tileset->data->texture = nullptr;
        RELEASE(tileset->data);
        tileset = tileset->next;
    }

    mapData.tilesets.Clear();

    // L06: DONE 2: clean up all layer data
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.layers.start;

    while (layerItem != NULL) {
        RELEASE(layerItem->data->tiles);
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }

    mapData.layers.Clear();

    ListItem<PhysBody*>* collisions;
    collisions = mapData.colliders.start;

    while (collisions != NULL) {
        app->physics->DestroyBody(collisions->data->body);
        RELEASE(collisions->data);
        collisions = collisions->next;
    }

    mapData.colliders.Clear();

    return true;
}

// Load new map
bool Map::Load(SString mapFileName)
{
    bool ret = true;

    // L05: DONE 3: Implement LoadMap to load the map properties
    // retrieve the paremeters of the <map> node and save it into map data

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }
    else {

        //Fill mapData variable
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tilewidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileheight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L09: TODO 2: Define a property to store the MapType and Load it from the map

        // L05: DONE 4: Implement the LoadTileSet function to load the tileset properties
       // Iterate the Tileset
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset")) {

            TileSet* tileset = new TileSet();

            //Load Tileset attributes
            tileset->name = tilesetNode.attribute("name").as_string();
            tileset->firstgid = tilesetNode.attribute("firstgid").as_int();
            tileset->margin = tilesetNode.attribute("margin").as_int();
            tileset->spacing = tilesetNode.attribute("spacing").as_int();
            tileset->tilewidth = tilesetNode.attribute("tilewidth").as_int();
            tileset->tileheight = tilesetNode.attribute("tileheight").as_int();
            tileset->columns = tilesetNode.attribute("columns").as_int();
            tileset->tilecount = tilesetNode.attribute("tilecount").as_int();

            //Load Tileset image
            SString mapTex = path;
            mapTex += tilesetNode.child("image").attribute("source").as_string();
            tileset->texture = app->tex->Load(mapTex.GetString());

            mapData.tilesets.Add(tileset);

        }

        // L06: DONE 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {

            // L06: DONE 4: Implement a function that loads a single layer layer
            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L08: DONE 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Reserve the memory for the data 
            mapLayer->tiles = new uint[mapLayer->width * mapLayer->height];
            memset(mapLayer->tiles, 0, mapLayer->width * mapLayer->height);

            //Iterate over all the tiles and assign the values in the data array
            int i = 0;
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles[i] = tileNode.attribute("gid").as_uint();
                i++;
            }

            //add the layer to the map
            mapData.layers.Add(mapLayer);
        }

        // L07 DONE 3: Create colliders      
        // L07 DONE 7: Assign collider type
        // Later you can create a function here to load and create the colliders from the map
        for (pugi::xml_node objectgroupNode = mapFileXML.child("map").child("objectgroup"); objectgroupNode != NULL; objectgroupNode = objectgroupNode.next_sibling("objectgroup"))
        {
            for (pugi::xml_node objectNode = objectgroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object"))
            {
                int x = objectNode.attribute("x").as_int();
                int y = objectNode.attribute("y").as_int();
                int w = objectNode.attribute("width").as_int();
                int h = objectNode.attribute("height").as_int();
                if (w == 0) {
                    std::string s = objectNode.child("polygon").attribute("points").as_string();
                    std::istringstream iss(s);
                    std::string point;
                    std::vector<int> intVector;

                    while (std::getline(iss, point, ' ')) {
                        size_t commaPos = point.find(',');
                        float x = std::stof(point.substr(0, commaPos));
                        float y = std::stof(point.substr(commaPos + 1));

                        // Convert to int (you might want to round or truncate depending on your specific needs)
                        int intX = static_cast<int>(x);
                        int intY = static_cast<int>(y);

                        intVector.push_back(intX);
                        intVector.push_back(intY);
                    }
                    int* intArray = intVector.data();

                    PhysBody* c1 = app->physics->CreateChain(x, y, intArray, intVector.size(), STATIC);
                    c1->ctype = ColliderType::PLATFORM;
                    mapData.colliders.Add(c1);
                }
                else {
                    PhysBody* c1;
                    SString name = objectgroupNode.attribute("name").as_string();
                    if (name == "plataformas") {
                        c1 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
                        c1->ctype = ColliderType::PLATFORM;
                    }
                    else if (name == "ganar") {
                        c1 = app->physics->CreateRectangleSensor(x + w / 2, y + h / 2, w, h, STATIC);
                        c1->ctype = ColliderType::VICTORY;
                    }
                    else if (name == "muerte") {
                        c1 = app->physics->CreateRectangle(x + w / 2, y + h / 2, w, h, STATIC);
                        c1->ctype = ColliderType::DEATH;
                    }
                    else if (name == "Luz") {
                        c1 = app->physics->CreateRectangleSensor(x + w / 2, y + h / 2, w, h, STATIC);
                        c1->ctype = ColliderType::DARK;
                    }
                    else if (name == "afuera") {
                        c1 = app->physics->CreateRectangleSensor(x + w / 2, y + h / 2, w, h, STATIC);
                        c1->ctype = ColliderType::OUTSIDE;
                    }
                   
                    mapData.colliders.Add(c1);
                }
            }
        }

          // L05: DONE 5: LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tilewidth, mapData.tileheight);

            LOG("Tilesets----");

            ListItem<TileSet*>* tileset;
            tileset = mapData.tilesets.start;
            while (tileset != NULL) {
                //iterate the tilesets
                LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
                LOG("tile width : %d tile height : %d", tileset->data->tilewidth, tileset->data->tileheight);
                LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
                tileset = tileset->next;
            }

            LOG("Layers----");

            ListItem<MapLayer*>* mapLayer;
            mapLayer = mapData.layers.start;

            while (mapLayer != NULL) {
                LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
                LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
                mapLayer = mapLayer->next;
            }
        }

        // Find the navigation layer
        ListItem<MapLayer*>* mapLayerItem;
        mapLayerItem = mapData.layers.start;
        navigationLayer = mapLayerItem->data;

        //Search the layer in the map that contains information for navigation
        while (mapLayerItem != NULL) {
            if (mapLayerItem->data->properties.GetProperty("Navigation") != NULL && mapLayerItem->data->properties.GetProperty("Navigation")->value) {
                navigationLayer = mapLayerItem->data;
                break;
            }
            mapLayerItem = mapLayerItem->next;
        }

        if (mapFileXML) mapFileXML.reset();
    }

    mapLoaded = ret;
    return ret;
}

// L08: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.Add(p);
    }

    return ret;
}

// L08: DONE 7: Implement a method to get the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* property = propertyList.start;
    Property* p = NULL;

    while (property)
    {
        if (property->data->name == name) {
            p = property->data;
            break;
        }
        property = property->next;
    }

    return p;
}

// L09: DONE 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
iPoint Map::WorldToMap(int x, int y) {

    iPoint ret(0, 0);
    ret.x = x / mapData.tilewidth;
    ret.y = y / mapData.tileheight;

    return ret;
}

void Map::CreateNavigationMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;

    //Sets the size of the map. The navigation map is a unidimensional array 
    uchar* navigationMap = new uchar[navigationLayer->width * navigationLayer->height];
    //reserves the memory for the navigation map
    memset(navigationMap, 1, navigationLayer->width * navigationLayer->height);

    for (int x = 0; x < mapData.width; x++)
    {
        for (int y = 0; y < mapData.height; y++)
        {
            //i is the index of x,y coordinate in a unidimensional array that represents the navigation map
            int i = (y * navigationLayer->width) + x;

            //Gets the gid of the map in the navigation layer
            int gid = navigationLayer->Get(x, y);

            //If the gid is a blockedGid is an area that I cannot navigate, so is set in the navigation map as 0, all the other areas can be navigated
            //!!!! make sure that you assign blockedGid according to your map
            if (gid == blockedGid) navigationMap[i] = 0;
            else navigationMap[i] = 1;
        }
    }

    *buffer = navigationMap;
    width = mapData.width;
    height = mapData.height;

}

void Map::InitMap()
{
    //Calls the functon to load the map, make sure that the filename is assigned
    SString mapPath = path;
    mapPath += name;
    Load(mapPath);

    //Initialize the navigation map
    uchar* navigationMap = NULL;
    CreateNavigationMap(mapData.width, mapData.height, &navigationMap);
    pathfinding->SetNavigationMap((uint)mapData.width, (uint)mapData.height, navigationMap);
    RELEASE_ARRAY(navigationMap);

    active = true;
}

// L06: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    ret.x = x * mapData.tilewidth;
    ret.y = y * mapData.tileheight;

    return ret;
}
