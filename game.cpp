//---------------------------------------------------------------------------
//Program: Snake group assignment
//Author: Kieran Dennis, Elliot Howard, Daniel Harman
//Last updated: 24 April 2016
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//Extentions implemented:
//
// 2a. Snake Automatic Motion - Daniel Harman	
// 2b. Invincible Snake       - Elliot Howard
// 2c. Game Levels			  - Kieran Dennis
// 2d. Best Scores Record	  - Partially implemented together, finished by Kieran Dennis
// 2e. Save/Load			  - Kieran Dennis 
// 2g. Mongoose Threat        - Elliot Howard 
// 2h. Countdown Timer        - Daniel harman
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>		//for output and input: cout <<, cin >>
#include <iomanip> 		//for output manipulators
#include <conio.h> 		//for getch()
#include <string>		//for string
#include <vector>
#include <time.h>		//Random seeding and for time display
#include <fstream>		//For reading/writing
#include <iterator>
using namespace std;

//include our own libraries
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.


//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//
const unsigned int MAXNAME(20);
const int PILLMOVELIMIT(10);
const int NUMBEROFSCORES(6);
//defining the size of the grid
const int  SIZEY(11);		//vertical dimension
const int  SIZEX(15);    	//horizontal dimension
const int  STARTTAILLEN(3);
const int  SPEEDLIMIT(100); //Fastest speed is 200 milliseconds
const int  MAXLEVEL(3);
//defining symbols used for display of the grid & content
const char SNAKEHEAD('0');   	//snake
const char SNAKETAIL('o');
const char MOUSE('@');
const char POWERPILL('+');
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char MONGOOSE('M');
//Define colours
const WORD SNAKECOLOUR = clRed;
const WORD TAILCOLOUR = clGreen;
const WORD MOUSECOLOUR = clYellow;
const WORD MONGOOSECOLOUR1 = clRed;
const WORD MONGOOSECOLOUR2 = clGrey;
//defining the command letters to move the snake on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
//defining the other command letters
const char QUIT('Q');		//to end the game
const char CHEATCHAR('C');
const char SCORECHAR('B');
const char SLOWCHAR('Z');
const char SAVECHAR('S');
const int NULLCHAR(0);

struct coord {
	int x, y;
};

struct TailPiece {
	int x, y;
	char symbol;
};

struct PillS {
	int x, y;
	char symbol;
	bool isVisible;
	int movesSincePlaced;
	int miceAmountWhenPlaced;
};

struct SnakeS {
	int x, y;
	int dx = 1, dy = 0;
	char symbol;
	vector<TailPiece> tail;
};

struct Mongoose {
	int x, y;
	char symbol;
};

struct HighScores {
	vector<string> names;
	vector<int> scores;
	const int numOfScores = 6;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	string getPlayerName();
	void initialiseGame(char grid[][SIZEX + 1], char maze[][SIZEX + 1], SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int miceCollected, const int level, int& speed, int& winScore, int& levelTime, Mongoose& mongoose, bool mongooseSpawn, bool& mongooseTouched);
	bool saveGame(const char grid[][SIZEX + 1], const SnakeS& snake, const TailPiece& mouse, const PillS& powerPill, const Mongoose& mongoose, const bool mongooseSpawn, const int miceCollected, const int score, const int secondsLeft, const int speed, const int level, string playerName);
	bool loadGame(char grid[][SIZEX + 1], SnakeS& snake, TailPiece& mouse, PillS& powerPill, Mongoose& mongoose, bool& mongooseSpawn, int& miceCollected, int& score, int& secondsLeft, int& speed, int& level, string playerName);
	void paintGame(const char gd[][SIZEX + 1], string& msg, const int score, const int miceCollected, const int bestScore, const int winScore, const int level, const string playerName, const bool wallTouched, const bool tailTouched, const bool gameover, const bool cheating, const bool slow, const bool invincible, const int invincibleTimeLeft, const bool mongooseTouched, const int secondsLeft, bool& mongooseColourChange);
	int getKeyPress();
	void moveSnake(char g[][SIZEX + 1], SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int key, string& msg, int& score, int& miceCollected, bool& wallTouched, bool& tailTouched, const bool cheating, bool& invincible, int& invincibleTimeLeft, bool& mongooseSpawn, bool& mongooseTouched);
	void updateGrid(char grid[][SIZEX + 1], const char maze[][SIZEX + 1], const SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int miceCollected, Mongoose& mongoose, bool& mongooseSpawn, bool& mongooseTouched);
	void writeBestScore(const int score, const int level, const string playerName);
	void findGameMode(const int key, bool& gameover, bool& cheating, bool& slowMode, string& message, SnakeS& snake, int& score, HighScores& scores, int& secondsLeft);
	void getBestScore(const string playerName, int& bestScore, int& level);
	void endProgram();
	void defaultHighScores(HighScores& scores);
	void loadScores(HighScores& scores);
	void saveScores(HighScores& scores, string playerName, int currentScore);
	void waitForPlayer();
	void getStartLevel(int& level);
	bool wantsToLoad(const string playerName);
	void setInitialMazeStructure(char maze[][SIZEX + 1], const int level, int& speed, int& winScore, int& levelTime);
	void keyDelayLoop(int& key, const bool slowMode, const int currentSpeed, const int startSpeed);

	//local variable declarations 
	char grid[SIZEY + 1][SIZEX + 1];	//grid for display
	char maze[SIZEY + 1][SIZEX + 1];	//structure of the maze
	SnakeS snake; 		//snake's position and symbol
	TailPiece mouse;
	PillS powerPill;
	HighScores scores;
	Mongoose mongoose;
	time_t initialTime(time(NULL));

	string playerName;

	string message("LET'S START...");	//current message to player

	int key(NULLCHAR);					//Set initial keypress
	int score(0);						//Set initial score 
	int miceCollected(0);
	int winScore(10);					//Can be changed with different mazes
	int startSpeed;						//speed in milliseconds
	int currentSpeed;
	int moves(0);
	int bestScore;
	int level(1);
	int invincibleTimeLeft(20);
	int levelTime(20);					//Twenty seconds default time
	int secondsLeft(0);

	bool wallTouched(false);
	bool mongooseTouched(false);
	bool tailTouched(false);
	bool gameover(false);
	bool cheating(false);
	bool invincible(false);
	bool slowMode(false);
	bool mongooseSpawn(false);
	bool timeout(false);
	bool mongooseColourChange(false);

	//action...
	srand(time(NULL)); //Seed random numbers (Only needed once)

	Clrscr();

	loadScores(scores); //Load all scores from file

	playerName = getPlayerName(); //Screen to get player's name
	getBestScore(playerName, bestScore, level); //Read in from file, players best score

