//TREASURE CAVE

#pragma once
#include "GSP115_Course_Project.h"

// Global Constants
const int MAX_ROWS = 7;
const int MIN_ROWS = 0;
const int MAX_COLS = 7;
const int MIN_COLS = 0;
const int TOTAL_ROWS = MAX_ROWS + 1;
const int TOTAL_COLS = MAX_COLS + 1;

// Combat Result Constants
const int PLAYER_DIES_WITH_WEAPON = 80;
const int MONSTER_DIES_WITH_WEAPON = 30;
const int PLAYER_DIES_NO_WEAPON = 20;
const int MONSTER_DIES_NO_WEAPON = 18;


//Display Constants
const char ULC = 201;       	//Upper left corner
const char HB = 205;        	//Horizontal border
const char URC = 187;       	//Upper right corner
const char VB = 186;        	//Vertical border
const char LRC = 188;       	//Lower right corner
const char LLC = 200;       	//Lower left corner
const char MT = ' ';        	//Empty location
const char PSymbol = 'P';   	//Player symbol
const char TOSymbol = 'T';  	//Torch symbol
const char WSymbol = 'W';   	//Weapon symbol
const char TRSymbol = '$';  	//Treasure symbol
const char MSymbol = 'M';   	//Monster symbol
const char MBSymbol = 'MB';   	//Monster 2 boogy man symbol
const char NSymbol = 'N';   	//Noisemaker symbol
const char XSymbol = 'X';   	//Cave exit symbol

using namespace std;

// function prototypes
gameObject placeInCave(gameObjectType array[TOTAL_ROWS][TOTAL_COLS]);
bool checkVisible(gameObject x, gameObject y, int dist);
bool showOnBoard(gameObject x);
// <WK6 status=permanent>
bool resolveCombat(playerObject &p, gameObject &m, int YouDie, int MonsterDies, string &str, bool &moveP, bool &mPause);
void setcolor(int c);
void gamecolor(unsigned char type);
// </WK6>

