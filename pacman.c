//Pacman game for final project CS 355
//Authors: Tessa Masi, Shiqi Tan

//Determine code for ghost AI, determine how to create the maze. 
//Want to make a maze that looks like 'CS 355'


#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// SYMBOLS
#define WALL '#'
#define PACMAN 'C'
#define INNER_WALL 'I'
#define EMPTY ' '
#define POINT '.'
#define CELL_SIZE 2  // Each corridor step is 2 map units wide


// MAP SIZE
#define ROWS 30
#define COLS 59

//TUNNEL INFO
#define WIDTH 59
#define TUNNEL_HEIGHT 15


//create ghost
#define GHOST_AMOUNT 4

//PowerPellets
# define PELLET_AMOUNT 4

//GHOST structure
typedef struct {
    int x, y;   
    int start_x, start_y; 
    int direction;
    int color_pair;
    int scared;
    int speed;
    int after_eaten;
    int move_counter;
    int move_timer;
} Ghost;


//creates # of structs for the # of ghosts needed. 
Ghost ghosts[GHOST_AMOUNT];
int blue;
int powerpellet_time = 0;

//PowerPellets structure
typedef struct{
    int x, y;
    int exist;
    //exist = 1 means powerpellets are still exist
    //exist = 0 means powerpellets are eaten by pacman
}PowerPellet;

PowerPellet pellets[4];

int opposite(int dir){
    if(dir == 0) return 1;
    if(dir == 1) return 0;
    if(dir == 2) return 3;
    return 2;
}

//ghost direction
int dx[4] = {0, 0, -1, 1};  
int dy[4] = {-1, 1, 0, 0};

int running = 1;
int points = 0;

//CREATE BASIC MAP (global)
char map[ROWS][COLS+1] = {
//make the width wider
//they should only have one spaces to move, gaining  a innerwall to limit the road
//and we can change the color to make it more "fancy"
   "###########################################################",
    "#I. . . . . . . . . . . . . . . . . . . . . . . . . . . .I#",
    "#I.I########I.I#########I.I#######I.I##################I.I#", 
    "#I.I######### . . . . . . . . . . . . . . . . . . . . . .I#",
    "#I.I#########I.I#########I.I#######I.I#####I.I#####I.I#I.I#",
    "#I.I##     ##I.I#########I.I#     #I.I#####I.I#####I.I#I.I#",
    "#I.I##     ##I.I#       #I.I###   #I.I#####I.I#####I.I#I.I#",
    "#I.I##     ##I.I#       #I.I###   #I.I#####I.I# . . .I#I.I#",
    "#I.I##     ##I.I#   #####I.I###   #I.I# . . .I#####I.I#I.I#",
    "#I.I##  #####I.I#   #####I.I##    #I.I#####I.I#####I.I#I.I#",
    "#I.I##  #####I.I#   #####I.I###   #I.I#####I.I#####I.I#I.I#",
    "#I.I##  #####I.I#       #I.I###   #I.I#####I.I#####I. . .I#",
    "#I.I##  #####I.I#####   #I.I###   #I.I#####I. . .##I.I#I.I#",
    "#I.I##  #####I.I#####   #I.I###   #I. . ###I.I#####I.I#I.I#",
    "#I.I##  #####I.I#####   #I.I###   #I.I#####I.I#####I.I#I.I#",
    "  .I##     ##I.I#       #I.I#     #I.I#####I.I#####I.I#I.  ",
    "#I.I##     ##I.I#       #I.I#######I. . . . . . . . . . .I#",
    "#I.I##     ##I.I#       #I. . . . . . .I###############I.I#",
    "#I.I##     ##I.I#########I.I#I.I#####I.I###############I.I#",
    "#I.I#########I.I#########I.I#I.I#####I.I###############I.I#",
    "#I.I#########I.I#########I.I#I.I#####I.I###############I.I#",
    "#I.I#########I. . . . . . .I#I. .I#I. . . . . . . . . . .I#",
    "#I. . . . . . . . .I###########I.I#I.I#######I.I#######I.I#",
    "#I.I#######I.I###I.I###########I.I#I.I#######I.I#######I.I#",
    "#I.I#######I.I###I.I###########I.I#I.I#######I.I#######I.I#",
    "#I.I#######I.I###I.I###########I.I#I.I#######I.I#######I.I#",
    "#I.I#######I.I###I.I###########I.I#I.I#######I.I#######I.I#",
    "#I.I#######I.I###I.I###########I.I#I.I#######I.I#######I.I#",
    "#I. . . . . . . . . . . . . . . . . .I#######I. . . . . .I#",
    "###########################################################"
};

