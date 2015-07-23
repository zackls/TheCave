unsigned short *videoBuffer;
const unsigned short goldNugget[256];
const unsigned short playerWidth;
const unsigned short playerHeight;
typedef struct
{
	int x;
	int y;
	int xVel;
	int yVel;
	int width;
	int height;
	//if the bullet is "gone" or not, bullets which are gone aren't drawn or otherwise dealt with
	int gone;
} bullet;
const bullet basicBullet;
typedef struct
{
	int x;
	int y;
	//inverse of the velocity, since floats shouldnt be used and enemies move slower than 1 px/second
	int invVel;
	int xDist;
	int yDist;
	int width;
	int height;
	int health;
	int reward;
	int gone;
	const unsigned short *image;
} enemy;
//base definitions of different enemy structs
const enemy basicEnemy;
const enemy largeEnemy;
const enemy fastEnemy;
const enemy superEnemy;
const enemy giantEnemy;
const enemy insaneEnemy;
const enemy sprintEnemy;
void fillRect(int x, int y, int width, int height, unsigned short color);
void fillDiamond(int x, int y, int size, unsigned short color);
void fillPixel(int x, int y, unsigned short color);
void clearScreen();
void erase(int x, int y, int width, int height);
void drawImage3(int x, int y, int width, int height, const unsigned short *image);
void drawImage(int x, int y, int width, int height, const unsigned short *image);
void drawPlayer(int x, int y, unsigned short turnAmount);
void drawText(int x, int y, char *str, unsigned short color);
int inBounds(int x, int y, int width, int height);
void waitForVBlank();