int main()
{
	//**Initialize Variables**
	srand(time(NULL));          	// Seed the random number function
	gameObjectType cave[TOTAL_ROWS][TOTAL_COLS];             	// the cave--a two dimensional array
	char board[TOTAL_ROWS + 3][TOTAL_COLS + 3] =             	// the game board--a two dimensional array
	{
		{ MT,MT,'0','1','2','3','4','5','6','7',MT },
		{ MT,ULC,HB,HB,HB,HB,HB,HB,HB,HB,URC },
		{ 'A',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'B',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'C',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'D',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'E',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'F',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'G',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ 'H',VB,MT,MT,MT,MT,MT,MT,MT,MT,VB },
		{ MT,LLC,HB,HB,HB,HB,HB,HB,HB,HB,LRC }
	};

	gameObjectType hold = EMPTY;                                        	// Holds objects under the monster<bug fix>
																			
	playerObject player = { true, false, false, false, false,{ -1, -1, false, true } };  // the player
	gameObject treasure = { -1, -1, false, true };        	// the treasure
	gameObject monster = { -1, -1, false, true };         	// the monster
	gameObject monsterB = { -1, -1, false, true };         	// the monster
	gameObject weapon = { -1, -1, false, true };          	// the weapon
	gameObject torch = { -1, -1, false, true };           	// the torch
	gameObject noisemaker = { -1, -1, false, true };  // the noisemaker
													  
	gameObject caveExit = { -1, -1, false, true };        	// the cave exit
															
	int row, column;                                             	// temporarily hold the new player position
	int MonsterMoveCounter = 6;                                         	// track and control monster movement around treasure
	int MonsterBMoveCounter = 6;
																			
	int killedValue = PLAYER_DIES_NO_WEAPON;            	// a random value above this level means you are killed by the monster
	int killMonsterValue = MONSTER_DIES_NO_WEAPON;  // a random value above this level and below killedValue means you kill the monster
	int killMonsterBValue = MONSTER_DIES_NO_WEAPON;

	string msg;                               // status message variable
	char command;                             	// player input
	bool monsterPause = false;  	// flag to indicate the monster has stopped moving
	bool monsterBPause = false;     // flag to indicate the second monster has stopped moving
	bool movePlayer = true;     	// flag to indicate the player position can be updated
	bool gameOver = false;      	// status flag

			//********************Prepare Cave*********************
									//...Initialize an empty cave
	for (gameObjectType(&R)[TOTAL_COLS] : cave)
	{
		for (auto &C : R) C = EMPTY;
	}
	//...Add player in rows 0-2, columns 0-2
	player.position.row = rand() % 3;
	player.position.column = rand() % 3;
	cave[player.position.row][player.position.column] = PLAYER;
	//...Add Treasure in rows 4-6, column 1-6
	treasure.row = rand() % 3 + 4;
	treasure.column = rand() % 6 + 1;
	cave[treasure.row][treasure.column] = TREASURE;
	//...Add Monster at treasure row +1 , column -1
	monster.row = treasure.row + 1;
	monster.column = treasure.column - 1;
	cave[monster.row][monster.column] = MONSTER;
	// set boogy man on grid
	monsterB.row = treasure.row + 2;
	monsterB.column = treasure.column - 3;
	cave[monsterB.row][monsterB.column] = MONSTERB;
	//...Add Weapon in any empty location
	weapon = placeInCave(cave);
	cave[weapon.row][weapon.column] = WEAPON;
	//...Add Noisemaker in any empty location
	noisemaker = placeInCave(cave);
	cave[noisemaker.row][noisemaker.column] = NOISEMAKER;
	//...Add Torch in any empty location
	torch = placeInCave(cave);
	cave[torch.row][torch.column] = TORCH;
	//...Add Cave Exit in any empty location.
	caveExit = placeInCave(cave);
	cave[caveExit.row][caveExit.column] = CAVE_EXIT;

	//**Play Game*************
	//...Begin Game Loop
	while (!gameOver)
	{
		//....Display Board
		//.....Check visibility in game
		if (!player.hasTorch)
		{
			torch.isVisible = checkVisible(player.position, torch, 2);
			weapon.isVisible = checkVisible(player.position, weapon, 2);
			treasure.isVisible = checkVisible(player.position, treasure, 2);
			monster.isVisible = checkVisible(player.position, monster, 2);
			monsterB.isVisible = checkVisible(player.position, monsterB, 2);
			noisemaker.isVisible = checkVisible(player.position, noisemaker, 2);
			caveExit.isVisible = checkVisible(player.position, caveExit, 2);
		}
		//.....Place visible objects on board--note: changing the order below will create a visual bug, so dont
		board[weapon.row + 2][weapon.column + 2] = showOnBoard(weapon) ? WSymbol : MT;
		board[torch.row + 2][torch.column + 2] = showOnBoard(torch) ? TOSymbol : MT;
		board[treasure.row + 2][treasure.column + 2] = showOnBoard(treasure) ? TRSymbol : MT;
		board[noisemaker.row + 2][noisemaker.column + 2] = showOnBoard(noisemaker) ? NSymbol : MT;
		board[caveExit.row + 2][caveExit.column + 2] = showOnBoard(caveExit) ? XSymbol : MT;
		board[monster.row + 2][monster.column + 2] = showOnBoard(monster) ? MSymbol : MT;
		board[monsterB.row + 2][monsterB.column + 2] = showOnBoard(monsterB) ? MBSymbol : MT;
		board[player.position.row + 2][player.position.column + 2] = player.alive ? PSymbol : MT;
		// Put the board on the screen
		for (char(&R)[TOTAL_COLS + 3] : board)
		{
			for (char &C : R)
			{
				gamecolor(C);
				cout << C;
			}
			cout << endl;
		}
		gamecolor('Z');
		cout << msg.c_str() << endl;
		//....Get command
		cout << "What is your command? ";
		cin >> command;
		//....Clear display and message
		msg.clear();
		system("cls");
		//....Process player command
		row = player.position.row;
		column = player.position.column;
		switch (command)
		{
		case 'a':
			column = player.position.column - 1;
			if (column < MIN_COLS)
			{
				column = player.position.column;
				msg = "You have hit the west wall!";
			}
			break;
		case 's':
			row = player.position.row + 1;
			if (row > MAX_ROWS)
			{
				row = player.position.row;
				msg = "You have hit the south wall!";
			}
			break;
		case 'w':
			row = player.position.row - 1;
			if (row < MIN_ROWS)
			{
				row = player.position.row;
				msg = "You have hit the north wall!";
			}
			break;
		case 'd':
			column = player.position.column + 1;
			if (column > MAX_COLS)
			{
				column = player.position.column;
				msg = "You have hit the east wall!";
			}
			break;
		case 'n':
			if (player.hasNoisemaker)
			{
				msg = "You make an irritating noise!";
				monsterPause = true;
				monsterBPause = true;
			}
			else
			{
				msg = "You make a feeble whimper.";
			}
			break;
		case 'q':
			gameOver = true;
			msg = "Quitting?\n";
			break;
		default:
			movePlayer = false;
			break;
		}
		//******Check if the game is  over
		if (!gameOver)
		{
			//..... Check for events
			switch (cave[row][column])
			{
				//......If exit found, set flag to exit game
			case CAVE_EXIT:
				gameOver = true;
				msg = "You exit the cave.";
				break;
				//......If treasure found, set flag to show player has treasure
			case TREASURE:
				player.hasTreasure = true;
				treasure.isFound = true;
				msg = "You found the treasure!";
				break;
				//......If weapon found, set flag to show player has weapon
			case WEAPON:
				player.hasWeapon = true;
				weapon.isFound = true;
				msg = "You have found The Holy Weapon of Destiny.";
				killedValue = PLAYER_DIES_WITH_WEAPON;
				killMonsterValue = MONSTER_DIES_WITH_WEAPON;
				killMonsterBValue = MONSTER_DIES_WITH_WEAPON;
				break;
				//......If noise-maker found, set flag to show player has noise-maker
			case NOISEMAKER:
				player.hasNoisemaker = true;
				noisemaker.isFound = true;
				msg = "You found a noisemaker.";
				break;
				//......If torch found, set flag to show player has torch and display all items in cave
			case TORCH:
				player.hasTorch = true;
				torch.isFound = true;
				msg = "Let there be light! You found a torch.";
				caveExit.isVisible = true;
				weapon.isVisible = true;
				treasure.isVisible = true;
				noisemaker.isVisible = true;
				monster.isVisible = true;
				monsterB.isVisible = true;
				break;
				//......If monster found
			case MONSTER:
				if (!monster.isFound)
				{
					msg = "You have found the monster";
					//.......Resolve combat- FIGHT!
					gameOver = resolveCombat(player, monster, killedValue, killMonsterValue, msg, movePlayer, monsterPause);
				}
				break;
			case MONSTERB:
				if (!monsterB.isFound)
				{
					msg = "You have found the Boogy monster";
					//.......Resolve combat
					gameOver = resolveCombat(player, monster, killedValue, killMonsterValue, msg, movePlayer, monsterPause);

				}
				break;
			}
			//.....Move Player
			if (movePlayer)
			{
				cave[player.position.row][player.position.column] = EMPTY;          	//updates position information
				cave[row][column] = PLAYER;
				board[player.position.row + 2][player.position.column + 2] = MT;	//clear the screen where player was
				player.position.row = row;
				player.position.column = column;
			}
			movePlayer = true;
			//.....Process Monster
			if (!monster.isFound)
			{
				if (monsterPause)
				{
					// Monster paused. Check if Monster starts moving again
					if (rand() % 3 == 1)
					{
						monsterPause = false;
					}
				}
				else
				{
					//......Move Monster
					MonsterMoveCounter = (++MonsterMoveCounter) % 8;
					row = monster.row;
					column = monster.column;
					switch (MonsterMoveCounter)
					{
					case 0:
						column++;
						break;
					case 1:
						row--;
						break;
					case 2:
						row--;
						break;
					case 3:
						column--;
						break;
					case 4:
						column--;
						break;
					case 5:
						row++;
						break;
					case 6:
						row++;
						break;
					case 7:
						column++;
						break;
					default:
						break;
					}
					//......Check for events
					//.......If player found
					if (cave[row][column] == PLAYER)
					{
						msg = "The monster has found you";
						//........Resolve combat
						gameOver = resolveCombat(player, monster, killedValue - 2, killMonsterValue - 2, msg, movePlayer, monsterPause);
						monsterBPause = false;
					}
					else
					{
						//........Move Monster
						// <WK6 status=permanent>
						cave[monster.row][monster.column] = hold;           	// reveal what is under the monster
						board[monster.row + 2][monster.column + 2] = MT;	// clear the screen behind monster
						hold = cave[row][column];                           // save what the monster is about to move over
						monster.row = row;                                    // update monster's row
						monster.column = column;                             // update monster's column
						cave[row][column] = MONSTER;                         // change monster's location in the cave
					}
				}
								
                  if (!monsterB.isFound)
				{
					if (monsterBPause)
					{
						// Monster paused. Check if Monster starts moving again
						if (rand() % 3 == 1)
						{
							monsterBPause = false;
						}
					}
					else
					{
						//......Move Monster
						MonsterBMoveCounter = (++MonsterBMoveCounter) % 8;
						row = monsterB.row;
						column = monsterB.column;
						switch (MonsterBMoveCounter)
						{
						case 0:
							column++;
							break;
						case 1:
							row--;
							break;
						case 2:
							row--;
							break;
						case 3:
							column--;
							break;
						case 4:
							column--;
							break;
						case 5:
							row++;
							break;
						case 6:
							row++;
							break;
						case 7:
							column++;
							break;
						default:
							break;
						}
						//......Check for events
						//.......If player found
						if (cave[row][column] == PLAYER)
						{
							msg = "The monster has found you";
							//........Resolve combat
							gameOver = resolveCombat(player, monster, killedValue - 2, killMonsterValue - 2, msg, movePlayer, monsterPause);
							
						}
						else
						{
							//........Move Monster
							cave[monsterB.row][monsterB.column] = hold;           	// reveal what is under the boogy  monster
							board[monsterB.row + 2][monsterB.column + 2] = MT;  	// clear the screen behind boogy monster
							hold = cave[row][column];                                   	// save what the boogy monster is about to move over
							monsterB.row = row;                                          	// update boogy monster's row
							monsterB.column = column;                                    	// update boogy monster's column
							cave[row][column] = MONSTERB;                                	// change boogy monster's location in the cave
						}
					}
				}
			}
		}
	}
	//...End Game Loop

	//******End Game**************
	//...Provide the end win/loss message
	cout << msg.c_str() << endl;
	if (player.alive)
	{
		if (player.hasTreasure) msg = "You are alive and rich!";
		else msg = "You didn't get the treasure, but you live to seek adventure another day.";
	}
	else
	{
		msg = "RIP Adventurer";
	}
	cout << msg.c_str() << endl;
	//...Do clean-up
	//...Quit
	
	return 0;
}
//--------------------------------------------------------------------------------
//================================================================================

