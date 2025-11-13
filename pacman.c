//Pacman game for final project CS 355
//Authors: Tessa Masi, Shiqi Tan

//Determine code for ghost AI, determine how to create the maze. 
//Want to make a maze that looks like 'CS 355'


#include <ncurses.h>
#include <stdlib.h>


//SYMBOLS
#define WALL '#'
//want to create a customizable character soon (like Pac-Man w/ a bow)
#define PACMAN 'C' 
#define EMPTY ' '

//MAP SIZE
#define ROWS 10
#define COLS 20

//CREATE BASIC MAP (global)
char map[ROWS][COLS + 1] = {
    "###################",
    "#        #        #",
    "# ####   #   #### #",
    "#                C#",
    "#### ### ### ### ##",
    "#                 #",
    "# ####   #   #### #",
    "#        #        #",
    "###################",
};

//PAC-MAN Position
int pacman_x = 17;
int pacman_y = 3;


  