// PAC-MAN Position and movement (global variables)
int pacman_x = 17;
int pacman_y = 3;
int pacman_dx = 0;
int pacman_dy = 0;
int pacman_speed = 3;  // Higher number = slower movement
int pacman_move_counter = 0;

void draw_map() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            char ch = map[y][x];
            if (ch == '#') {
                attron(COLOR_PAIR(1));
                mvaddch(y, x, ' ');
                attroff(COLOR_PAIR(1));
            }else if (ch == 'I'){
                attron(COLOR_PAIR(1));
                mvaddch(y, x, ' ');
                attroff(COLOR_PAIR(1));
            }else if (ch == 'C') {
                attron(COLOR_PAIR(2));
                mvaddch(y, x, PACMAN);
                attroff(COLOR_PAIR(2));
            } else if (ch == '@'){
                attron(COLOR_PAIR(3));
                mvaddch(y, x, '@');
                attroff(COLOR_PAIR(3));
            }
            else {
                mvaddch(y, x, ch);
            }
        }
    }
}

// Tunnel
void tunnel(int *x, int y, int width) {
    if (y == TUNNEL_HEIGHT) {
        if (*x < 0) *x = width - 1;
        else if (*x >= width) *x = 0;
    }
}


// Draw a small banner / points display at the TOP LEFT (row 0..5, col 0..width)
void draw_pacman_label() {
    static int last_second = 0;
    static int toggle = 0;

    time_t now = time(NULL);
    if (now != last_second) {
        last_second = now;
        toggle = !toggle;    // switch color every second
    }

    const char *points_map[6] = {
        "  ######*   #####*   ######* ###*   ###*  #####*  ###*   ##* ",
        "  ##***##* ##***##* ##****** ####* ####* ##***##* ####*  ##* ",
        "  ######** #######* ##*      ##*####*##* #######* ##*##* ##* ",
        "  ##*****  ##***##* ##*      ##**##**##* ##***##* ##**##*##* ",
        "  ##*      ##*  ##* *######* ##* **  ##* ##*  ##* ##* *####* ",
        "  ***      ***  ***  ******* ***     *** ***  *** ***  ***** "
    };
    int start_row = ROWS + 1;  // << draw at the bottom ALWAYS
    int start_col = 0;         // or center later

    for (int y = 0; y < 6; y++) {
        const char *row = points_map[y];

        for (int x = 0; row[x] != '\0'; x++) {
            char ch = row[x];
            int scr_y = start_row + y;
            int scr_x = start_col + x;
            if (ch == '*') {
                if (toggle)
                    attron(COLOR_PAIR(2));
                else
                    attron(COLOR_PAIR(5));

                mvaddch(scr_y, scr_x, POINT);
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(5));
            }
            else{
                attron(A_DIM);
                mvaddch(scr_y, scr_x, ch);
                attroff(A_DIM);
            }
        } 
    }
}


// Draw the points collected in the top-left corner
void draw_points_corner() {
    attron(COLOR_PAIR(3));          // Use the same color as the points
    mvprintw(30, 0, "Points: %d", points);  // row 0, col 0
    attroff(COLOR_PAIR(5));
    mvprintw(30, 10, "Lives: %d", 3);  // row 0, col 0
}



