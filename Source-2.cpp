#include <windows.h>	// Header file for the Windows Library
#include <gl/gl.h>	// Header file for the OpenGL32 Library
#include <gl/glu.h>	// Header file for the GlU32 Library
#include <gl/glut.h>	// Header file for the Glut32 Library
#include <math.h>	// Header file for the Math Library
#include<time.h>
#include<stdlib.h>//header for c++ and c
#include<string>
#include <stdio.h>
#include<iostream>
#include <MMSystem.h>
#include <glut.h>
//These lines above link the code to the openGL libraries.
#pragma comment(lib, "opengl32.lib")	// import gl library
#pragma comment(lib, "glu32.lib")	// import glu library
#pragma comment(lib, "glut32.lib")	// import glut library
///variable intialization to use
#define COL 40
#define ROW 40
int FBS = 10; // frame per second so the snake will move 10 boxes in one second
#define Up 1
#define Down -1
#define Right 2
#define Left -2
#define MAX 60
bool fast = false;
int timer = 0;
int gridX, gridY, snakePosX[60] = { 20,20,20,20,20 }, snakePosY[60] = { 20,19,18,17,16 };
int SnakeDirection = Right, foodX, foodY, Score = 1, snake_length = 5, acceleratorX=-10, acceleratorY=-10,accTime=100;
int goldenMealX = -10, goldenMealY = -10,winScore=30,GMtime=30,timer2=0;
bool gameOver = false, makefood = true, snakesound = true, make_trap = false, createAccelator=false;
bool make_goldenMeal = false,win=false;
double angle = 0, rotatingValue = 0.3;
LPCWSTR game = L"Gongratulations";
LPCWSTR game3 = L"YOU WON";
LPCWSTR game2 = L"Game Over";
/**
Creates the main window, registers event handlers, and
initializes OpenGL stuff.
*/
void InitGraphics(int argc, char** argv);
/*
creation of the window
*/
void OnDisplay();
//TIMER CALL BACK
void timerFun(int);
// claering the buffer defalut color
void init();
// reshape the window when resized or moved
void reshape_fun(int w, int h);
// for initializaiton of the grid
void initGrid(int x, int y);
//Drawing the Grid
void drawGrid();
//drawing the snake
void drawSnake();
// drawing a single unit in the GRID
void unit(int x, int y);
// detect keyboard press button
void keyboardFun(int key, int, int);
void OnkeyPress(unsigned char, int, int);
// drawing food
void drawFood();
// creating random numbers for the position of the food
void random(int& x, int& y);

///Sounds
void eatSound();
void LoseSound();
void snakeSound();
void winSound();
void eatGSound();
void speedupSound();
//accelerate
void drawAccelerator();
void accelerate();
void checkAcceleratorEaten();
///Golden MEAL
void goldenMeal(float,float,float);
void drawGoldenMeal();
void checkGoldenMealEaten();


int main(int argc, char* argv[]) {
	InitGraphics(argc, argv);
	return 0;
}


/**
Creates the main window, registers event handlers, and
initializes OpenGL stuff.
Initialization is done once at the start of the program.
Rendering is done each time the window needs redrawing
*/
//// all the initial values and funcitons related to the window
void InitGraphics(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);// DOUBLE BUFFER IS USED FOR 
	//REDRAWING WHILE A WINDOW IS BEING DISPLAYED THE OTHER ONE IS BEING DISPLAYED

	glutInitWindowPosition(0, 0); //pass (-1, -1) for Window-Manager defaults  glutInitWindowPosition(X, Y);
	glutInitWindowSize(1000, 1000);  //glutInitWindowSize(width, hight);
	glutCreateWindow("Snake Game");
	//OnDisplay will handle the paint event
	glutDisplayFunc(OnDisplay);
	glutReshapeFunc(reshape_fun);//this is used for recreation of the window if it've been moved or resized
	glutTimerFunc(0, timerFun, 0);//needs a call_back which will be trigered after the amount of second specified
	glutSpecialFunc(keyboardFun);
	glutKeyboardFunc(OnkeyPress);
	init();
	glutMainLoop(); //Well glutMainLoop is the main function the keeps calling and calling the displaying functions and which also keeps your window actually opened
}
///OUR WORK
void init() {
	glClearColor(0, 0, 0, 1);
	initGrid(COL, ROW);
}
void reshape_fun(int w, int h) {
	// first we need to set the viewport 
	glViewport(0, 0, GLsizei(w), GLsizei(h));
	//seting up the coordinates of the window
	// we want it like this
	// the left bottom of the window is 0,0 and we want to have 40 unites from left to right and from top to bottom
	/*									.(40,40)
8	.
7	.
6	.
5	.
4	.
3	.
2	.
1	.
0	.  .  .  .  .  .  .  .  .  .  .  .  .  .
	0  1  2  3  4  5  6  7  8  9  10 11 12 13
	*/

	//for projectoin we will use projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();//so it load it as the default no changes happen to it at the begining
	glOrtho(0.0, COL, 0.0, ROW, -1.0, 1.0);//Orhtographic proojection
	//											znear -1 it's out of the screen zfar =  1 goniing in the screen
	glMatrixMode(GL_MODELVIEW);
}

void OnDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);//CLEAR THE COLOR BUFFER EVERY TIME IT DISPLAYS
	if (snakesound) {
		snakeSound();
		std::cout << "snake\n";
		snakesound = false;
	}


	//std::cout << ++timer << '\n';
	drawGrid();
	drawSnake();
	drawFood();
	timer++;
	timer2++;
	if (snake_length - 5 >= winScore)win = true;

	if (timer2 == GMtime) { make_goldenMeal = true; }
	std::cout << timer << "\n";
	if (timer2 == (GMtime * 2)) { goldenMealX = -10; goldenMealY = -10; timer2 = 0; }

	if (timer == accTime) { createAccelator = true; }
	std::cout << timer << "\n";
	if (timer == (accTime * 2)) { acceleratorX = -10; acceleratorY = -10; timer = 0; }
	drawAccelerator();
	drawGoldenMeal();
	glutSwapBuffers();
	/// you will find the window black because of the GL clear color which is black by default
	if (gameOver) {

		LoseSound();
		snake_length -= 5;
		std::string ss = std::to_string(snake_length);
		//std::cout<<ss  ;
		std::string s = "Your score : ";
		s += ss;
		LPWSTR ws = new wchar_t[50];
		copy(s.begin(), s.end(), ws);
		ws[s.size()] = 0; // zero at the end
		MessageBox(NULL, ws, game2, MB_OK | MB_ICONQUESTION);
		exit(0);
	}
	if (win) {
		winSound();
		MessageBox(NULL, game, game3, MB_OK | MB_ICONQUESTION);
		exit(0);

	}
}
/// timer funciton for rebuild the OnDisplay so we can see things moooving
void timerFun(int) {
	angle += rotatingValue*10;
	if (angle > 360)angle -= 360;
	//std::cout <<"angle === "<< angle << '\n';
	glutPostRedisplay();//give openGl the urge to call the display fun as soon as possible
	glutTimerFunc(1000 / FBS, timerFun, 0);///it take argument of milliseconds and 1 s has 1000 ms and we want 10/1 so it will be 1000/10

}

// keyboard pressed keys || taking input from the keyboard using arrows
void keyboardFun(int key, int, int) {
	switch (key) {
	case GLUT_KEY_UP:
		if (SnakeDirection != Down)
			SnakeDirection = Up;
		break;
	case GLUT_KEY_DOWN:
		if (SnakeDirection != Up)
			SnakeDirection = Down;
		break;
	case GLUT_KEY_LEFT:
		if (SnakeDirection != Right)
			SnakeDirection = Left;
		break;
	case	GLUT_KEY_RIGHT:
		if (SnakeDirection != Left)
			SnakeDirection = Right;
		break;
	}

}
///using w,s,a,d
void OnkeyPress(unsigned char key, int x, int y) {
	if (key == 27)exit(0);
	switch (key)
	{
	case 'E':
	case 'e':
		if (fast) { FBS = 10; fast = false; }
		else{
			FBS = 20;
			fast = true;
		}
		
		break;
	case 'W':
	case 'w':
		if (SnakeDirection != Down)
			SnakeDirection = Up;
		break;
	case 'S':
	case 's':
		if (SnakeDirection != Up)
			SnakeDirection = Down;
		break;
	case'A':
	case 'a':
		if (SnakeDirection != Right)
			SnakeDirection = Left;
		break;
	case'D':
	case	'd':
		if (SnakeDirection != Left)
			SnakeDirection = Right;
		break;
	
	}
}

