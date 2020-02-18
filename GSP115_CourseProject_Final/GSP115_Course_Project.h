#pragma once
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

enum gameObjectType
{
	EMPTY, PLAYER, TREASURE, MONSTER, MONSTERB, WEAPON, TORCH, NOISEMAKER, CAVE_EXIT 
};

struct gameObject
{
	int row;                    	// row position of the object
	int column;                 	// column position of the object
	bool isFound;       	// flag to indicate if the object has been found (or is dead, in the case of the monster)
	bool isVisible;     	// flag to indicate if the object can be seen on the board
};

struct playerObject
{
	bool alive;         	// flag to indicate if the player is alive or dead
	bool hasWeapon;     	// flag to indicate if the player has the weapon
	bool hasTreasure;   	// flag to indicate if the player has the treasure
	bool hasTorch;      	// flag to indicate if the player has the torch
	bool hasNoisemaker; 	// flag to indicate if the player has the noisemaker
	gameObject position;	// variables for row, column and visibility
};

