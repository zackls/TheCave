#include "mylib.h"
#include "game.h"
#include "gover.h"
#include "startscreen.h"
#include <stdlib.h>
#include <stdio.h>

#define REG_DISPCNT *(unsigned short*) 0x4000000
#define MODE_3 3
#define BG2_EN (1 << 10)

//colors
#define COLOR(red, green, blue) (((red) & 0x1F) + (((green) & 0x1F) << 5) + (((blue) & 0x1F) << 10))

//buttons
#define BUTTONS           *(volatile unsigned short*) 0x4000130
#define BUTTON_A          (1 << 0)
#define BUTTON_B          (1 << 1)
#define BUTTON_SELECT     (1 << 2)
#define BUTTON_START      (1 << 3)
#define BUTTON_RIGHT      (1 << 4)
#define BUTTON_LEFT       (1 << 5)
#define BUTTON_UP         (1 << 6)
#define BUTTON_DOWN       (1 << 7)
#define BUTTON_R          (1 << 8)
#define BUTTON_L          (1 << 9)

#define KEY_DOWN(key)			(~(BUTTONS) & key)
#define KEY_UP(key)				((BUTTONS) & key)
#define KEY_PRESSED(key, held)	KEY_DOWN(key) && !(held & key)
#define KEY_RELEASED(key, held)	KEY_UP(key) && !(held & key)

//game states
typedef enum {
	GOTOSTART,
	START,
	GOTOCREDITS,
	CREDITS,
	GOTOGAME,
	GAME,
	GOTOLEVELUP,
	LEVELUP,
	GOTOGAMEOVER,
	GAMEOVER
} GameState;