	if (wantsToLoad(playerName))
	{
		loadGame(grid, snake, mouse, powerPill, mongoose, mongooseSpawn, miceCollected, score, secondsLeft, currentSpeed, level, playerName); // load game from text file
		paintGame(grid, message, score, miceCollected, bestScore, winScore, level, playerName, wallTouched, tailTouched, gameover, cheating, slowMode, invincible, invincibleTimeLeft, mongooseTouched, secondsLeft, mongooseColourChange);	//display game info, modified grid & message
		setInitialMazeStructure(maze, level, startSpeed, winScore, levelTime); //set the initial maze structure
	}
	else
	{
		getStartLevel(level); //Give player option to start at a different unlocked level

		initialiseGame(grid, maze, snake, mouse, powerPill, miceCollected, level, startSpeed, winScore, levelTime, mongoose, mongooseSpawn, mongooseTouched);	//initialise grid (incl. walls & snake)
		secondsLeft = levelTime; //Set beginning seconds
		currentSpeed = startSpeed; //Set the inital speed of the snake
		paintGame(grid, message, score, miceCollected, bestScore, winScore, level, playerName, wallTouched, tailTouched, gameover, cheating, slowMode, invincible, invincibleTimeLeft, mongooseTouched, secondsLeft, mongooseColourChange);			//display game info, modified grid & messages
	}

	waitForPlayer(); //Wait for enter keypress
	initialTime = time(NULL); //Set the time that the game began

	while (!wallTouched && !tailTouched && !gameover && !mongooseTouched && !timeout)			//while user does not want to quit
	{

		keyDelayLoop(key, slowMode, currentSpeed, startSpeed); //Loop for set time and get keypress

		moveSnake(grid, snake, mouse, powerPill, key, message, score, miceCollected, wallTouched, tailTouched, cheating, invincible, invincibleTimeLeft, mongooseSpawn, mongooseTouched);	//move snake in that direction
		updateGrid(grid, maze, snake, mouse, powerPill, miceCollected, mongoose, mongooseSpawn, mongooseTouched);			//update grid information

		findGameMode(key, gameover, cheating, slowMode, message, snake, score, scores, secondsLeft); //Check if we are going to be entering one of the special modes

		//Logic for end game state
		if (miceCollected >= winScore) {
			if (level < MAXLEVEL)
			{
				miceCollected = 0; //Reset the number of mice collected 

				initialiseGame(grid, maze, snake, mouse, powerPill, miceCollected, ++level, startSpeed, winScore, levelTime, mongoose, mongooseSpawn, mongooseTouched);	//reinitialize game for next level
				updateGrid(grid, maze, snake, mouse, powerPill, miceCollected, mongoose, mongooseSpawn, mongooseTouched);			//update grid information
				secondsLeft = levelTime; //Set beginning seconds
				paintGame(grid, message, score, miceCollected, bestScore, winScore, level, playerName, wallTouched, tailTouched, gameover, cheating, slowMode, invincible, invincibleTimeLeft, mongooseTouched, secondsLeft, mongooseColourChange);			//display game info, modified grid & messages

				currentSpeed = startSpeed; //Set the inital speed of the snake loaded from file 
				message = "LEVEL COMPLETED";

				waitForPlayer(); //Wait for enter keypress

				initialTime = time(NULL); //Set the time that the game began

			}
			else
			{
				gameover = true;
				message = "GAME OVER. YOU WIN - WELL DONE!";
			}

		}

		else if (wallTouched || tailTouched || mongooseTouched || secondsLeft < 0) {
			//Set the gameover message
			message = "GAME OVER. YOU LOSE!";
			if (secondsLeft < 0) {
				timeout = true;
			}
		}

		if (key == SAVECHAR)
		{
			if (saveGame(grid, snake, mouse, powerPill, mongoose, mongooseSpawn, miceCollected, score, secondsLeft, currentSpeed, level, playerName)) //Write game to text file
			{
				message = "GAME SAVED";
			}
			else
			{
				message = "GAME COULD NOT BE SAVED";
			}
		}

		if (invincible)
		{
			invincibleTimeLeft--;
			if (invincibleTimeLeft == 0)
			{
				invincible = false;
			}
		}

		if (powerPill.isVisible) {
			++powerPill.movesSincePlaced; //Increments the moves counter for the pill so that the pill can disappear after a set number of moves
		}

		//Increase the speed of the game by 50 milliseconds every 10 moves up to the speed limit
		if (moves % 10 == 0 && currentSpeed > SPEEDLIMIT) {
			currentSpeed -= 50; //Increase speed as time goes on by reducing the delay
		}

		paintGame(grid, message, score, miceCollected, bestScore, winScore, level, playerName, wallTouched, tailTouched, gameover, cheating, slowMode, invincible, invincibleTimeLeft, mongooseTouched, secondsLeft, mongooseColourChange);		//display game info, modified grid & messages

		secondsLeft = levelTime - difftime(time(0), initialTime); //Get difference in time

		key = NULLCHAR; // Reset the input key

		moves++;
	}

	//Write the player's high score
	if (!timeout) { //Don't write scores if player ran out of time
		writeBestScore(score, level, playerName);
		saveScores(scores, playerName, score); //Save the leaderboard 
	}
	endProgram();						//display final message

	return 0;

}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX + 1], char maze[][SIZEX + 1], SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int miceCollected, const int level, int& speed, int& winScore, int& levelTime, Mongoose& mongoose, bool mongooseSpawn, bool& mongooseTouched)
{ //initialise grid & place snake in middle

	void setInitialMazeStructure(char g[][SIZEX + 1], const int level, int& speed, int& winScore, int& levelTime);
	void setInitialSnakeCoordinates(SnakeS& snake);
	void setInitialSnakeTail(SnakeS& snake);
	void updateGrid(char grid[][SIZEX + 1], const char maze[][SIZEX + 1], const SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int miceCollected, Mongoose& mongoose, bool& mongooseSpawn, bool& mongooseTouched);
	void initSpawns(SnakeS& snake, TailPiece& mouse, PillS& powerPill, char g[][SIZEX + 1], const int miceCollected, Mongoose& mongoose);

	setInitialMazeStructure(maze, level, speed, winScore, levelTime);				//initialise maze
	initSpawns(snake, mouse, powerPill, maze, miceCollected, mongoose);				//Set initial postions of snake and mouse
	setInitialSnakeTail(snake);														//Create the snake's tail
	updateGrid(grid, maze, snake, mouse, powerPill, miceCollected, mongoose, mongooseSpawn, mongooseTouched);		//prepare grid
}

