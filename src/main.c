/**
 * Source for main, see README.md for more info
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "collection.h"
#include "common.h"
#include "rlutil.h"

#define DEBUG 0

// FPS

static const int UP = 'w';
static const int DOWN = 's';
static const int LEFT = 'a';
static const int RIGHT = 'd';
static const int QUIT = 'q';
static const int FIRE = ' ';

static const int FRAME_RATE = 60;

static const float X_CORRECTION = 2;
static const float Y_CORRECTION = 1;

static const int X_MOVE_OFFSET = 2;
static const int Y_MOVE_OFFSET = 1;
static const int LEVEL_PAUSE = 130.0; // seems long enough

typedef enum {
	X, Y
} Axis;

typedef struct Player {
	int x;
	int y;
	char c;
	long life;
	long hp;
	int xLength;
	int yLength;
	int ammo;
	bool dep;
} Player;

typedef void (*StepFunction)(void *self, void *world);

typedef enum {
	DAMAGE = 1, DEATH = 2, HP = 3, LIFE = 4, GAME_OVER = 5, PLAYER_AMMO = 6
} BulletType;

typedef struct Bullet {
	double x;
	double y;
	double theta;
	double velocity; // in rad/frame
	BulletType type;
	char c;
	long life;
	StepFunction sFn;
	bool recycled;

} Bullet;

typedef struct Level {
	long level; // and hope this does not overflow
	double speed;
	long ammo;
	long timer;
} Level;

typedef struct World {
	int width;
	int height;
	Level level;
	Player player;
	LinkedList bullets;
	long frame;
	long millis;

	int interval;
	int counter;

} World;

double checkAndSnapBound(World *world, Axis axis, double value) {
	if (value <= 1) {
		return 1;
	}
	int max = axis == X ? world->width : world->height;
	if (value >= max - 1) {
		return max - 1;
	}
	return value;
}

void constantVelocity(Bullet *bullet, World *world) {
}

void increasingVelocity(Bullet *bullet, World *world) {
	bullet->velocity *= (1.0 + 0.0025);
}

void decreasingVelocity(Bullet *bullet, World *world) {
	bullet->velocity *= (1.0 - 0.0025);
}

void tracing(Bullet *bullet, World *world) {

	//bullet->velocity *= (1.0 - 0.002);

	double deltaX = (double) world->player.x - bullet->x;
	double deltaY = (double) bullet->y - world->player.y;
	double thetaCorrection = toDeg(atan2(deltaY, deltaX));

	bullet->theta = thetaCorrection;

}

void timeTrigeredPattern() {

}

BulletType randomBulletType() {
	return randRange(DAMAGE, GAME_OVER);
}

void quit(World *world) {
	//cls();
	locate(1, 1);
	setColor(WHITE);
	printf("GAME OVER\nRESULTS:[LEVEL: %d HP:%d LIFE:%d]\n", world->level.level,
			world->player.hp, world->player.life);
	showcursor();
	exit(EXIT_SUCCESS);
}

void nextLevel(World *world) {
	world->level.level++;
	world->level.ammo = world->level.level * 1000;

	freeList(&world->bullets);
	world->bullets = newList(sizeof(Bullet));
	world->player.ammo = world->level.level * 10;
	world->level.timer = -1;

}

bool levelFinished(World *world) {
	if (world->level.timer >= 0) {
		if (world->level.timer >= LEVEL_PAUSE) {
			return true;
		}
		world->level.timer++;
		return false;
	}

	if (world->level.ammo != 0)
		return false;
	Node *node = world->bullets.head;
	while (node != NULL) {
		Bullet *bullet = ((Bullet*) node->data);
		if (!bullet->recycled)
			return false;
		node = node->next;
	}
	world->level.timer = 0;
	return false;
}

bool playerAlive(World *world) {
	return !(world->player.hp <= 0 || world->player.life <= 0);
}

void checkAndProcessCollision(World *world) {
	Node *node = world->bullets.head;
	while (node != NULL) {
		Bullet *bullet = ((Bullet*) node->data);
		if (!bullet->recycled && (bullet->type != PLAYER_AMMO)) {

			if ((bullet->type == PLAYER_AMMO)) {

			} else {
				int xOffset = world->player.xLength / 2;
				int yOffset = world->player.yLength / 2;
				int bX = (bullet->x);
				int bY = (bullet->y);
				int pX = world->player.x;
				int pY = world->player.y;
				if (((pX - xOffset) <= bX && bX <= (pX + xOffset))
						&& ((pY - yOffset) <= bY && bY <= (pY + yOffset))) {
					bullet->recycled = true;
					switch (bullet->type) {
					case DAMAGE:
						world->player.hp -= (world->level.level * 1000);
						break;
					case DEATH:
						world->player.life--;
						break;
					case HP:
						world->player.hp += (world->level.level * 500);
						break;
					case LIFE:
						world->player.life++;
						break;
					case GAME_OVER:
						world->player.life -= 3;
						break;
					}
				}
			}
		}
		node = node->next;
	}
}

void updateWorld(World *world) {
	world->width = tcols();
	world->height = trows();
	world->frame++;
	world->millis += FRAME_RATE;
}

void renderHUD(World *world) {
	// move to bottom left

	bool levelWait = world->level.timer != -1;

	locate(1, world->height - (levelWait ? 2 : 1));
	setColor(WHITE);
	if (levelWait)
		printf("NEXT LEVEL IN %d\n", (LEVEL_PAUSE - world->level.timer));
	printf(
			"LEVEL: %d HP:%d LIFE:%d BULLET:%ld AMMO: %d\nRES=[%dx%d], [q] to EXIT",
			world->level.level, world->player.hp, world->player.life,
			world->level.ammo, world->player.ammo, world->width, world->height);

}

void renderDebug(World *world) {
	locate(1, 1);
	setColor(WHITE);
	printf("FPS:%d FRAME:%ld ELAPSED MS: %ld", FRAME_RATE, world->frame,
			world->millis);
}

typedef void (*DeployTemplateFn)(Bullet *previous, Bullet *bullet, World *world,
		int n, int max, int rand);

inline double randVelocity(World *world) {

	return randRange(5, 10) / 15.0 + ((double) world->level.level / 80);
}

void playerCircleTemplate(Bullet *previous, Bullet *bullet, World *world, int n,
		int max, int rand) {
	bullet->x = world->player.x;
	bullet->y = world->player.y;
	bullet->theta = wrapDegree(n * (360.0 / max) + rand);
	bullet->velocity =
			(previous == NULL) ? randVelocity(world) : previous->velocity;
	bullet->sFn = decreasingVelocity;
}

void circleTemplate(Bullet *previous, Bullet *bullet, World *world, int n,
		int max, int rand) {
	bullet->x = (world->width / 2) + rand;
	bullet->y = world->height / 4;
	bullet->theta = wrapDegree(n * 10 + rand);
	bullet->velocity =
			(previous == NULL) ? randVelocity(world) : previous->velocity;
	bullet->sFn = decreasingVelocity;
}

void hvExplosionTemplate(Bullet *previous, Bullet *bullet, World *world, int n,
		int max, int rand) {
	bullet->x = (world->width / 2) + rand;
	bullet->y = world->height / 4;
	bullet->theta = wrapDegree(n * 10 + rand);
	bullet->velocity = randRange(6, 20) / 9.50 + ((double) world->level.level / 80);
	bullet->sFn = decreasingVelocity;
}

void spiralTemplate(Bullet *previous, Bullet *bullet, World *world, int n,
		int max, int rand) {

	int shift = rand > 0 ? -2 : 2;

	bullet->x =
			previous == NULL ? (world->width / 2 + rand) : previous->x + shift;
	bullet->y = previous == NULL ? (world->height / 4) : previous->y + shift;
	bullet->theta = wrapDegree(n * 10 + rand);
	bullet->velocity =
			(previous == NULL) ? randVelocity(world) : previous->velocity;
	bullet->sFn = increasingVelocity;
}

void horizontalTemplate(Bullet *previous, Bullet *bullet, World *world, int n,
		int max, int rand) {
	bullet->x = round(
			((double) world->width
					/ (max >= world->width ? world->width / 2 : max)) * n);
	bullet->y = randRange(2, 10);
	bullet->theta = 0; // 0 degree does down
	bullet->velocity =
			(previous == NULL) ? randVelocity(world) : previous->velocity;
	bullet->sFn = increasingVelocity;
}

DeployTemplateFn *randomTemplate() {
	int (*p[4])();
	p[0] = circleTemplate;
	p[1] = spiralTemplate;
	p[2] = horizontalTemplate;
	p[3] = hvExplosionTemplate;
	return *p[3];
}

void deployPlayerBullet(World *world) {

	if (world->player.dep) {
		world->player.dep = false;
	} else {
		return;
	}

	if (world->player.ammo > 0) {
		world->player.ammo--;
	} else {
		return;
	}
	//locate(1, 4);
	//printf("ACTUAL %d ammo:%ld\n", 6, world->level.ammo);
	int count = randRange(3, (int) world->width * (1.0 / 3.0));
	if (world->level.ammo < count) {
		count = world->level.ammo;
	}
	world->level.ammo -= count;
	int i;

	Bullet *bullet = malloc(sizeof(Bullet) * count);
	int range = world->width / 2;
	int randDelta = randRange(-range, range);

	for (i = 0; i < count; i++) {
		playerCircleTemplate(i == 0 ? NULL : &bullet[i - 1], &bullet[i], world,
				i, count, randDelta);
		bullet[i].c = 'o';
		bullet[i].life = 0;
		bullet[i].recycled = false;
		bullet[i].type = PLAYER_AMMO;
		appendList(&world->bullets, &bullet[i]);
	}
}

void deployBullet(World *world, BulletType type) {

	if (world->counter < world->interval) {
		world->counter++;
		return;
	} else {
		world->counter = 0;
		world->interval = randRange(0, (int) (50.0 / world->level.level));
	}

	if (world->level.ammo == 0) {
		// ammo depleted
		return;
	}

	int count = randRange(15, (int) world->width * (1.0 / 3.0));
	if (world->level.ammo < count) {
		count = world->level.ammo;
	}
	world->level.ammo -= count;
	int i;
	// gotta make space for our bullets :)
	Bullet *bullet = malloc(sizeof(Bullet) * count);
	int range = world->width / 2;
	int randDelta = randRange(-range, range);

	DeployTemplateFn templateFn = randomTemplate();

	for (i = 0; i < count; i++) {
		templateFn(i == 0 ? NULL : &bullet[i - 1], &bullet[i], world, i, count,
				randDelta);
		bullet[i].c = 'o';
		bullet[i].life = 0;
		bullet[i].recycled = false;
		bullet[i].type = type;
		appendList(&world->bullets, &bullet[i]);
	}
}

//bool forEachBullet(void *data) {
//	Bullet *bullet = ((Bullet*) data);
//	return true;
//}

void renderBullet(World *world) {

	Node *node = world->bullets.head;
	while (node != NULL) {
		Bullet *bullet = ((Bullet*) node->data);

		if (!bullet->recycled) {

			// move
			bullet->y += bullet->velocity * cos(toRad(bullet->theta))
					* Y_CORRECTION;
			bullet->x += bullet->velocity * sin(toRad(bullet->theta))
					* X_CORRECTION;

			bullet->sFn(bullet, world);
			bullet->life++;

			if ((checkAndSnapBound(world, X, bullet->x) != bullet->x
					|| checkAndSnapBound(world, Y, bullet->y) != bullet->y)) {
				bullet->recycled = true;
			} else {
				locate(round(bullet->x), round(bullet->y));
				int colour;
				switch (bullet->type) {
				case DAMAGE:
					colour = YELLOW;
					break;
				case HP:
					colour = GREEN;
					break;
				case LIFE:
					colour = LIGHTGREEN;
					break;
				case DEATH:
					colour = LIGHTRED;
					break;
				case GAME_OVER:
					colour = RED;
					break;
				case PLAYER_AMMO:
					colour = WHITE;
					break;
				}

				setColor(colour);
				//printf("%c [%ld](%f,%f)", bullet->c, bullet->life, bullet->x, bullet->y);
				printf("%c", bullet->c);
			}
		}

		node = node->next;
	}

}

void handleInput(World *world) {
	if (!kbhit())
		return;
	char key = getch();

	Player *player = &world->player;
	if (key == FIRE) {
		player->dep = true;

	} else if (key == UP) {
		player->y -= Y_MOVE_OFFSET;
	} else if (key == DOWN) {
		player->y += Y_MOVE_OFFSET;
	} else if (key == LEFT) {
		player->x -= X_MOVE_OFFSET;
	} else if (key == RIGHT) {
		player->x += X_MOVE_OFFSET;
	} else if (key == QUIT) {
		quit(&world);
	} else {
		// TODO handle bad input
	}

	player->x = checkAndSnapBound(world, X, player->x);
	player->y = checkAndSnapBound(world, Y, player->y);
}

void setupPlayer(World *world) {
	world->player.x = world->width / 2;
	world->player.y = (int) world->height * (3.0 / 4.0);
}

void renderPlayer(World *world) {
	// handle input
	locate(world->player.x, world->player.y);
	setColor(WHITE);
	//printf("%c [%d,%d]", world->player.c, world->player.x, world->player.y);
	printf("%c", world->player.c);
}

void renderWelcome(World *world) {
	locate(1, 1);
	printf("Welcome to touhouC, a bullet hell game implemented in console.\n");
	printf(
			"Dodge bullets! some bullets are good so you might want to look for them.\n");
	printf(
			"Use the standard [W, S, A, D] for [UP, DOWN, LEFT, RIGHT] and [q] to exit at any point.\n");
	printf("Press any key to start....\n");
	anykey();
}

int main(int argc, char **argv) {
	srand(time(NULL)); // setup random
	hidecursor();

	cls();
	World world = { .player = { .x = 1, .y = 1, .c = 'X', .hp = 70000, .life =
			100, .xLength = 2, .yLength = 2 }, .level = { .level = 1 },
			.bullets = newList(sizeof(Bullet)) };

	updateWorld(&world);

	renderWelcome(&world);

	setupPlayer(&world);

	nextLevel(&world);

	while (true) {
		msleep(1000 / FRAME_RATE);
		cls();
		//printf("\033[2J\033[H"); // cls suck

		updateWorld(&world);

		handleInput(&world);
		renderPlayer(&world);

		deployBullet(&world, randomBulletType());
		deployPlayerBullet(&world);

		renderBullet(&world);
		checkAndProcessCollision(&world);
		renderHUD(&world);
		renderDebug(&world);

		if (!playerAlive(&world)) {
			quit(&world);
		}

		if (levelFinished(&world)) {
			nextLevel(&world);
		}

		fflush(stdout);
	}

	quit(&world);
	return 0;
}

