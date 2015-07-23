void dma_copy(void *dst, void *src, short count);
void shootBullets(int gunLevel, short playerX, short playerY, short playerRotation, bullet *bullets, short maxBullets);
void spawnEnemy(short currentLevel, enemy *enemies, short maxEnemies);
void eraseEnemies(enemy *enemies, short maxEnemies, short framesPassed);
void eraseBullets(bullet *bullets, short maxBullets);
void moveEnemy(enemy *e, bullet *bullets, short maxBullets, short framesPassed, short gunDamage);
void drawEnemy(enemy *e, int erase);
void renderBullets(bullet *bullets, short maxBullets, short gunDamage);