bool saveGame(const char grid[][SIZEX + 1], const SnakeS& snake, const TailPiece& mouse, const PillS& powerPill, const Mongoose& mongoose, const bool mongooseSpawn, const int miceCollected, const int score, const int secondsLeft, const int speed, const int level, string playerName)
{
	bool gameSaved = true;
	int tailSize = snake.tail.size();
	ofstream fout(playerName + ".save", ios::out);
	if (fout.fail())
	{
		gameSaved = false;
	}
	else
	{

		//Grid is saved first

		for (int i = 0; i < SIZEX; i++) //top row of fill characters for reading
		{
			fout << "X";
		}
		fout << endl;
		for (int row(1); row <= SIZEY; ++row)
		{	//for each row (vertically)
			fout << "X"; // adds a fill char to the left of the grid
			for (int col(1); col <= SIZEX; ++col)
			{	//for each column (horizontally)
				fout << grid[row][col];
			}
			fout << endl;
		}

		//snake is saved next
		fout << snake.x << endl;						//x coord	(int)
		fout << snake.y << endl;						//y coord	(int)
		fout << snake.dx << endl;						//dx		(int)
		fout << snake.dy << endl;						//dy		(int)
		fout << snake.symbol << endl;					//symbol	(char)
		fout << tailSize << endl;						//tail size	(int)
		for (int i = 0; i < tailSize; i++)	//for each tail segment
		{
			fout << snake.tail[i].x << endl;			//x coord	(int)
			fout << snake.tail[i].y << endl;			//y coord	(int)
			fout << snake.tail[i].symbol << endl;		//symbol	(char)
		}
		//mouse is saved next
		fout << mouse.x << endl;						//x coord	(int)
		fout << mouse.y << endl;						//y coord	(int)
		fout << mouse.symbol << endl;					//symbol	(char)
		//powerPill is saved next
		fout << powerPill.x << endl;					//x coord					(int)
		fout << powerPill.y << endl;					//y coord					(int)
		fout << powerPill.symbol << endl;				//symbol					(char)
		fout << powerPill.movesSincePlaced << endl;		//moves since last placed	(int)
		fout << powerPill.miceAmountWhenPlaced << endl;	//mice when placed			(int)
		fout << powerPill.isVisible << endl;			//is visible				(bool)
		//mongoose saved next
		fout << mongoose.x << endl;						//x coord		(int)
		fout << mongoose.y << endl;						//y coord		(int)
		fout << mongoose.symbol << endl;				//symbol		(char)
		fout << mongooseSpawn << endl;					//mongoosespawn (bool)
		//Players Scores at the time of the save recorded
		fout << miceCollected << endl;					//miceCollected (int)
		fout << score << endl;							//score			(int)
		fout << secondsLeft << endl;					//secondsLeft	(int)
		fout << speed << endl;							//current speed (int)
		fout << level;									//current level (int)
	}
	fout.close();
	return gameSaved;
}

bool loadGame(char grid[][SIZEX + 1], SnakeS& snake, TailPiece& mouse, PillS& powerPill, Mongoose& mongoose, bool& mongooseSpawn, int& miceCollected, int& score, int& secondsLeft, int& speed, int& level, string playerName)
{
	bool gameLoaded = true;
	string tempStore;
	int tailSize;
	int tempX;
	int tempY;
	char tempChar;
	ifstream fin(playerName + ".save", ios::in);

	if (fin.fail())
	{
		gameLoaded = false;
	}
	else
	{

		//Grid is loaded first

		string temp;					//temp store for each line in the file
		vector<string> tempGrid;		//temp store for grid, line by line
		for (int i = 0; i <= SIZEY; i++)
		{
			getline(fin, temp);				//read line of grid
			tempGrid.push_back(temp);		//store in temp grid
		}
		for (int row(1); row <= SIZEY; ++row) {	//for each row (vertically)
			for (int col(1); col <= SIZEX; ++col) {	//for each column (horizontally)
				grid[row][col] = tempGrid[row].c_str()[col]; //copy temp grid into grid
			}
		}

		//snake is loaded next
		getline(fin, tempStore);
		snake.x = atoi(tempStore.c_str());			//x coord	(int)
		getline(fin, tempStore);
		snake.y = atoi(tempStore.c_str());			//y coord	(int)
		getline(fin, tempStore);
		snake.dx = atoi(tempStore.c_str());			//dx		(int)
		getline(fin, tempStore);
		snake.dy = atoi(tempStore.c_str());			//dy		(int)
		getline(fin, tempStore);
		snake.symbol = tempStore.c_str()[0];		//symbol	(char)
		getline(fin, tempStore);
		tailSize = atoi(tempStore.c_str());			//tail size	(int)
		for (tailSize; tailSize > 0; tailSize--)	//for each tail segment
		{
			getline(fin, tempStore);
			tempX = atoi(tempStore.c_str());
			getline(fin, tempStore);
			tempY = atoi(tempStore.c_str());
			getline(fin, tempStore);
			tempChar = tempStore.c_str()[0];
			snake.tail.push_back({ tempX, tempY, tempChar });
		}
		//mouse is loaded next
		getline(fin, tempStore);
		mouse.x = atoi(tempStore.c_str());			//x coord	(int)
		getline(fin, tempStore);
		mouse.y = atoi(tempStore.c_str());			//y coord	(int)
		getline(fin, tempStore);
		mouse.symbol = tempStore.c_str()[0];		//symbol	(char)
		//powerPill is saved next
		getline(fin, tempStore);
		powerPill.x = atoi(tempStore.c_str());						//x coord					(int)
		getline(fin, tempStore);
		powerPill.y = atoi(tempStore.c_str());						//y coord					(int)
		getline(fin, tempStore);
		powerPill.symbol = tempStore.c_str()[0];					//symbol					(char)
		getline(fin, tempStore);
		powerPill.movesSincePlaced = atoi(tempStore.c_str());		//moves since last placed	(int)
		getline(fin, tempStore);
		powerPill.miceAmountWhenPlaced = atoi(tempStore.c_str());	//mice when placed			(int)
		getline(fin, tempStore);
		powerPill.isVisible = atoi(tempStore.c_str());				//is visible				(bool)
		//mongoose saved next
		getline(fin, tempStore);
		mongoose.x = atoi(tempStore.c_str());		//x coord		(int)
		getline(fin, tempStore);
		mongoose.y = atoi(tempStore.c_str());		//y coord		(int)
		getline(fin, tempStore);
		mongoose.symbol = tempStore.c_str()[0];		//symbol		(char)
		getline(fin, tempStore);
		mongooseSpawn = atoi(tempStore.c_str());	//mongoosespawn (bool)
		//Players Scores at the time of the save recorded
		getline(fin, tempStore);
		miceCollected = atoi(tempStore.c_str());	//miceCollected (int)
		getline(fin, tempStore);
		score = atoi(tempStore.c_str());			//score			(int)
		getline(fin, tempStore);
		secondsLeft = atoi(tempStore.c_str());		//secondsLeft	(int)
		getline(fin, tempStore);
		speed = atoi(tempStore.c_str());			//current speed (int)
		getline(fin, tempStore);
		level = atoi(tempStore.c_str());			//current level (int)
	}
	fin.close();
	return gameLoaded;
}