// Modify move_pacman function
void move_pacman() {
    pacman_move_counter++;
    if (pacman_move_counter < pacman_speed) {
        return;  // Skip movement this frame
    }
    pacman_move_counter = 0;
    
    int new_x = pacman_x + pacman_dx;
    int new_y = pacman_y + pacman_dy;
    //when go to the tunnel location, apply tunnel
    tunnel(&new_x, new_y, WIDTH);
        
    // Check boundaries and walls
    if (new_x >= 0 && new_x < COLS && new_y >= 0 && new_y < ROWS && 
        map[new_y][new_x] != WALL  && map[new_y][new_x] != INNER_WALL ) {

        if(map[new_y][new_x] == POINT) {
        points++;
    }

    
    if(map[pacman_y][pacman_x] == PACMAN) {
        map[pacman_y][pacman_x] = EMPTY;  
    }
        
        /*if(map[new_y][new_x] == POINT){
            new_x = new_x;
            points++;
            
        } else if(map[new_y][new_x] == EMPTY && ((map[new_y][new_x-1] == POINT) || (map[new_y][new_x+1]) == POINT) ){
            if(map[new_y][new_x-1] == POINT){
                new_x = new_x - 1;
            } else {
                new_x = new_x + 1;
            }
        }
        // Clear old position
        map[pacman_y][pacman_x] = EMPTY;*/
        
        // Update position
        pacman_x = new_x;
        pacman_y = new_y;
        
        // Draw Pac-Man at new position
        map[pacman_y][pacman_x] = PACMAN;
    }
}

//Initialize powerpellets
void initialize_pellets() {
    int positions[PELLET_AMOUNT][2] = {
        {2, 1},    
        {56, 1},   
        {2, 28},   
        {56, 28}   
    };
    for(int i=0; i<PELLET_AMOUNT; i++) {
        pellets[i].x = positions[i][0];
        pellets[i].y = positions[i][1];
        pellets[i].exist = 1;
        map[pellets[i].y][pellets[i].x] = '@'; 
    }
}

// Check if Pac-Man eats pellet
void check_pellets() {
    for(int i=0; i<PELLET_AMOUNT; i++) {
        if(pellets[i].exist && pacman_x == pellets[i].x && pacman_y == pellets[i].y){
            pellets[i].exist = 0;
            map[pacman_y][pacman_x] = PACMAN;
            blue = 1; 
            powerpellet_time = time(NULL);

            for(int j=0;j<GHOST_AMOUNT;j++){
                ghosts[j].scared = 1;
                ghosts[j].after_eaten = 0;
            }
        }
    }
}



//initialize ghosts
// In initialize_ghosts(), make ghosts much slower:
void initialize_ghosts(){
    srand(time(NULL));
    for(int i = 0; i < GHOST_AMOUNT; i++){
        int gx, gy;
        ghosts[i].scared = 0;  // Start not scared
        //make sure get a right position in map
        do {
            gx = rand() % (COLS-2) + 1;
            gy = rand() % (ROWS-2) + 1;
        } while(map[gy][gx] == WALL || map[gy][gx] == INNER_WALL || map[gy][gx] == EMPTY || map[gy][gx] == PACMAN);
        ghosts[i].x = ghosts[i].start_x = gx;
        ghosts[i].y = ghosts[i].start_y = gy;
        ghosts[i].direction = rand() % 4;
        ghosts[i].color_pair = i + 4;
        ghosts[i].speed = 3 + i;  // Much higher numbers = slower ghosts
        ghosts[i].move_counter = 0;
        ghosts[i].after_eaten = 0;
    }
}

