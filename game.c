#include "mylib.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

#define COLOR(red, green, blue) (((red) & 0x1F) + (((green) & 0x1F) << 5) + (((blue) & 0x1F) << 10))

void shootBullets(int gunLevel, short playerX, short playerY, short playerRotation, bullet *bullets, short maxBullets)
{
	bullet newBullet = basicBullet;
	//places the bullet(s) in front of the players gun, which proved to be a complicated thing
	newBullet.x = playerX + playerWidth * (playerRotation <= 3) - 2 + 2 * (playerRotation <= 4 && playerRotation >= 1);
	newBullet.y = playerY + playerHeight * (playerRotation >= 2 && playerRotation <= 5) - 3 * (playerRotation == 7 || playerRotation <= 1)
																						- 2 * (playerRotation == 2)
																						- (playerRotation >= 3 && playerRotation <= 5)
																						+ 2 * (playerRotation == 6);
	if (gunLevel == 1 || gunLevel == 3)
	{
		newBullet.xVel = 4 * (playerRotation >= 1 && playerRotation <= 3) - 4 * (playerRotation >= 5);
		newBullet.yVel = 4 * (playerRotation >= 3 && playerRotation <= 5) - 4 * (playerRotation == 7 || playerRotation <= 1);
		//puts the bullet into the array of bullets
		if (inBounds(newBullet.x, newBullet.y, newBullet.width, newBullet.height))
		{
			newBullet.gone = 0;
			int i;
			for (i = 0; i < maxBullets && !bullets[i].gone; i++);
			if (i != maxBullets)
			{
				bullets[i] = newBullet;
			}
		}
	}
	if (gunLevel == 2 || gunLevel == 3)
	{
		bullet bulletTwo = basicBullet;
		//puts two bullets spraying out at an angle from the gun
		switch(playerRotation)
		{
			case 0:
				newBullet.xVel = -1;
				newBullet.yVel = -4;
				bulletTwo.xVel = 1;
				bulletTwo.yVel = -4;
				break;
			case 1:
				newBullet.xVel = 2;
				newBullet.yVel = -3;
				bulletTwo.xVel = 3;
				bulletTwo.yVel = -2;
				break;
			case 2:
				newBullet.xVel = 4;
				newBullet.yVel = -1;
				bulletTwo.xVel = 4;
				bulletTwo.yVel = 1;
				break;
			case 3:
				newBullet.xVel = 3;
				newBullet.yVel = 2;
				bulletTwo.xVel = 2;
				bulletTwo.yVel = 3;
				break;
			case 4:
				newBullet.xVel = 1;
				newBullet.yVel = 4;
				bulletTwo.xVel = -1;
				bulletTwo.yVel = 4;
				break;
			case 5:
				newBullet.xVel = -2;
				newBullet.yVel = 3;
				bulletTwo.xVel = -3;
				bulletTwo.yVel = 2;
				break;
			case 6:
				newBullet.xVel = -4;
				newBullet.yVel = 1;
				bulletTwo.xVel = -4;
				bulletTwo.yVel = -1;
				break;
			case 7:
				newBullet.xVel = -3;
				newBullet.yVel = -2;
				bulletTwo.xVel = -2;
				bulletTwo.yVel = -3;
				break;
		}
		bulletTwo.x = newBullet.x;
		bulletTwo.y = newBullet.y;
		//puts both bullets into the array
		if (inBounds(newBullet.x, newBullet.y, newBullet.width, newBullet.height))
		{
			newBullet.gone = 0;
			int i;
			for (i = 0; i < maxBullets && !bullets[i].gone; i++);
			if (i != maxBullets)
			{
				bullets[i] = newBullet;
			}
		}
		if (inBounds(bulletTwo.x, bulletTwo.y, bulletTwo.width, bulletTwo.height))
		{
			bulletTwo.gone = 0;
			int i;
			for (i = 0; i < maxBullets && !bullets[i].gone; i++);
			if (i != maxBullets)
			{
				bullets[i] = bulletTwo;
			}
		}
	}
}