void initSpawns(SnakeS& snake, TailPiece& mouse, PillS& powerPill, char g[][SIZEX + 1], const int miceCollected, Mongoose& mongoose) {
	//Collab
	//Sets the properties for mouse and snake
	void setInitialSnakeCoordinates(SnakeS& snake);
	void setInitialMongooseCoordinates(Mongoose& mongoose, const coord& c);
	coord getValidRandomCoord(const int xlimit, const int ylimit, const char g[][SIZEX + 1], const SnakeS& snake, const TailPiece& mouse, const PillS& pill);
	void spawnPill(PillS& pill, const coord& c, const int miceCollected);
	void spawnMouse(TailPiece& mouse, const coord& c);

	coord mouseCoord, pillCoord, mongooseCoord;

	//gets a random coordinate for the orignal placement of the mongoose
	mongooseCoord = getValidRandomCoord(SIZEX + 1, SIZEY + 1, g, snake, mouse, powerPill);
	setInitialMongooseCoordinates(mongoose, mongooseCoord);
	setInitialSnakeCoordinates(snake);
	snake.symbol = SNAKEHEAD;


	do {
		mouseCoord = getValidRandomCoord(SIZEX + 1, SIZEY + 1, g, snake, mouse, powerPill);
		pillCoord = getValidRandomCoord(SIZEX + 1, SIZEY + 1, g, snake, mouse, powerPill);

		spawnMouse(mouse, mouseCoord);
		spawnPill(powerPill, pillCoord, miceCollected);

	} while (mouse.x == powerPill.x && mouse.y == powerPill.y); //Condition here is to fix the bug where the pill could spawn on the mouse

}

void setInitialSnakeTail(SnakeS& snake)
{
	//Adds the inital tail to snake struct along with postions
	snake.tail._Pop_back_n(snake.tail.size());

	for (int i(0); i < STARTTAILLEN; i++) {
		snake.tail.push_back({ snake.x, snake.y, SNAKETAIL });
	}

}

