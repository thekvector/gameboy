#include "myLib.h"
#include "text.h"
#include "bird.h"
#include "pipeBottom.h"
#include "pipeTop.h"
#include "startScreen.h"
#include "background.h"
#include "endScreen.h"
#include "stdio.h"

typedef struct {
    int row;
    int col;
} BIRD;
	
typedef struct {
	int topHeight;
	int bottomHeight;
	int col;
} PIPE;

enum GBAState {
	START,
	START_STAY_SAME,
	PLAY_SETUP,
	PLAY,
	END,
	END_STAY_SAME
};

void initialize();
void drawBird();
void undrawBird(const u16* image);
int alive();
void makePipeHeight(PIPE *pipe);
void drawPipe(PIPE *pipe);
void undrawPipe(const u16* image);
void movePipes();
void incrementScore();
int checkCollisions();

const int pipeGap = BIRD_HEIGHT * 3;
int pipeWidth = BIRD_WIDTH * 1.25;

const int numPipes = 4;

const int birdWidth = BIRD_WIDTH;
const int birdHeight = BIRD_HEIGHT;

BIRD gameBird;
PIPE pipe1;
PIPE pipe2;
PIPE pipe3;

volatile int score = 0;

int main() {
	REG_DISPCNT = MODE3 | BG2_ENABLE;

	enum GBAState state = START;

	while(1) {
		waitForVblank();
		switch(state) {
			case START:
				drawImage3(0, 0, FLAPPYSTART_WIDTH, FLAPPYSTART_HEIGHT, flappyStart);
				drawString(20, 60, "Press Start to play!", WHITE);
				state = START_STAY_SAME;
				initialize();
				break;
			case START_STAY_SAME:
				if (KEY_DOWN_NOW(BUTTON_START)) {
					state = PLAY_SETUP;
				}
				break;
			case PLAY_SETUP:
				drawImage3(0, 0, FLAPPYBACKGROUND_WIDTH, FLAPPYBACKGROUND_HEIGHT, flappyBackground);
				state = PLAY;
				break;		
			case PLAY:
				undrawBird(flappyBackground);
				undrawPipe(flappyBackground);
				movePipes();
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					state = START;
				}
				if (KEY_DOWN_NOW(BUTTON_UP)) {
					gameBird.row = gameBird.row - 5;
				} else {
					gameBird.row = gameBird.row + 2;
				}
				if (!alive()) {
					state = END;
					break;
				}
				char currScore[3];
				drawString(0, 0, currScore, WHITE);
				drawBird();
				drawPipe(&pipe1);
				drawPipe(&pipe2);
				drawPipe(&pipe3);
				sprintf(currScore, "Score: %d", score);
				incrementScore();
				delay(3);
				break;
			case END:
				drawImage3(0, 0, FLAPPYGAMEOVER_WIDTH, FLAPPYGAMEOVER_HEIGHT, flappyGameOver);
				char scoreString[3];
				sprintf(scoreString,"Game over :(. Your score was %d.", score);
				drawString(8, 8, scoreString, GREEN);
				drawString(120, 50, "Press START to RESTART", WHITE);
				drawString(140, 50, "Press SELECT for MAIN MENU", WHITE);
				state = END_STAY_SAME;
				break;
			case END_STAY_SAME:
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					state = START;
				}
				if (KEY_DOWN_NOW(BUTTON_START)) {
					initialize();
					state = PLAY_SETUP;
				}
				break;
		}
	}
}

void initialize() {
	score = 0;

	gameBird.col = 20;
	gameBird.row = 60;

	pipe1.col = 140;
	pipe2.col = 230;
	pipe3.col = 320;
	makePipeHeight(&pipe1);
	makePipeHeight(&pipe2);
	// makePipeHeight(&pipe3);
}

void drawBird() {
	drawImage3(gameBird.row, gameBird.col, birdWidth, birdHeight, bird);
}

void undrawBird(const u16* image) {
	undrawImage3(gameBird.row, gameBird.col, birdWidth, birdHeight, image);
}

void undrawPipe(const u16* image) {
	undrawImage3(0, pipe1.col, pipeWidth, pipe1.topHeight, image);
	undrawImage3(pipe1.topHeight + pipeGap, pipe1.col, pipeWidth, pipe1.bottomHeight, image);
	undrawImage3(0, pipe2.col, pipeWidth, pipe2.topHeight, image);
	undrawImage3(pipe2.topHeight + pipeGap, pipe2.col, pipeWidth, pipe2.bottomHeight, image);
	undrawImage3(0, pipe3.col, pipeWidth, pipe3.topHeight, image);
	undrawImage3(pipe3.topHeight + pipeGap, pipe3.col, pipeWidth, pipe3.bottomHeight, image);
}
int alive() {
	if (checkCollisions()) {
		return 0;
	}
	if (gameBird.row > 0 && gameBird.row < 160 - birdHeight) {
		return 1;
	} 
	return 0;
}

void makePipeHeight(PIPE *pipe) {
	pipe -> topHeight = rand() % (birdHeight * 5) + (birdHeight * 3);
	pipe -> bottomHeight = 160 - (pipe -> topHeight) - pipeGap;
}

void drawPipe(PIPE *pipe) {
	if (pipe -> col >= 240) {
		makePipeHeight(pipe);
		return;
	} else if (pipe -> col < 5) {
		pipe -> col = 260;
		return;
	}
	drawImage3(0, pipe -> col, pipeWidth, pipe -> topHeight, pipeTop);
    drawImage3((pipe -> topHeight + pipeGap), pipe -> col, pipeWidth, pipe -> bottomHeight, pipeBottom);	
	//drawRect((pipe -> topHeight + pipeGap), pipe -> col, pipe -> bottomHeight, pipeWidth, GREEN);
}

void movePipes() {
	pipe1.col = pipe1.col - 3;
	pipe2.col = pipe2.col - 3;
	pipe3.col = pipe3.col - 3;
}

void incrementScore() {
	if (gameBird.col == pipe1.col) {
		score++;
	}
	if (gameBird.col == pipe2.col) {
		score++;
	}
	if (gameBird.col == pipe3.col) {
		score++;
	}
}

int checkCollisions() {
	// for (int i = 0; i < pipeWidth; i++) {
		if ((gameBird.col >= pipe1.col && gameBird.col <= pipe1.col + pipeWidth) &&
		 (gameBird.row < pipe1.topHeight || gameBird.row > (pipe1.topHeight + pipeGap))) {
			return 1;
		} else if ((gameBird.col >= pipe2.col && gameBird.col <= pipe2.col + pipeWidth) && 
		(gameBird.row < pipe2.topHeight || gameBird.row > (pipe2.topHeight + pipeGap))) {
			return 1;
		} else if ((gameBird.col >= pipe3.col && gameBird.col <= pipe3.col + pipeWidth) && 
			(gameBird.row < pipe3.topHeight || gameBird.row > (pipe3.topHeight + pipeGap))) {
			return 1;
		}
	// }
	return 0;
}