void spawnEnemy(short currentLevel, enemy *enemies, short maxEnemies)
{
	enemy newEnemy = basicEnemy;
	if (rand() % 6 == 0 && currentLevel >= 2)
	{
		newEnemy = largeEnemy;
	}
	else if (rand() % 6 == 0 && currentLevel >= 3)
	{
		newEnemy = fastEnemy;
	}
	else if (rand() % 6 == 0 && currentLevel >= 4)
	{
		newEnemy = superEnemy;
	}
	else if (rand() % 6 == 0 && currentLevel >= 5)
	{
		newEnemy = giantEnemy;
	}
	else if (rand() % 6 == 0 && currentLevel >= 7)
	{
		newEnemy = insaneEnemy;
	}
	else if (rand() % 6 == 0 && currentLevel >= 9)
	{
		newEnemy = sprintEnemy;
	}
	if (rand() % 3 == 0)
	{
		int r = rand();
		newEnemy.x = (r % 2 == 0) * 240 - (r % 2 == 1) * newEnemy.width;
		newEnemy.y = r % 150 + 10;
	}
	else
	{
		int r = rand();
		newEnemy.x = r % 240;
		newEnemy.y = 10 + (r % 2 == 0) * 150 - (r % 2 == 1) * newEnemy.height;
	}
	newEnemy.xDist = 120 - (newEnemy.x + newEnemy.width / 2);
	newEnemy.yDist = 90 - (newEnemy.y + newEnemy.height / 2);
	newEnemy.gone = 0;
	int i;
	for (i = 0; i < maxEnemies && !enemies[i].gone; i++);
	if (i != maxEnemies)
	{
		enemies[i] = newEnemy;
	}
}

void eraseEnemies(enemy *enemies, short maxEnemies, short framesPassed)
{
	for (int i = 0; i < maxEnemies; i++)
	{
		if (!enemies[i].gone && (framesPassed % enemies[i].invVel == 0))
		{
			drawEnemy(&enemies[i], 1);
		}
	}
}

void eraseBullets(bullet *bullets, short maxBullets)
{
	for (int i = 0; i < maxBullets; i++)
	{
		if (!bullets[i].gone)
		{
			erase(bullets[i].x, bullets[i].y, bullets[i].width, bullets[i].height);
		}
	}
}

//returns 1 if the enemy was killed in moving
void moveEnemy(enemy *e, bullet *bullets, short maxBullets, short framesPassed, short gunDamage)
{
	e->x += (e->xDist * (framesPassed % e->invVel == 0) * 2 / (abs(e->xDist) + abs(e->yDist)));
	e->y += (e->yDist * (framesPassed % e->invVel == 0) * 2 / (abs(e->xDist) + abs(e->yDist)));
	e->xDist = 120 - (e->x + e->width / 2);
	e->yDist = 90 - (e->y + e->height / 2);
	for (int j = 0; j < maxBullets; j++)
	{
		if (!bullets[j].gone)
		{
			if (e->x + e->width > bullets[j].x && e->x < bullets[j].x + bullets[j].width
				&& e->y + e->height > bullets[j].y && e->y < bullets[j].y + bullets[j].height)
			{
				e->health -= gunDamage;
				bullets[j].gone = 1;
			}
		}
	}
}

void drawEnemy(enemy *e, int erased)
{
	int adjX = e->x;
	int adjY = e->y;
	int adjWidth = e->width;
	int adjHeight = e->height;
	if (adjX < 0)
	{
		adjWidth = adjWidth + adjX;
		adjX = 0;
	}
	else if (adjX + adjWidth >= 240)
	{
		adjWidth = 240 - adjX;
	}
	if (adjY < 10)
	{
		adjHeight = adjHeight + adjY - 10;
		adjY = 10;
	}
	else if (adjY + adjHeight >= 160)
	{
		adjHeight = 160 - adjY;
	}
	if (erased)
	{
		erase(adjX, adjY, adjWidth, adjHeight);
	}
	else
	{
		drawImage3(adjX, adjY, adjWidth, adjHeight, e->image);
	}
}

void renderBullets(bullet *bullets, short maxBullets, short gunDamage)
{
	for (int i = 0; i < maxBullets; i++)
	{
		if (!bullets[i].gone)
		{
			if (inBounds(bullets[i].x + bullets[i].xVel, bullets[i].y + bullets[i].yVel, bullets[i].width, bullets[i].height))
			{
				bullets[i].x += bullets[i].xVel;
				bullets[i].y += bullets[i].yVel;
				fillDiamond(bullets[i].x, bullets[i].y, bullets[i].width, COLOR(0x03 * (gunDamage + 2) > 0x1F ? 0x1F : 0x03 * (gunDamage + 2), 0x0F, 0x0F));
			}
			else
			{
				bullets[i].gone = 1;
			}
		}
	}
}