void setInitialMazeStructure(char maze[][SIZEX + 1], const int level, int& speed, int& winScore, int& levelTime)
{
	//Set up maze from level file
	char levelNum = level + 48;
	string file = (string("level") + levelNum + ".maz"); //open level file
	ifstream fin(file, ios::in);

	if (fin.fail())
	{
		//Default empty maze when levels fail to load
		winScore = 10;
		speed = 1000;
		levelTime = 120;

		char initialMaze[SIZEY + 1][SIZEX + 1] 	//local array to store the maze structure
			= {
				{ 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X' },
				{ 'X', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
				{ 'X', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },

		};
		// with '#' for wall, ' ' for tunnel and 'X' for unused part of array
		//copy into maze structure
		for (int row(1); row <= SIZEY; ++row)
		{	//for each row (vertically)
			for (int col(1); col <= SIZEX; ++col)
			{	//for each column (horizontally)
				maze[row][col] = initialMaze[row][col];
			}
		}
	}
	else
	{
		string temp;					//temp store for each line in the file
		vector<string> tempGrid;		//temp store for grid, line by line
		getline(fin, temp);				//first line is winscore
		winScore = atoi(temp.c_str());	//convert string to int
		getline(fin, temp);				//second line is speed
		speed = atoi(temp.c_str());		//convert string to int
		getline(fin, temp);				//third line is level time
		levelTime = atoi(temp.c_str());	//convert string to int
		for (int i = 0; i <= SIZEY + 1; i++)	//grid starts at the fourth line
		{
			getline(fin, temp);				//read line of grid
			tempGrid.push_back(temp);		//store in temp grid
		}
		for (int row(1); row <= SIZEY; ++row) {	//for each row (vertically)
			for (int col(1); col <= SIZEX; ++col) {	//for each column (horizontally)
				maze[row][col] = tempGrid[row].c_str()[col]; //copy temp grid into maze
			}
		}
	}
}

void setInitialSnakeCoordinates(SnakeS& snake)
{ //calculate snake's coordinates at beginning of game
	snake.y = 3;   	//y-coordinate: vertically
	snake.x = 5; 	//x-coordinate: horizontally
}

void setInitialMongooseCoordinates(Mongoose& mongoose, const coord& c)
{
	//sets the original coordinates for the mongoose, only called once
	mongoose.x = c.x;
	mongoose.y = c.y;
	mongoose.symbol = MONGOOSE;
}

//---------------------------------------------------------------------------
//----- update game state
//---------------------------------------------------------------------------

void spawnMouse(TailPiece& mouse, const coord& c) {
	mouse.x = c.x;
	mouse.y = c.y;
	mouse.symbol = MOUSE;
}

void spawnPill(PillS& pill, const coord& c, const int miceCollected) {
	pill.x = c.x;
	pill.y = c.y;
	pill.symbol = POWERPILL;
	pill.isVisible = false;
	pill.miceAmountWhenPlaced = miceCollected;
}

coord getValidRandomCoord(const int xlimit, const int ylimit, const char g[][SIZEX + 1], const SnakeS& snake, const TailPiece& mouse, const PillS& pill) {
	//Collab
	//Function to get random coordinates that only land on free space
	bool checkCollision(const int x, const int y, const SnakeS& snake, const TailPiece& mouse, const PillS& pill);

	coord retVals;

	do {

		retVals.x = rand() % (xlimit + 2);
		retVals.y = rand() % (ylimit + 2);

	} while (g[retVals.y][retVals.x] != TUNNEL || checkCollision(retVals.x, retVals.y, snake, mouse, pill)); //Loop if coords have landed on a tunnel space or any play pieces

	return retVals;

}

bool checkCollision(const int x, const int y, const SnakeS& snake, const TailPiece& mouse, const PillS& pill) {

	//Function to check if coords conflicts with game pieces

	bool retVal = false;

	//Check if the coordinate will land on the snake
	if ((x == snake.x && y == snake.y) || (x == mouse.x && y == mouse.y) || (x == pill.x && y == pill.y)) {
		retVal = true;
	}

	//Check if the coordinate will land on any of the snake's tail 
	for each (TailPiece tailSegment in snake.tail)
	{
		if (x == tailSegment.x && y == tailSegment.y) {
			retVal = true;
		}
	}

	return retVal;
}

void setCheating(string& message, bool& cheating, SnakeS& snake, int& score) {
	//Logic for toggling cheat mode
	void cheat(SnakeS& snake, int& score);;

	if (!cheating) {
		cheat(snake, score);
		message = "CHEAT MODE ON";
	}
	else {
		message = "CHEAT MODE OFF";
	}
	cheating = !cheating;
	Sleep(700); //Give a short delay
}

coord moveMongoose(char grid[][SIZEX + 1], Mongoose& mongoose, const SnakeS& snake, const TailPiece& mouse, const PillS& pill, bool& mongooseTouched)
{
	//gets a random movement for the mongoose which will be one space in any direction from the placement of the mongoose
	bool checkCollision(const int x, const int y, const SnakeS& snake, const TailPiece& mouse, const PillS& pill, int from);

	coord retVals;
	int randomIndex;
	//possible moves for both x and y
	int possibleMoves[3] = { -1, 0, 1 };

	do {
		//gets random number and uses it as an index for the list, then adds that number onto the x and y value
		randomIndex = (rand() % 3);
		retVals.x = mongoose.x + possibleMoves[randomIndex];
		randomIndex = (rand() % 3);
		retVals.y = mongoose.y + possibleMoves[randomIndex];

	} while (grid[retVals.y][retVals.x] != TUNNEL && grid[retVals.y][retVals.x] != SNAKEHEAD);

	//if the mongoose moves onto the space of the snakehead, it will terminate the game

	if (grid[retVals.y][retVals.x] == SNAKEHEAD)
	{
		mongooseTouched = true;
	}

	return retVals;
}

void keyDelayLoop(int& key, const bool slowMode, const int currentSpeed, const int startSpeed) {
	//Timing loop for getting key state and for delay
	void getKeyInput(int& keyPressed);

	for (int i = 0; i < 2; i++) {
		if (key == NULLCHAR) {
			getKeyInput(key);
		}
		//Check if slowMode is enabled
		if (slowMode) {
			Sleep(startSpeed);
		}
		else {
			Sleep(currentSpeed);
		}
	}
}

void waitForPlayer() {

	//Wait and ask the player to press any key to begin the game
	Gotoxy(40, 20);
	cout << "Press any key to begin";

	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
}

void findGameMode(const int key, bool& gameover, bool& cheating, bool& slowMode, string& message, SnakeS& snake, int& score, HighScores& scores, int& secondsLeft) {

	bool wantsToSlow(const int k);
	void setCheating(string& message, bool& cheating, SnakeS& snake, int& score);
	bool wantsToQuit(const int k);
	bool wantsToCheat(const int k);
	bool wantsToViewScores(const int k);
	void displayScores(const HighScores& scores);

	if (wantsToQuit(key))
	{
		//For when the player is quitting
		gameover = true;
		message = "PLAYER WANTS TO QUIT!";
	}

	else if (wantsToCheat(key))
	{
		//If the player has pressed the 
		setCheating(message, cheating, snake, score);
	}

	else if (wantsToViewScores(key)) {

		time_t initialTime = time(NULL); //Get the time when the screen is loaded
		displayScores(scores); //Display them
		secondsLeft += difftime(time(0), initialTime); //Get the time spent in the score screen and add it back on
	}

	else if (wantsToSlow(key)) {
		//Toggle slowmode when the SLOWCHAR key is pressed
		slowMode = !slowMode;

		if (slowMode) {
			message = "SLOW MODE ACTIVATED!"; //Inform the player
		}
	}
}

bool wantsToLoad(const string playerName)
{
	bool load = true;
	char choice;
	ifstream fin(playerName + ".save", ios::in);

	if (fin.fail())
	{
		load = false;   //Skip file load if opening fails
	}
	else
	{
		do
		{
			//ask if player wants to load until a valid input is given
			Clrscr();
			SelectTextColour(clYellow);
			Gotoxy(20, 10);
			cout << "LOAD PREVIOUS SAVEGAME? (Y/N): ";
			SelectTextColour(clWhite);
			cin >> choice;
			toupper(choice);

		} while (choice != 'Y' && choice != 'N');
		if (choice == 'N')
		{
			load = false;  //Player chooses not to load
		}
	}
	return load;
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX + 1], const char maze[][SIZEX + 1], const SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int miceCollected, Mongoose& mongoose, bool& mongooseSpawn, bool& mongooseTouched)
{ //update grid configuration after each move
	void setMaze(char g[][SIZEX + 1], const char m[][SIZEX + 1]);
	coord moveMongoose(char g[][SIZEX + 1], Mongoose& mongoose, const SnakeS& snake, const TailPiece& mouse, const PillS& pill, bool& mongooseTouched);
	void placeSnake(char g[][SIZEX + 1], const SnakeS& snake);
	void placeMouse(char g[][SIZEX + 1], const TailPiece& mouse);
	void placePill(char g[][SIZEX + 1], PillS& powerPill, const int miceCollected, const SnakeS& snake, const TailPiece& mouse);
	void placeMongoose(char g[][SIZEX + 1], Mongoose& mongoose, const coord& c, const SnakeS& snake, const TailPiece& mouse, const PillS& pill);

	coord mongooseCoord;

	setMaze(grid, maze);									//reset the empty maze configuration into grid
	placeSnake(grid, snake);								//set snake in grid
	placeMouse(grid, mouse);								//Set Mouse in grid
	placePill(grid, powerPill, miceCollected, snake, mouse);				//Set pill in grid

	//takes away the mongoose when there is a new level
	if (miceCollected < 3)
	{
		mongooseSpawn = false;
	}

	if (mongooseSpawn)
	{
		mongooseCoord = moveMongoose(grid, mongoose, snake, mouse, powerPill, mongooseTouched);
		placeMongoose(grid, mongoose, mongooseCoord, snake, mouse, powerPill);
	}
}

void setMaze(char grid[][SIZEX + 1], const char maze[][SIZEX + 1])
{ //reset the empty/fixed maze configuration into grid

	for (int row(1); row <= SIZEY; ++row) {					//for each row (vertically)
		for (int col(1); col <= SIZEX; ++col) {				//for each column (horizontally)
			grid[row][col] = maze[row][col];
		}
	}
}

void placeMouse(char g[][SIZEX + 1], const TailPiece& mouse)
{
	//Place mouse at new position in grid
	g[mouse.y][mouse.x] = mouse.symbol;
}

void placeSnake(char g[][SIZEX + 1], const SnakeS& snake)
{ //place snake at its new position in grid
	void placeTail(char g[][SIZEX + 1], const SnakeS& snake);

	placeTail(g, snake); //Put tail down before head,so that head is always drawn on top.
	g[snake.y][snake.x] = snake.symbol;

}

void placeTail(char g[][SIZEX + 1], const SnakeS& snake)
{
	//Place the snake's tail on the board
	for (unsigned int i(0); i < snake.tail.size(); i++) {
		g[snake.tail[i].y][snake.tail[i].x] = snake.tail[i].symbol;
	}
}

void placePill(char g[][SIZEX + 1], PillS& powerPill, const int miceCollected, const SnakeS& snake, const TailPiece& mouse)
{

	void spawnPill(PillS& pill, const coord& c, const int miceCollected);
	//Logic for pill placement and reveal

	/*This if statement is executed when the number of mice collected is 2, the pill is not already visible
	 *and that the number of mice when the pill was last placed isn't same to prevent the pill reappearing again after the move-limit is reached
	 *Places the pill, resets the number of moves made since placed and makes it visible
	 */
	if (powerPill.miceAmountWhenPlaced != miceCollected && miceCollected % 2 == 0 && !powerPill.isVisible) {

		g[powerPill.y][powerPill.x] = powerPill.symbol;
		powerPill.movesSincePlaced = 0;
		powerPill.miceAmountWhenPlaced = miceCollected;
		powerPill.isVisible = true;

	}

	//Excuted while the pill is displayed and the move limit is reached, removes the pill from the board.
	else if (powerPill.isVisible  && powerPill.movesSincePlaced >= PILLMOVELIMIT) {

		g[powerPill.y][powerPill.x] = TUNNEL;
		powerPill.movesSincePlaced = 0;
		spawnPill(powerPill, getValidRandomCoord(SIZEX, SIZEY, g, snake, mouse, powerPill), miceCollected); //Randomise pill position

	}
	//Otherwise just place the pill
	else if (powerPill.isVisible) {
		g[powerPill.y][powerPill.x] = powerPill.symbol;
	}

}

void placeMongoose(char grid[][SIZEX + 1], Mongoose& mongoose, const coord& mongooseCoord, const SnakeS& snake, const TailPiece& mouse, const PillS& pill)
{
	//function for putting the mongoose on the grid and setting the mongoose coordinates to the coordinates taken from the moveMongoose function
	mongoose.x = mongooseCoord.x;
	mongoose.y = mongooseCoord.y;
	mongoose.symbol = MONGOOSE;
	grid[mongoose.y][mongoose.x] = mongoose.symbol;
}

//---------------------------------------------------------------------------
//----- move the snake
//---------------------------------------------------------------------------

void moveSnake(char g[][SIZEX + 1], SnakeS& snake, TailPiece& mouse, PillS& powerPill, const int key, string& msg, int& score, int& miceCollected, bool& wallTouched, bool& tailTouched, const bool cheating, bool& invincible, int& invincibleTimeLeft, bool& mongooseSpawn, bool& mongooseTouched)
{ //move snake in required direction

	void setSnakeDirection(int k, int& dx, int& dy);
	void drawTail(SnakeS& snake, int intialx, int initaly);
	void increaseTail(SnakeS& snake, const bool cheating, int len = 2);
	void resetTail(SnakeS& snake);
	coord getValidRandomCoord(const int xlimit, const int ylimit, const char g[][SIZEX + 1], const SnakeS& snake, const TailPiece& mouse, const PillS& pill);

	//calculate direction of movement required by key - if any
	setSnakeDirection(key, snake.dx, snake.dy); 	//find direction indicated by key
	int initx(snake.x), inity(snake.y);

	//check new target position in grid and update snake coordinates if move is possible
	switch (g[snake.y + snake.dy][snake.x + snake.dx])
	{			//...depending on what's on the target position in grid...


	case MOUSE:
		//Increment the score counters
		score++;
		miceCollected++;
		if (miceCollected == 3)
		{
			mongooseSpawn = true;
		}
		increaseTail(snake, cheating);		//Make tail longer
		spawnMouse(mouse, getValidRandomCoord(SIZEX, SIZEY, g, snake, mouse, powerPill));		//Move the mouse somewhere else
		snake.y += snake.dy;		//go in that Y direction
		snake.x += snake.dx;		//go in that X direction
		drawTail(snake, initx, inity);	//Redraw tail
		break;

	case POWERPILL:

		resetTail(snake); //Reset the tail's length

		invincible = true;
		invincibleTimeLeft = 21;
		spawnPill(powerPill, getValidRandomCoord(SIZEX, SIZEY, g, snake, mouse, powerPill), miceCollected);
		snake.y += snake.dy;		//go in that Y direction
		snake.x += snake.dx;		//go in that X direction
		drawTail(snake, initx, inity);
		break;

	case TUNNEL:
		//can move
		snake.y += snake.dy;		//go in that Y direction
		snake.x += snake.dx;		//go in that X direction
		drawTail(snake, initx, inity);
		break;

	case SNAKETAIL:
		if (!invincible)
		{
			tailTouched = true;
			break;
		}


	case WALL:  		//Game over if tail or wall is bumped into
		if (!invincible) //if the snake is not in invincible mode, the game will be over, same applies for the tail and the mongoose
		{
			wallTouched = true;
			break;
		}
		else
		{
			//code for making the snake appear on the other side of the grid if it runs into a wall
			if (snake.y > SIZEY - 2 && snake.dy == 1)
			{
				snake.y = 2;
				drawTail(snake, initx, inity);
			}
			else if (snake.y < 3 && snake.dy == -1)
			{
				snake.y = SIZEY - 1;
				drawTail(snake, initx, inity);
			}
			else if (snake.x > SIZEX - 3 && snake.dx == 1)
			{
				snake.x = 2;
				drawTail(snake, initx, inity);
			}
			else if (snake.x < 3 && snake.dx == -1)
			{
				snake.x = SIZEX - 2;
				drawTail(snake, initx, inity);
			}
			break;
		}

	case MONGOOSE:
		if (!invincible)
		{
			mongooseTouched = true;
			break;
		}
	}
}

void drawTail(SnakeS& snake, const int initx, const int inity) {
	//collab
	//As a Group: Tail follows snake head
	//Moves 
	for (unsigned int i(snake.tail.size()); i > 1; --i)
	{
		snake.tail[i - 1].x = snake.tail[i - 2].x;
		snake.tail[i - 1].y = snake.tail[i - 2].y;
	}
	snake.tail[0].x = initx;
	snake.tail[0].y = inity;
}

void increaseTail(SnakeS& snake, const bool cheating, int len = 2) {
	//Function increase tail size. Default by spec is by two
	//Collab

	if (!cheating) //If we are in the cheating state we don't want to increase the length 
	{
		for (int i = 0; i < len; i++)
		{
			snake.tail.push_back({ snake.tail.back().x, snake.tail.back().y, SNAKETAIL }); //Add tail segments to body
		}
	}
}

void resetTail(SnakeS& snake) {
	//Pop tail segments until the default tail length is reached
	while (snake.tail.size() > STARTTAILLEN) {
		snake.tail.pop_back();
	}
}

void cheat(SnakeS& snake, int& score) {
	//Cheat function, resets tail length, score, beep alarm
	void resetTail(SnakeS& snake);

	score = 0;
	resetTail(snake);
	cout << "\a \a \a";

}

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------

void getKeyInput(int& keyPressed) {

	//Reads the current state of all keys and sets the key state

	if (GetAsyncKeyState(VK_LEFT)) {
		keyPressed = LEFT;
	}

	if (GetAsyncKeyState(VK_UP)) {
		keyPressed = UP;
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		keyPressed = RIGHT;
	}

	if (GetAsyncKeyState(VK_DOWN)) {
		keyPressed = DOWN;
	}

	if (GetAsyncKeyState(0x42)) { //For 'B' key
		keyPressed = SCORECHAR;
	}

	if (GetAsyncKeyState(0x43)) { //For 'C' key
		keyPressed = CHEATCHAR;
	}

	if (GetAsyncKeyState(0x51)) { //For 'Q' key
		keyPressed = QUIT;
	}

	if (GetAsyncKeyState(0x5A)) { //For 'Z' key
		keyPressed = SLOWCHAR;
	}

	if (GetAsyncKeyState(0x53)) { //For 'S' key
		keyPressed = SAVECHAR;
	}

}

void setSnakeDirection(const int key, int& dx, int& dy)
{ //
	switch (key)		//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP: 		//when UP arrow pressed...
		dx = 0;
		dy = -1;	//decrease the Y coordinate
		break;
	case DOWN: 		//when DOWN arrow pressed...
		dx = 0;
		dy = +1;	//increase the Y coordinate
		break;
	}
}

bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return toupper(key) == QUIT;
}

bool wantsToCheat(const int k)
{	//Check if user wants to cheat (when key is 'C' or 'c')
	return toupper(k) == CHEATCHAR;
}

bool wantsToViewScores(const int k) {
	return toupper(k) == SCORECHAR;
}

bool wantsToSlow(const int k) {
	return toupper(k) == SLOWCHAR;
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

void displayStartScreen() {

	//Print our lovely title text
	SelectTextColour(clGreen);
	cout << " ____              _        _ _ _ " << endl;
	cout << "/ ___| _ __   __ _| | _____| | | |" << endl;
	cout << "\\___ \\| '_ \\ / _` | |/ / _ \\ | | |" << endl;
	cout << " ___) | | | | (_| |   <  __/_|_|_|" << endl;
	cout << "|____/|_| |_|\\__,_|_|\\_\\___(_|_|_)" << endl;
	cout << "Kieran Dennis, Elliot Howard, Daniel Harman - Computer Science 2016";

}

string getPlayerName() {
	//Asks the player to enter their name and caps it at 20 chars
	void displayStartScreen();

	string name;

	//Continue to loop asking for name when entered text is > 20 chars
	do {
		Clrscr();
		displayStartScreen(); //Display our nice title screen text
		SelectTextColour(clYellow);
		Gotoxy(20, 10);

		//Ask for player's name
		cout << "ENTER PLAYER NAME (max 20 characters): ";
		SelectTextColour(clWhite);
		cin >> name;

	} while (name.length() > MAXNAME); //Loop while it's larger than 20 chars

	return name;
}

void getStartLevel(int& level)
{
	string temp;

	do
	{
		Clrscr();
		Gotoxy(15, 3);
		SelectTextColour(clRed);
		//Draw the header 
		cout << " LEVEL  SELECTION";
		Gotoxy(15, 4);
		cout << "------------------";
		Gotoxy(15, 5);

		for (int i = 1; i <= MAXLEVEL; i++)
		{
			//Display unlocked levels
			if (level >= i)
			{
				cout << "LEVEL " << i << "   UNLOCKED" << endl;
			}
			//Else show them as locked 
			else
			{
				cout << "LEVEL " << i << "     LOCKED" << endl;
			}
			Gotoxy(15, 5 + i);
		}
		cout << "SELECT LEVEL: ";
		cin >> temp;
		//Pressing K will unlock all levels
		if (temp == "k" || temp == "K")
		{
			level = MAXLEVEL;
		}

	} while (atoi(temp.c_str()) > level || atoi(temp.c_str()) < 1);

	level = atoi(temp.c_str());
}

void paintGame(const char gd[][SIZEX + 1], string& msg, const int score, const int miceCollected, const int bestScore, const int winScore, const int level, const string playerName, const bool wallTouched, const bool tailTouched, const bool gameover, const bool cheating, const bool slow, const bool invincible, const int invincibleTimeLeft, const bool mongooseTouched, const int secondsLeft, bool& mongooseColourChange)
{ //display game title, messages, maze, snake & apples on screen
	void paintGrid(const char g[][SIZEX + 1], bool invincible, bool& mongooseColourChange);

	time_t theTime = time(0);   // get time now
	struct tm now;
	localtime_s(&now, &theTime);

	//clear screen
	Clrscr();

	//display game title
	SelectTextColour(clYellow);
	Gotoxy(0, 0);
	cout << "__Snake Game__" << endl;

	SelectBackColour(clWhite);
	SelectTextColour(clRed);

	//Display project title
	Gotoxy(40, 0);
	cout << "FoP Task 1c: 2016";

	//Display team members
	Gotoxy(40, 1);
	cout << "Group R8: Kieran Dennis, Elliot Howard, Daniel Harman";

	//Display time and date
	Gotoxy(40, 2);
	cout << setw(2) << setfill('0') << now.tm_mday
		<< '/' << setw(2) << setfill('0') << (now.tm_mon + 1)
		<< '/' << (now.tm_year + 1900) << " ";
	cout << setw(2) << setfill('0') << now.tm_hour << ":";
	cout << setw(2) << setfill('0') << now.tm_min << ":";
	cout << setw(2) << setfill('0') << now.tm_sec;

	// display grid contents
	paintGrid(gd, invincible, mongooseColourChange);

	//display menu options available
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 3);
	cout << "TO MOVE USE KEYBOARD ARROWS ";

	Gotoxy(40, 4);
	cout << "TO QUIT ENTER 'Q'           ";

	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(40, 5);
	cout << "TIME REMAINING: ";

	SelectBackColour(clGreen);
	SelectTextColour(clBlack);
	cout << secondsLeft;

	SelectBackColour(clBlack);
	SelectTextColour(clWhite);

	Gotoxy(40, 6);
	cout << "PLAYER'S NAME:		: " << playerName;

	//Display the best score
	Gotoxy(40, 7);
	cout << "BEST SCORE SO FAR	: " << bestScore;

	//Display the score
	Gotoxy(40, 8);
	cout << "CURRENT SCORE		: " << score;
	//Display current game level
	Gotoxy(40, 9);
	cout << "CURRENT LEVEL		: " << level;
	//display number of mice collected
	Gotoxy(40, 11);
	cout << "NUMBER OF MICE COLLECTED: " << miceCollected << " OF " << winScore;

	Gotoxy(40, 12);
	if (!cheating) {

		cout << "PRESS 'C' TO CHEAT";
	}
	else {
		cout << "CHEAT IS ON - PRESS 'C' TO TAKE IT OFF";
	}
	Gotoxy(40, 13);
	if (!slow) {
		cout << "PRESS 'Z' TO ACTIVATE SLOW MODE";
	}
	else {
		cout << "SLOWMODE IS ON - PRESS 'Z' TO TAKE IT OFF";
	}
	Gotoxy(40, 14);
	cout << "PRESS 'S' TO SAVE";
	Gotoxy(40, 15);
	cout << "PRESS 'B' TO VIEW SCOREBOARD";

	Gotoxy(40, 17);
	if (invincible)
	{
		cout << "YOU ARE INVINCIBLE: " << invincibleTimeLeft << " MOVES LEFT!" << endl;
	}

	if (wallTouched) { //Gameover message has a unique colour set
		cout << "YOU BUMPED INTO A WALL" << endl;

	}
	else if (tailTouched) { //Gameover message has a unique colour set
		cout << "YOU BUMPED INTO YOURSELF" << endl;
	}
	else if (mongooseTouched) {
		cout << "YOU GOT EATEN BY THE MONGOOSE" << endl;
	}

	else if (secondsLeft < 0) {
		cout << "YOU RAN OUT OF TIME" << endl;
	}

	if (gameover || wallTouched || tailTouched || mongooseTouched || secondsLeft < 0) {
		SelectBackColour(clGreen);
		SelectTextColour(clBlue);
		cout << '\a';
	}

	Gotoxy(40, 17); //Set final position
	cout << msg;	//display current message
	msg = "";		//reset message to blank
}