gameObject placeInCave(gameObjectType array[TOTAL_ROWS][TOTAL_COLS])
{
	int r, c;
	gameObject obj;
	do
	{
		r = rand() % 8;
		c = rand() % 8;
	} while (array[r][c] != EMPTY);
	obj.row = r;
	obj.column = c;
	obj.isFound = false;
	return obj;
}

bool checkVisible(gameObject x, gameObject y, int dist)
{
	if ((abs(x.row - y.row) < dist && (abs(x.column - y.column) < dist))) return true;
	return false;
}

bool showOnBoard(gameObject x)
{
	return ((x.isVisible) && (!x.isFound));
}
// resolve combat- FIGHT TO THE DEATH
bool resolveCombat(playerObject &p, gameObject &m, int YouDie, int MonsterDies, string &str, bool &moveP, bool &mPause)
{
	int combatRoll = rand() % 100;
	//........Killed by Monster, and player vanishes
	if (combatRoll > YouDie)
	{
		str += " and you have been eaten by a MONSTER!\nGAME OVER!";
		moveP = false;
		p.position.isVisible = false;
		p.alive = false;
		return true;
	}
	//........Kill Monster
	else if (combatRoll > MonsterDies)
	{
		if (p.hasWeapon) str += " and you slay the monster with \nThe Holy Weapon of Destiny.";
		else str += " , but--against all odds--you actually killed the monster with your bare hands.";
		m.isFound = true;
		return false;
	}
	//........Survive Attack
	else
	{
		str += " and you managed to survive the monster's attack.";
		moveP = false;
		mPause = true;
		return false;
	}
}

void gamecolor(unsigned char type)
{
	switch (type) {
	case 201: case 205: case 200: case 188: case 186: case 187:
		setcolor(FOREGROUND_RED);
		break;
	case 'W':
		setcolor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case '$':
		setcolor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case 'P':
		setcolor(FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
		break;
	case 'T':
		setcolor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case 'X':
		setcolor(BACKGROUND_BLUE | BACKGROUND_GREEN);
		break;
	case 'M':
		setcolor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case 'N':
		setcolor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	default:
		setcolor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	}
}

void setcolor(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}