#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;

enum sates {
	IDLE,
	WALIKING,
	DIE,
	JUMP1,
	JUMP2
};

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void respawn();

	// obtein the current tile which player stays
	int getPlayerTileX();

	int getPlayerTileY();

	bool LoadState(pugi::xml_node& node);

	bool SaveState(pugi::xml_node& node);

	void SetPlayerPos(int x, int y);

	void respawnlastcheckpoint();

public:

	//L02: DONE 2: Declare player parameters
	float speed;
	SDL_Texture* texture = NULL;
	SDL_Texture* blendTexture = NULL;
	int blenditerator = 0;
	pugi::xml_node config;
	uint texW, texH;
	iPoint Sposition;

	Animation* currentSAnimation = nullptr;
	Animation* currentAnimation = nullptr;
	// A set of animations
	Animation idleAnim;
	Animation walkAnim;
	Animation dieAnim;
	Animation jumpAnim1;
	Animation jumpAnim2;
	Animation blendFadeIN;
	Animation blendFadeOut;
	Animation shurikenanim;
	bool flip;
	bool dark;
	bool blend = false;
	//Audio fx
	int pickCoinFxId = -1;
	int victory = -1;
	int checkfx = -1;

	// L07 DONE 5: Add physics to the player - declare a Physics body
	PhysBody* pfeet = NULL;
	PhysBody* pbodyshuriken = NULL;
	bool shuriken = false;
	int maxremainingJumpSteps = 30;
	int remainingJumpSteps = 0;
	float jumpForceReduce = 0;
	int jumps = 2;

	bool godmode;
	sates state;
	bool death;
	iPoint initPosition;
	iPoint lastcheckpoint;
	iPoint lastcheckpoint2;
	float force;
	fPoint direction;

	int icheckiterator = 0;

	bool fxbossplayed = false;
	bool bossing = false;
	bool fight = false;

	iPoint checkpointposlvl1[3];
	iPoint checkpointposlvl2;
	int lives = 3;
	SString textLives;
	SDL_Texture* wonTexture = NULL;
	SDL_Texture* loseTexture = NULL;

	int conins = 0;

	bool won = false;
	bool lose = false;
};

#endif // __PLAYER_H__