///creating random numbers for food positions
void random(int& x, int& y) {
	int _maxX = gridX - 4;
	int _maxY = gridY - 4;
	int _min = 1;

	srand(time(NULL));
	x = _min + rand() % (_maxX - _min);
	y = _min + rand() % (_maxY - _min);
	std::cout << "x === " << x << '\n';
	std::cout << "y === " << y << '\n';
	for (int j = 0; j < snake_length; j++) {
		if (x == snakePosX[j] && y == snakePosY[j])random(x, y);
	}
}

//drawing the grid 
void initGrid(int x, int y) {
	gridX = x;
	gridY = y;
}

void drawGrid() {
	for (int i = 0; i < gridX; i++)
		for (int j = 0; j < gridY; j++)
			unit(i, j);
}

void unit(int x, int y) {

	if (x == 0 || y == 0 || x == gridX - 1 || y == gridY - 1) {
		glLineWidth(3.0);
		glColor3f(1.0, 0.0, 0.0);
	}
	else {
		//glLineWidth(3.0);
		//glColor3f(1.0, 1.0, 1.0);
		return;
	}

	glBegin(GL_LINE_LOOP);
	glVertex2f(x, y);
	glVertex2f(x + 1, y);
	glVertex2f(x + 1, y + 1);
	glVertex2f(x, y + 1);
	glEnd();
}
//drawing Snake
void drawSnake() {
	//snakeSound();

	glColor3f(0, 1, 1);
	int head1;
	int head2;
	for (int i = snake_length - 1; i > 0; i--) {
		snakePosX[i] = snakePosX[i - 1];
		snakePosY[i] = snakePosY[i - 1];
	}
	if (SnakeDirection == Up)snakePosY[0]++;
	else if (SnakeDirection == Down)snakePosY[0]--;
	else if (SnakeDirection == Left)snakePosX[0]--;
	else if (SnakeDirection == Right)snakePosX[0]++;
	head1 = snakePosX[0];
	head2 = snakePosY[0];
	for (int j = 1; j < snake_length; j++) {
		if (head1 == snakePosX[j] && head2 == snakePosY[j])gameOver = true;
	}

	for (int i = 0; i < snake_length; i++) {
		if (i > 0)glColor3f(0, 1, 0);
		glRectd(snakePosX[i], snakePosY[i], snakePosX[i] + 1, snakePosY[i] + 1);
	}
	if (snakePosX[0] == 0 || snakePosY[0] == 0 || snakePosX[0] == gridX - 1 || snakePosY[0] == gridY - 1) {
		gameOver = true;
	}
	checkGoldenMealEaten();
	if (snakePosX[0] == foodX && snakePosY[0] == foodY) {
		eatSound();
		makefood = true;
		++snake_length;
		snakesound = true;
	}
	checkAcceleratorEaten();
	//checkGoldenMealEaten();
}


//drawing food
void drawFood() {
	if (makefood) {
		random(foodX, foodY);
		//acceleratorX = foodX;
		//acceleratorY = foodY;

	}
	makefood = false;
	glColor3f(1, 1, 0);
	glRectf(foodX, foodY, foodX + 1, foodY + 1);

}
///drawing Accelator accelerator body , accelerator check if eaten
void drawAccelerator() {
	if(createAccelator)
	random(acceleratorX, acceleratorY);
	//acceleratorX = 19;
	//acceleratorY = 6;
	createAccelator = false;
	accelerate();

}
void accelerate() {	
	//glLoadIdentity();
	glTranslatef(acceleratorX, acceleratorY, 0);
	glColor3f(1.0f, 215.0 / 255, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 0);
	glVertex2f(0, 4);
	glVertex2f(1,2);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2f(1,0);
	glVertex2f(1,4);
	glVertex2f(2, 2);
	glEnd();
	glLoadIdentity();
}
void checkAcceleratorEaten() {
	if (snakePosX[0] == acceleratorX && (snakePosY[0] >= acceleratorY && snakePosY[0] <= acceleratorY + 4)) {
		speedupSound();
		FBS++;
		std::cout << "FBS" << FBS << '\n';
		std::cout << "accX" << acceleratorX << '\n';
		std::cout << "accY" << acceleratorY << '\n';
		acceleratorX = -10;
		acceleratorY = -10;
	}
	else if ((snakePosX[0] >= acceleratorX && snakePosX[0] <= acceleratorX + 2) && snakePosY[0] == acceleratorY) {
		speedupSound();
		FBS++;
		//std::cout << "FBS" << FBS << '\n';
		//std::cout << "accX" << acceleratorX << '\n';
		//std::cout << "accY" << acceleratorY << '\n';
		acceleratorX = -10;
		acceleratorY = -10;
	}
	else if ((snakePosX[0] >= acceleratorX && snakePosX[0] <= acceleratorX + 2) && snakePosY[0] == acceleratorY + 4) {
		speedupSound();
		FBS++;
		//std::cout << "FBS" << FBS << '\n';
		//std::cout << "accX" << acceleratorX << '\n';
		//std::cout << "accY" << acceleratorY << '\n';
		acceleratorX = -10;
		acceleratorY = -10;
	}
}