//Ghosts movement, follow pacman
void move_ghosts12(int i) {
    for(int i = 0; i < GHOST_AMOUNT; i++) {
        
        // Only move when timer reaches speed
        if (ghosts[i].move_counter < ghosts[i].speed) {
            continue;
        }
        // --- Ghost chooses direction ---
        int best_dir = -1;
        int min_dist = 1000;
        for(int d = 0; d < 4; d++) {
            int nx = ghosts[i].x + dx[d];
            int ny = ghosts[i].y + dy[d];
            tunnel(&nx, ny, WIDTH);

            if(nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && map[ny][nx] != WALL && map[ny][nx] != INNER_WALL) {
                int dist = abs(nx - pacman_x) + abs(ny - pacman_y);

                if(dist < min_dist) {
                    min_dist = dist;
                    best_dir = d;
                }
            }
        }
        // --- Move ghost ---
        if (best_dir != -1) {
            ghosts[i].x += dx[best_dir];
            ghosts[i].y += dy[best_dir];
        }
        // Reset timer
        ghosts[i].move_counter = 0;
    }
}

//ghosts 2, randomly move
void move_ghosts3(int i){
     if (ghosts[i].move_counter < ghosts[i].speed) return;
    int nx = ghosts[i].x + dx[ghosts[i].direction];
    int ny = ghosts[i].y + dy[ghosts[i].direction];
    tunnel(&nx, ny, WIDTH);

    if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS &&
        map[ny][nx] != WALL && map[ny][nx] != INNER_WALL) {
        ghosts[i].x = nx;
        ghosts[i].y = ny;
    } else {
        // Choose a new valid direction randomly
        int possible[4];
        int count = 0;
        for (int d = 0; d < 4; d++) {
            if (d == opposite(ghosts[i].direction)) continue;
            int tx = ghosts[i].x + dx[d];
            int ty = ghosts[i].y + dy[d];
            if (tx >= 0 && tx < COLS && ty >= 0 && ty < ROWS &&
                map[ty][tx] != WALL && map[ty][tx] != INNER_WALL) {
                possible[count++] = d;
            }
        }
        if (count > 0) {
            ghosts[i].direction = possible[rand() % count];
            ghosts[i].x += dx[ghosts[i].direction];
            ghosts[i].y += dy[ghosts[i].direction];
        } else {
            ghosts[i].direction = opposite(ghosts[i].direction);
        }
    }
    ghosts[i].move_counter = 0;
}

//ghosts 4, somtimes follow, somtimes randomly
void move_ghosts4(int i){
    if (ghosts[i].move_counter < ghosts[i].speed) return;
    int choice = rand() % 3;
    if (choice == 0) {
        move_ghosts12(i); //follow
    } else {
        move_ghosts3(i);   // randomly
    }

    ghosts[i].move_counter = 0;
}


//pacman encounter ghosts
void check_encounter(){
    for (int i = 0; i < 4; i++){
        if(ghosts[i].x == pacman_x && ghosts[i].y == pacman_y){
            //if power pellets, blue mode
            if(ghosts[i].scared){
                ghosts[i].x = ghosts[i].start_x;
                ghosts[i].y = ghosts[i].start_y;
                ghosts[i].scared = 0;
                ghosts[i].speed = 3 + i; //different speed
                ghosts[i].after_eaten = 1;
                ghosts[i].move_counter = 0;
                ghosts[i].direction = rand()%4;
                continue;
            } else{
                //no power pellets
                mvprintw(ROWS+1, 0, "Game-Over!");
                refresh();
                getch();
                endwin();
                exit(0);
            }   
        }
    }
}

//DRAW GHOSTS
void draw_ghosts(){
    for(int i=0; i < GHOST_AMOUNT; i++){
        attron(COLOR_PAIR(ghosts[i].color_pair));
        mvaddch(ghosts[i].y, ghosts[i].x, 'G'); 
        attroff(COLOR_PAIR(ghosts[i].color_pair));
    }
}

// Spawn power-pellets occasionally. Track last_spawned so we don't spawn every frame.
int last_power_spawn_points = -1;

