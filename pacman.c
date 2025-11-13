/Pacman game for final project CS 355
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


//DRAW THE MAP
//mvprintw is an ncurses function that lets you move the cursor 
//and print text on the screen at the same time

void draw_map(){
    for(int y = 0; y < ROWS; y++){
        //prints the value for map[y] at row y, column 0 in the terminal window, so it prints each row
        mvprintw(y, 0, "%s", map[y]);
    }
}

//MOVE PAC-MAN IF NO WALL
void move_pacman(int dy, int dx){
    int new_y = pacman_y + dy;
    int new_x = pacman_x + dx;
    if ((map[new_y][new_x]) != WALL){
        map[pacman_y][pacman_x] = EMPTY;
        pacman_y = new_y;
        pacman_x = new_x;
        map[pacman_y][pacman_x] = PACMAN;
    }
}

int main(){
    initscr();                //Start ncurses
    noecho();                 //Do not print key presses
    curs_set(FALSE);          //Hide cursor
    keypad(stdscr, TRUE);     //Enable arrow keys

    int ch;
    while((ch = getch()) != 'q') { //Press 'q' to quit
        switch (ch){
            case KEY_UP:
                move_pacman(-1, 0);
                break;
            case KEY_DOWN:
                move_pacman(1, 0);
                break;
            case KEY_LEFT:
                move_pacman(0, -1);
                break;
            case KEY_RIGHT:
                move_pacman(0, 1);
                break;
        }
        clear();
        draw_map();
        refresh();
    }
    endwin();
    return 0;
}







  



