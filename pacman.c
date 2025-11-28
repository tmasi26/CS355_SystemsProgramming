//Pacman game for final project CS 355
//Authors: Tessa Masi, Shiqi Tan

//Determine code for ghost AI, determine how to create the maze. 
//Want to make a maze that looks like 'CS 355'


#include <ncurses.h>
#include <stdlib.h>
#include <time.h>


//SYMBOLS
#define WALL '#'
//want to create a customizable character soon (like Pac-Man w/ a bow)
#define PACMAN 'C' 
#define EMPTY ' '

//MAP SIZE
#define ROWS 30
#define COLS 30

//create ghost
#define GHOST_AMOUNT 4

//GHOST structure
typedef struct {
    int x, y;             
    int start_x, start_y; 
    int direction;         
    int color_pair;
    int scared;
    int speed;
    int after_eaten;
} Ghost;

Ghost ghosts[GHOST_AMOUNT];

int blue;
int powerpellet_time = 0;


//CREATE BASIC MAP (global)
char map[ROWS][COLS+1] = {
"##############################",
"#............................#",
"#.#####.########.######.####.#",
"#.#####......................#",
"#.#####.#####.####.###.###.#.#",
"#.#   #.#####.#  #.###.###.#.#",
"#.#   #.#   #.## #.###.###.#.#",
"#.#   #.#   #.## #.###.#...#.#",
"#.#   #.# ###.## #.#...###.#.#",
"#.# ###.# ###.#  #.###.###.#.#",
"#.# ###.# ###.## #.###.###.#.#",
"#.# ###.#   #.## #.###.###...#",
"#.# ###.### #.## #.###...#.#.#",
"#.# ###.### #.## #..##.###.#.#",
"#.# ###.### #.## #.###.###.#.#",
"#.#   #.#   #.#  #.###.###.#.#",
"#.#   #.#   #.####...........#",
"#.#   #.#   #.......########.#",
"#.#   #.#####.#.###.########.#",
"#.#####.#####.#.###.########.#",
"#.#####.#####.#.###.########.#",
"#.#####.......#..#...........#",
"#.........######.#.####.####.#",
"#.####.##.######.#.####.####.#",
"#.####.##.######.#.####.####.#",
"#.####.##.######.#.####.####.#",
"#.####.##.######.#.####.####.#",
"#.####.##.######.#.####.####.#",
"#.........######...####......#",
"##############################"
};



//PAC-MAN Position
int pacman_x = 17;
int pacman_y = 3;


//DRAW THE MAP
//mvprintw is an ncurses function that lets you move the cursor 
//and print text on the screen at the same time