void paintGrid(const char g[][SIZEX + 1], bool invincible, bool& mongooseColourChange)
{ //display grid content on screen

	SelectBackColour(clBlack);
	SelectTextColour(clWhite);
	Gotoxy(0, 2);

	for (int row(1); row <= SIZEY; ++row)	//for each row (vertically)
	{
		for (int col(1); col <= SIZEX; ++col) {
			//for each column (horizontally)
			switch (g[row][col]) {

				//Powerpill and mouse share the same colour (yellow)
			case POWERPILL:
			case MOUSE:
				SelectTextColour(MOUSECOLOUR);
				break;

				//Set the snake tail colour when printing out tail pieces, different when invincible
			case SNAKETAIL:
				if (invincible)
				{
					SelectTextColour(clRed);
				}
				else
				{
					SelectTextColour(TAILCOLOUR);
				}
				break;

				//Set the snake head colour, different when invincible
			case SNAKEHEAD:
				if (invincible)
				{
					SelectTextColour(clBlue);
				}
				else
				{
					SelectTextColour(SNAKECOLOUR);
				}
				break;

				//interchanges mongoose colours between red and grey to make it look like its flashing
			case MONGOOSE:
				if (mongooseColourChange)
				{
					SelectTextColour(MONGOOSECOLOUR1);
					mongooseColourChange = false;
				}
				else
				{
					SelectTextColour(MONGOOSECOLOUR2);
					mongooseColourChange = true;
				}
				break;
			}

			cout << g[row][col];	//output cell content
			SelectTextColour(clWhite); //Reset colours
		}
		cout << endl;
	}
}