int main()
{
	REG_DISPCNT = MODE_3 | BG2_EN;
	GameState currentState = GOTOSTART;
	int currentSelection = 0;
	char buffer[10];
	volatile int genericDelayTimer = 0;
	volatile int shootTimer = 0;
	volatile int spawnTimer = 0;
	volatile int framesPassed = 0;
	unsigned short held = 0;
	short playerX = 240 / 2 - 2;
	short playerY = 160 / 2 - 3 + 30;
	short hitMiddle = 0;
	short hitPlayer = 0;
	short playerRotation = 0;
	short playerMoney = 0;
	short fireRate = 6;
	short gunLevel = 1;
	short gunDamage = 2;
	short enemiesKilled = 0;
	short currentLevel = 1;
	short maxBullets = 50;
	short maxEnemies = 30;
	bullet bullets[maxBullets];
	enemy enemies[maxEnemies];

	while (1)
	{
		waitForVBlank();

		//---START SCREEN---
		//draw start screen
		if (currentState == GOTOSTART)
		{
			playerX = 240 / 2 - 2;
			playerY = 160 / 2 - 3 + 30;
			hitMiddle = 0;
			hitPlayer = 0;
			playerRotation = 0;
			playerMoney = 0;
			fireRate = 6;
			gunLevel = 1;
			gunDamage = 2;
			enemiesKilled = 0;
			currentLevel = 1;
			for (int i = 0; i < maxBullets; i++)
			{
				bullets[i] = basicBullet;
			}
			for (int i = 0; i < maxEnemies; i++)
			{
				enemies[i] = basicEnemy;
			}
			clearScreen();
			drawImage3(0, 0, STARTSCREEN_WIDTH, STARTSCREEN_HEIGHT, startscreen);
			drawText(88, 15, "THE CAVE", COLOR(0x1F, 0x1F, 0x1F));
			drawText(20, 50, "Start", COLOR(0x13, 0x13, 0x13));
			drawText(20, 60, "Credits", COLOR(0x13, 0x13, 0x13));
			currentSelection = 0;
			fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0x1F, 0x1F, 0x1F));
			currentState = START;
		}
		//start screen
		else if (currentState == START)
		{
			if (genericDelayTimer < 10)
			{
				genericDelayTimer++;
			}
			if ((KEY_DOWN(BUTTON_DOWN) || KEY_DOWN(BUTTON_UP)) && genericDelayTimer >= 10)
			{
				//current selection = 0 for start, 1 for credits
				fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0, 0, 0));
				currentSelection = currentSelection ^ 1;
				fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0x1F, 0x1F, 0x1F));
				genericDelayTimer = 0;
			}
			if (KEY_PRESSED(BUTTON_A, held) && currentSelection)
			{
				currentState = GOTOCREDITS;
				genericDelayTimer = 0;
			}
			else if (KEY_PRESSED(BUTTON_A, held) && !currentSelection)
			{
				currentState = GOTOGAME;
				genericDelayTimer = 0;
			}
		}

		//---CREDITS---
		//draw credits
		else if (currentState == GOTOCREDITS)
		{
			clearScreen();
			drawText(20, 10, "I made this game:\n Zack Littke-Smith", COLOR(0, 0x1F, 0x1F));
			drawText(50, 50, "Back", COLOR(0, 0x1F, 0));
			fillDiamond(39, 49, 10, COLOR(0x1F, 0x1F, 0x1F));
			currentState = CREDITS;
		}
		//credits
		else if (currentState == CREDITS)
		{
			if (KEY_PRESSED(BUTTON_A, held))
			{
				currentState = GOTOSTART;
			}
		}

		//---GAME---
		//initialize game
		else if (currentState == GOTOGAME)
		{
			clearScreen();
			fillRect(0, 9, 240, 1, COLOR(0x10, 0x10, 0x10));
			drawImage3(240 / 2 - 8, 160 / 2 - 8 + 10, 16, 16, goldNugget);
			drawPlayer(playerX, playerY, playerRotation);
			sprintf(buffer, "Money:%d", playerMoney);
			drawText(8, 0, buffer, COLOR(0x13, 0x13, 0x13));
			sprintf(buffer, "Level:%d", currentLevel);
			drawText(168, 0, buffer, COLOR(0x13, 0x13, 0x13));
			currentState = GAME;
		}
		//game
		else if (currentState == GAME)
		{
			framesPassed++;

			//player movement
			if (genericDelayTimer < 7)
			{
				genericDelayTimer++;
			}
			//rotation
			if (KEY_DOWN(BUTTON_R) && genericDelayTimer >= 7 && !KEY_DOWN(BUTTON_L))
			{
				erase(playerX, playerY, playerWidth, playerHeight);
				playerRotation++;
				playerRotation = playerRotation % 8;
				drawPlayer(playerX, playerY, playerRotation);
				genericDelayTimer = 0;
			}
			else if (KEY_DOWN(BUTTON_L) && genericDelayTimer >= 7 && !KEY_DOWN(BUTTON_R))
			{
				erase(playerX, playerY, playerWidth, playerHeight);
				playerRotation += 7;
				playerRotation = playerRotation % 8;
				drawPlayer(playerX, playerY, playerRotation);
				genericDelayTimer = 0;
			}
			//position
			if (KEY_DOWN(BUTTON_UP) && inBounds(playerX, playerY - 1, playerWidth, playerHeight))
			{
				erase(playerX, playerY, playerWidth, playerHeight);
				playerY--;
				drawPlayer(playerX, playerY, playerRotation);
			}
			if (KEY_DOWN(BUTTON_DOWN) && inBounds(playerX, playerY + 1, playerWidth, playerHeight))
			{
				erase(playerX, playerY, playerWidth, playerHeight);
				playerY++;
				drawPlayer(playerX, playerY, playerRotation);
			}
			if (KEY_DOWN(BUTTON_LEFT) && inBounds(playerX - 1, playerY, playerWidth, playerHeight))
			{
				erase(playerX, playerY, playerWidth, playerHeight);
				playerX--;
				drawPlayer(playerX, playerY, playerRotation);
			}
			if (KEY_DOWN(BUTTON_RIGHT) && inBounds(playerX + 1, playerY, playerWidth, playerHeight))
			{
				erase(playerX, playerY, playerWidth, playerHeight);
				playerX++;
				drawPlayer(playerX, playerY, playerRotation);
			}

			//player shooting
			if (shootTimer < fireRate) {
				shootTimer++;
			}
			if (KEY_DOWN(BUTTON_A) && shootTimer >= fireRate) {
				shootBullets(gunLevel, playerX, playerY, playerRotation, bullets, maxBullets);
				//resets timer for correctly timed shooting
				shootTimer = 0;
			}

			//spawn enemies
			if (spawnTimer < 200)
			{
				spawnTimer++;
			}
			srand(framesPassed);
			if (spawnTimer >= 150 + rand() % 50 - currentLevel * 10)
			{
				spawnEnemy(currentLevel, enemies, maxEnemies);
				spawnTimer = 0;
			}

			//erase stuff
			eraseEnemies(enemies, maxEnemies, framesPassed);
			eraseBullets(bullets, maxBullets);

			//render stuff
			for (int i = 0; i < maxEnemies; i++)
			{
				if (!enemies[i].gone)
				{
					moveEnemy(&enemies[i], bullets, maxBullets, framesPassed, gunDamage);
					//check if enemy is dead
					if (enemies[i].health <= 0)
					{
						enemies[i].gone = 1;
						sprintf(buffer, "Money:%d", playerMoney);
						drawText(8, 0, buffer, COLOR(0, 0, 0));
						playerMoney += enemies[i].reward;
						sprintf(buffer, "Money:%d", playerMoney);
						drawText(8, 0, buffer, COLOR(0x13, 0x13, 0x13));
						enemiesKilled++;
						erase(enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height);
					}
					else
					{
						//check if the enemy is touching the middle piece
						if (enemies[i].x + enemies[i].width > 112 && enemies[i].x < 128 && enemies[i].y + enemies[i].height > 82 && enemies[i].y < 98)
						{
							hitMiddle = 1;
							currentState = GOTOGAMEOVER;
							genericDelayTimer = 0;
						}
						//check if the enemy is touching the player
						if (enemies[i].x + enemies[i].width > playerX - 1 && enemies[i].x < playerX + playerWidth + 1
								&& enemies[i].y + enemies[i].height > playerY - 1 && enemies[i].y < playerY + playerHeight + 1)
						{
							hitPlayer = 1;
							currentState = GOTOGAMEOVER;
							genericDelayTimer = 0;
						}
						drawEnemy(&enemies[i], 0);
					}
				}
			}
			renderBullets(bullets, maxBullets, gunDamage);

			//check for level up
			if (enemiesKilled == currentLevel * 30 - 5)
			{
				currentState = GOTOLEVELUP;
			}
		}

		//---LEVEL UP---
		//ingame level up
		else if (currentState == GOTOLEVELUP)
		{
			erase(0, 10, 240, 150);
			for (int i = 0; i < maxBullets; i++)
			{
				bullets[i] = basicBullet;
			}
			for (int i = 0; i < maxEnemies; i++)
			{
				enemies[i] = basicEnemy;
			}
			sprintf(buffer, "Level:%d", currentLevel);
			drawText(168, 0, buffer, COLOR(0, 0, 0));
			currentLevel++;
			sprintf(buffer, "Level:%d", currentLevel);
			drawText(168, 0, buffer, COLOR(0x13, 0x13, 0x13));
			drawText(88, 25, "Level Up!", 0x035F);
			drawText(20, 40, "Upgrade:", COLOR(0x13, 0x13, 0x13));
			drawText(140, 40, "Cost:", COLOR(0x13, 0x13, 0x13));
			drawText(20, 50, "Fire Rate", COLOR(0x13, 0x13, 0x13));
			if (fireRate <= 3)
			{
				sprintf(buffer, "MAX");
			}
			else
			{
				sprintf(buffer, "%d", 25 * (7 - fireRate));
			}
			drawText(140, 50, buffer, COLOR(0x13, 0x13, 0x13));
			drawText(20, 60, "Gun Level", COLOR(0x13, 0x13, 0x13));
			if (gunLevel >= 3)
			{
				sprintf(buffer, "MAX");
			}
			else
			{
				sprintf(buffer, "%d", 75 * gunLevel);
			}
			drawText(140, 60, buffer, COLOR(0x13, 0x13, 0x13));
			drawText(20, 70, "Gun Damage", COLOR(0x13, 0x13, 0x13));
			sprintf(buffer, "%d", 100 * (gunDamage - 1));
			drawText(140, 70, buffer, COLOR(0x13, 0x13, 0x13));
			drawText(20, 80, "Exit", COLOR(0x13, 0x13, 0x13));
			fillDiamond(10, 49, 10, COLOR(0x1F, 0x1F, 0x1F));
			genericDelayTimer = 0;
			currentSelection = 0;
			currentState = LEVELUP;
		}
		else if (currentState == LEVELUP)
		{
			if (genericDelayTimer < 10)
			{
				genericDelayTimer++;
			}
			if (KEY_DOWN(BUTTON_DOWN) && genericDelayTimer >= 10)
			{
				//current selection = 0 for fire rate, 1 for gun level, 2 for gun damage, and 3 for exit
				fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0, 0, 0));
				currentSelection = (currentSelection + 1) % 4;
				fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0x1F, 0x1F, 0x1F));
				genericDelayTimer = 0;
			}
			else if (KEY_DOWN(BUTTON_UP) && genericDelayTimer >= 10)
			{
				fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0, 0, 0));
				currentSelection = (currentSelection + 3) % 4;
				fillDiamond(10, 49 + currentSelection * 10, 10, COLOR(0x1F, 0x1F, 0x1F));
				genericDelayTimer = 0;
			}
			if (KEY_PRESSED(BUTTON_A, held) && currentSelection == 0 && genericDelayTimer >= 10 && playerMoney >= 25 * (7 - fireRate) && fireRate >= 4)
			{
				sprintf(buffer, "Money:%d", playerMoney);
				drawText(8, 0, buffer, COLOR(0, 0, 0));
				playerMoney -= 25 * (7 - fireRate);
				sprintf(buffer, "Money:%d", playerMoney);
				drawText(8, 0, buffer, COLOR(0x13, 0x13, 0x13));
				sprintf(buffer, "%d", 25 * (7 - fireRate));
				drawText(140, 50, buffer, COLOR(0, 0, 0));
				fireRate--;
				if (fireRate <= 3)
				{
					sprintf(buffer, "MAX");
				}
				else
				{
					sprintf(buffer, "%d", 20 * (7 - fireRate));
				}
				drawText(140, 50, buffer, COLOR(0x13, 0x13, 0x13));
				genericDelayTimer = 0;
			}
			else if (KEY_PRESSED(BUTTON_A, held) && currentSelection == 1 && genericDelayTimer >= 10 && playerMoney >= 75 * gunLevel && gunLevel <= 2)
			{
				sprintf(buffer, "Money:%d", playerMoney);
				drawText(8, 0, buffer, COLOR(0, 0, 0));
				playerMoney -= 75 * gunLevel;
				sprintf(buffer, "Money:%d", playerMoney);
				drawText(8, 0, buffer, COLOR(0x13, 0x13, 0x13));
				sprintf(buffer, "%d", 75 * gunLevel);
				drawText(140, 60, buffer, COLOR(0, 0, 0));
				gunLevel++;
				if (gunLevel >= 3)
				{
					sprintf(buffer, "MAX");
				}
				else
				{
					sprintf(buffer, "%d", 75 * gunLevel);
				}
				drawText(140, 60, buffer, COLOR(0x13, 0x13, 0x13));
				genericDelayTimer = 0;
			}
			else if (KEY_PRESSED(BUTTON_A, held) && currentSelection == 2 && genericDelayTimer >= 10 && playerMoney >= 100 * (gunDamage - 1))
			{
				sprintf(buffer, "Money:%d", playerMoney);
				drawText(8, 0, buffer, COLOR(0, 0, 0));
				playerMoney -= 100 * (gunDamage - 1);
				sprintf(buffer, "Money:%d", playerMoney);
				drawText(8, 0, buffer, COLOR(0x13, 0x13, 0x13));
				sprintf(buffer, "%d", 100 * (gunDamage - 1));
				drawText(140, 70, buffer, COLOR(0, 0, 0));
				gunDamage++;
				sprintf(buffer, "%d", 100 * (gunDamage - 1));
				drawText(140, 70, buffer, COLOR(0x13, 0x13, 0x13));
				genericDelayTimer = 0;
			}
			else if (KEY_PRESSED(BUTTON_A, held) && currentSelection == 3 && genericDelayTimer >= 10)
			{
				currentState = GOTOGAME;
				genericDelayTimer = 0;
			}
		}

		//---GAME OVER---
		//draw game over screen after pausing to allow the player to see what went wrong, indicated by a red rectangle
		else if (currentState == GOTOGAMEOVER)
		{
			if (hitPlayer)
			{
				fillRect(playerX - 1, playerY, playerHeight, playerHeight, COLOR(0x1F, 0, 0));
			}
			if (hitMiddle)
			{
				fillRect(112, 82, 16, 16, COLOR(0x1F, 0, 0));
			}
			if (genericDelayTimer < 60)
			{
				genericDelayTimer++;
			}
			if (genericDelayTimer >= 60)
			{
				currentState = GAMEOVER;
				clearScreen();
				drawImage3(0, 0, GOVER_WIDTH, GOVER_HEIGHT, gover);
				drawText(84, 10, "GAME OVER", COLOR(0x1F, 0, 0));
				sprintf(buffer, "You made it all the way\nto level %d!", currentLevel);
				drawText(20, 30, buffer, COLOR(0x13, 0x13, 0x13));
				fillDiamond(20, 60, 10, COLOR(0x1F, 0x1F, 0x1F));
				drawText(30, 60, "Back", COLOR(0x13, 0x13, 0x13));
			}
			framesPassed = 0;

		}
		//return to start menu when A is pressed
		else if (currentState == GAMEOVER)
		{
			if (KEY_PRESSED(BUTTON_A, held))
			{
				currentState = GOTOSTART;
			}
		}


		//return to start screen if select is pressed at any time
		if (KEY_PRESSED(BUTTON_SELECT, held))
		{
			currentState = GOTOSTART;
		}

		held = ~BUTTONS;
	}
	return 0;
}