void draw_map(){
    for (int y = 0; y < ROWS; y++){
        for(int x = 0; x < COLS; x++){
            char ch = map[y][x];
            if ( ch == '#'){
                attron(COLOR_PAIR(1));
                mvaddch(y, x, ' '); //print a **space** colored blue
                attroff(COLOR_PAIR(1));
            }
            else if (ch == 'C'){
                attron(COLOR_PAIR(2));
                mvaddch(y, x, PACMAN); // actually draw Pac-Man
                attroff(COLOR_PAIR(2));
            }
            else{
                mvaddch(y, x, ch); //free space or dots
            }
        }
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

//ghost direction
    int dx[4] = {0, 0, -1, 1};  
    int dy[4] = {-1, 1, 0, 0};


    //initialize ghosts
    void initialize_ghosts(){
        srand(time(NULL));
        for(int i=0; i<4; i++){
            int gx, gy;
            ghosts[i].scared = 1;
            //make sure get a right position in map
            do {
                gx = rand() % (COLS-2) + 1;
                gy = rand() % (ROWS-2) + 1;
            } while(map[gy][gx] == WALL || (gx == pacman_x && gy == pacman_y)||map[gy][gx] != '.');

            //get start
            ghosts[i].x = ghosts[i].start_x = gx;
            ghosts[i].y = ghosts[i].start_y = gy;
            ghosts[i].direction = rand() % 4;
            ghosts[i].color_pair = i + 4;
            ghosts[i].speed = 2 + i; //different speed
            ghosts[i].move_counter = 0;
        }
    }


    int opposite(int dir){
        if(dir == 0) return 1;
        if(dir == 1) return 0;
        if(dir == 2) return 3;
        return 2;
    }

    //Ghosts movement
    void move_ghosts(int i){
       
            //next position
            int nx = ghosts[i].x + dx[ghosts[i].direction];
            int ny = ghosts[i].y + dy[ghosts[i].direction];
            //no wall, keep going
            if(map[ny][nx] != WALL){
                ghosts[i].x = nx;
                ghosts[i].y = ny;
            } else {
                //change direction
                int possible[4];
                int count=0;
                for(int d=0; d<4; d++){
                    //reduce go back and forth
                    if(d == opposite(ghosts[i].direction)){
                        continue; 
                    }
                    //test all direction 
                    int tx = ghosts[i].x + dx[d];
                    int ty = ghosts[i].y + dy[d];
                    if(map[ty][tx] != WALL) possible[count++] = d;
                }
                if(count > 0){
                    ghosts[i].direction = possible[rand()%count];
                    ghosts[i].x += dx[ghosts[i].direction];
                    ghosts[i].y += dy[ghosts[i].direction];
                } else {
                    ghosts[i].direction = opposite(ghosts[i].direction);
                }
            }

            //pacman encounter ghosts
            if(ghosts[i].x == pacman_x && ghosts[i].y == pacman_y){
                //if power pellets, blue mode
                if(ghosts[i].scared){
                    ghosts[i].x = ghosts[i].start_x;
                    ghosts[i].y = ghosts[i].start_y;
                    ghosts[i].scared = 0;
                    ghosts[i].speed = 2 + i; //different speed
                    ghosts[i].move_counter = 0;
                    ghosts[i].direction = rand()%4;
                    continue;
                }else{
                    //no power pellets
                    mvprintw(ROWS+1, 0, "Game-Over!");
                    refresh();
                    getch();
                    endwin();
                    exit(0);
                }

               
            }
    }

    //DRAW GHOSTS
    void draw_ghosts(){
        for(int i=0; i<GHOST_AMOUNT; i++){
            attron(COLOR_PAIR(ghosts[i].color_pair));
            mvaddch(ghosts[i].y, ghosts[i].x, 'G'); 
            attroff(COLOR_PAIR(ghosts[i].color_pair));
        }
    }

    //blue mode
    void blue_ghosts() {
    for(int i = 0; i < GHOST_AMOUNT; i++) {
        // 1. after eaten by pacman ,recover
        if(ghosts[i].after_eaten) {
            ghosts[i].color_pair = i + 4;
        } else if(blue == 1 && ghosts[i].scared) {
            // 2. blue mode
            ghosts[i].color_pair = 8;
        } else {
            // 3. no powerpellet , no blue mode
            ghosts[i].color_pair = i + 4;
        }
    }
}


int main(){
    initscr();                //Start ncurses
    noecho();                 //Do not print key presses
    curs_set(FALSE);          //Hide cursor
    keypad(stdscr, TRUE);     //Enable arrow keys
    start_color();            //enable colors
    use_default_colors();     //Optional: use terminal default

    init_pair(1, COLOR_BLUE, COLOR_BLUE);       //walls: blue on blue
    init_pair(2, COLOR_YELLOW, -1);             //pacman: yellow foreground, default background
    init_pair(3, COLOR_WHITE, -1);              //Free space: white

    init_pair(4, COLOR_RED, -1);                // ghost 1 
    init_pair(5, COLOR_CYAN, -1);               // ghost 2 
    init_pair(6, COLOR_MAGENTA, -1);            // ghost 3
    init_pair(7, COLOR_GREEN, -1);              // ghost 4
    init_pair(8, COLOR_BLUE, -1);               //scared mode

    initialize_ghosts();

    blue = 1; //test blue mode
    

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
        for(int i = 0; i < GHOST_AMOUNT; i++){
            ghosts[i].move_counter++;
            // slow speed, mover until counter
            if(ghosts[i].move_counter >= ghosts[i].speed){
                move_ghosts(i);  
                ghosts[i].move_counter = 0;
    }
}
        // blue mode timer check
        if (blue == 1 && powerpellet_time == 0){
            powerpellet_time = time(NULL); // start timer when first triggered
        }
    
        if (blue == 1 && time(NULL) - powerpellet_time >= 10){
            blue = 0;              // turn off blue mode
            powerpellet_time = 0;  // reset timer
                     
        }
        blue_ghosts();
        clear();
        draw_map();      
        draw_ghosts();   
        refresh();
        napms(180);
    }
    endwin();
    return 0;
}









  