void defaultHighScores(HighScores& scores) {
	//Fill HighScores with default values
	for (int i = 0; i < NUMBEROFSCORES; i++)
	{
		scores.names.push_back("anonymous");
		scores.scores.push_back(-1);
	}

}

void displayScores(const HighScores& scores) {

	Clrscr(); //Clear the screen

	//Set the position and colour of the title 
	Gotoxy(17, 0);
	SelectTextColour(clYellow);
	cout << "HIGH SCORES";
	Gotoxy(10, 1);
	cout << "-----------------------";

	int y = 3;
	//Loop to print out all the names
	SelectTextColour(clGreen);
	for each (string name in scores.names)
	{
		Gotoxy(10, y++);
		cout << name << ":";
	}

	y = 3;
	//Loop to print out each of the scores
	SelectTextColour(clRed);
	for each (int score in scores.scores)
	{
		Gotoxy(30, y++);
		cout << score << ":";
	}

	Gotoxy(10, 10);
	cout << "Press enter to return";
	while (GetAsyncKeyState(VK_RETURN) == 0); //Keep looping until enter pressed.

}

//---------------------------------------------------------------------------
//----- File operations 
//---------------------------------------------------------------------------

void getBestScore(const string playerName, int& bestScore, int& level) {
	//Reads from the file playerName.txt and gets the score witten to it
	ifstream fin(playerName + ".txt", ios::in);

	//Opening will fail for new player names, just set the default scores
	if (fin.fail()) {
		bestScore = 0;
		level = 1;
	}
	else {

		string temp;
		getline(fin, temp); //Get score from file
		bestScore = atoi(temp.c_str());
		getline(fin, temp);
		level = atoi(temp.c_str());

		//Fixes the bug where a level less than 1 would lock all levels 
		if (level < 1)
		{
			level = 1;
		}
	}
	fin.close();

}

void loadScores(HighScores& scores) {
	ifstream fin("HighScores.txt", ios::in);
	string temp;

	//If the file does not exist just load the default scores 
	if (fin.fail()) {
		defaultHighScores(scores);
	}
	else {
		//Read in all the names
		for (int i = 0; i < NUMBEROFSCORES; i++)
		{
			getline(fin, temp);
			scores.names.push_back(temp);
		}
		//Read all the scores 
		for (int i = 0; i < 6; i++)
		{
			getline(fin, temp);
			scores.scores.push_back(atoi(temp.c_str()));
		}
	}
	fin.close();
}

void writeBestScore(const int score, int level, const string playerName) {
	//Checks that a new highscore is registered and writes to the file if so
	int bestScore;
	int bestLevel;
	getBestScore(playerName, bestScore, bestLevel);

	ofstream fout(playerName + ".txt", ios::out);

	if (bestScore <= score)
	{
		fout << score << endl;
	}
	if (bestLevel <= level)
	{
		fout << level;

	}
	fout.close();
}

void saveScores(HighScores& scores, string playerName, int currentScore)
{
	vector<int>::iterator scoresIt = scores.scores.begin();		//iterator for scores
	vector<string>::iterator namesIt = scores.names.begin();	//iterator for names

	int i = 0;
	bool inserted = false;

	while (i < NUMBEROFSCORES && !inserted)
	{
		if (scores.scores.at(i) <= currentScore)
		{
			scores.scores.insert(scoresIt + i, currentScore);		//inserts the current score if it is higher than the previous score
			scores.names.insert(namesIt + i, playerName);			//inserts the name of the above highscore
			scores.scores.pop_back();							//clears lowest score to keep only 6 highscores
			scores.names.pop_back();							//clears name of the above lowest score
			inserted = true;									//allows loop to exit when highscore is inserted
		}
		i++;
	}


	ofstream fout("HighScores.txt", ios::out);
	//write each highscore name on a new line
	for each (string name in scores.names)
	{
		fout << name << endl;
	}
	//write each score on a new line
	for each (int score in scores.scores)
	{
		fout << score << endl;
	}
}

void endProgram()
{
	SelectBackColour(clRed);
	SelectTextColour(clYellow);
	Gotoxy(40, 16);
	//hold output screen until a keyboard key is hit
	cout << "\n";
	system("pause");
}