/*void power_pellets() {
    // spawn at multiples of 25 once (ignore zero)
    if (points > 0 && points % 25 == 0 && points != last_power_spawn_points) {
        int rx, ry;
        // try to find a '.' location to replace with '@' (power pellet)
        for (int tries = 0; tries < 200; tries++) {
            rx = rand() % (COLS-2) + 1;
            ry = rand() % (ROWS-2) + 1;
            if (map[ry][rx] == POINT) {
                map[ry][rx] = '@';   // mark power pellet on map
                last_power_spawn_points = points;
                break;
            }
        }
    }
}*/


void blue_ghosts() {
    for(int i = 0; i < GHOST_AMOUNT; i++) {
        // 1. just enter blue mode
        if(blue == 1 && ghosts[i].scared && !ghosts[i].after_eaten) {
            ghosts[i].speed = 10; 
            ghosts[i].color_pair = 8;
        } else {
            ghosts[i].color_pair = i + 4;
            ghosts[i].speed = 3 + i; 
        }
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);  // Make getch() non-blocking
    start_color();
    use_default_colors();

    init_pair(1, COLOR_BLUE, COLOR_BLUE);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_WHITE, -1);
    // Add color pairs for ghosts
    init_pair(4, COLOR_RED, -1);
    init_pair(5, COLOR_CYAN, -1);
    init_pair(6, COLOR_MAGENTA, -1); 
    init_pair(7, COLOR_GREEN, -1);
    init_pair(8, COLOR_BLUE, -1);  // For scared ghosts



    // Initialize Pac-Man position
    map[pacman_y][pacman_x] = PACMAN;
    initialize_pellets();
    
    initialize_ghosts();
    

    while (running) {
        int ch = getch();
        
        // Handle input
        switch(ch) {
            case KEY_UP:    
                pacman_dy = -1; 
                pacman_dx = 0;
                check_encounter(); 
                break;
            case KEY_DOWN:  
                pacman_dy = 1;  
                pacman_dx = 0; 
                check_encounter();
                break;
            case KEY_LEFT:  
                pacman_dx = -1; 
                pacman_dy = 0; 
                check_encounter();
                break;
            case KEY_RIGHT: 
                pacman_dx = 1;  
                pacman_dy = 0; 
                check_encounter();
                break;
            case 'q':       
                running = 0; 
                check_encounter();
                break;
        }
        
        // Move ghosts (they're already slowed by their speed counters)
        for(int i = 0; i < GHOST_AMOUNT; i++){
            ghosts[i].move_counter++;
            if(ghosts[i].move_counter >= ghosts[i].speed){
                if(i == 0 || i == 1)      move_ghosts12(i); // follow    
                else if(i == 2) move_ghosts3(i); //randomly   
                else if(i == 3) move_ghosts4(i);  //follow and randomly   
                
                //move_ghosts(i);
                check_encounter();  
                ghosts[i].move_counter = 0;
        }
    }
    
        // blue mode timer check
        if (blue == 1 && powerpellet_time == 0){
            powerpellet_time = time(NULL); // start timer when first triggered
        }
    
        if (blue == 1 && time(NULL) - powerpellet_time >= 10) {
        blue = 0;
        powerpellet_time = 0;
        for (int i = 0; i < GHOST_AMOUNT; i++) {
            ghosts[i].scared = 0;        
            ghosts[i].color_pair = i + 4;
            ghosts[i].speed = 3 + i;
        }
    }
        
        // Move Pac-Man (now slowed by his speed counter)
        check_pellets(); 
        move_pacman();
        check_encounter();
        blue_ghosts();
        
        clear();
        draw_map();  
        draw_ghosts();  
        
        draw_pacman_label();  
        draw_points_corner();
        refresh();
        napms(75);  // You can adjust this for overall game speed
    }
    endwin();
    return 0;
}