///draw a meal that gives the snake +5 score and length
void drawGoldenMeal() {
	if (make_goldenMeal)
		random(goldenMealX, goldenMealY);
	make_goldenMeal = false;
	std::cout << "GOLDEn x==" << goldenMealX << '\n';
	std::cout << "GOLDEn y==" << goldenMealY << '\n';
	goldenMeal(goldenMealX, goldenMealY, 1);
}
void drawLine(float x,float y, float r ,int size) {
	glLineWidth(size);
	glBegin(GL_LINES);
	glVertex2f(x-r,y);
	glVertex2f(x+r,y);
	glEnd();
	glLineWidth(size);
	glBegin(GL_LINES);
	glVertex2f(x, y-r);
	glVertex2f(x, y+r);
	glEnd();
}
void drawCircle(float x, float y, float r) {
	//glTranslatef(x, y, 0);
	float pi = 3.14;
	GLfloat pi2 = 2 * pi;
	int triangles = 100;
	glBegin(GL_POLYGON);
	//glColor3f(1, 0, 0);
	for (int i = 0; i < triangles; i++) {
		float theta = i * pi2 / triangles;
		glVertex2f(x + (r * cos(theta)), y + (r * sin(theta)));
	}
	glVertex2f(x + (r * cos(0.0)), y + (r * sin(0.0)));
	glEnd();
}
void goldenMeal(float x, float y, float r) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	glRotatef(angle, 0, 0, 1);
	glTranslatef(-3, -3, 0);
	glColor3f(1.0f, 215.0 / 255, 0);
	drawCircle(3, 3, r);
	glColor3f(1.0f, 1.0f, 1.0f);
	drawLine(3, 3, r, 2);
	glPopMatrix();
}
//check if the meal 've been eaten
void checkGoldenMealEaten() {
	if (snakePosX[0] == goldenMealX &&(snakePosY[0] >= goldenMealY-1&&snakePosY[0]<=goldenMealY+1)) {
		std::cout << "UEEEES\n";
		eatGSound();
		goldenMealX = -100;
		goldenMealY = -100;
		snake_length+=5;
		snakesound = true;
	}
	else if ((snakePosX[0] >= goldenMealX - 1 && snakePosX[0] <= goldenMealX + 1) && snakePosY[0] == goldenMealY) {
		eatGSound();
		goldenMealX = -100;
		goldenMealY = -100;
		snake_length += 5;
		snakesound = true;
	}
}




/// <summary>
/// GAME sounds
/// </summary>
void eatSound() {
	PlaySound(TEXT("food.wav"), NULL, SND_ASYNC | SND_FILENAME);
}
void eatGSound() {
	PlaySound(TEXT("eat.wav"), NULL, SND_ASYNC | SND_FILENAME);
}
void LoseSound() {
	PlaySound(TEXT("lose.wav"), NULL, SND_ASYNC | SND_FILENAME);

}
void snakeSound() {
	PlaySound(TEXT("hissing.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

}
void winSound() {
	PlaySound(TEXT("win.wav"), NULL, SND_ASYNC | SND_FILENAME );
}
void speedupSound() {
	PlaySound(TEXT("speed.wav"), NULL, SND_ASYNC | SND_FILENAME);

}

