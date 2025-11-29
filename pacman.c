//Pacman game for final project CS 355
//Authors: Tessa Masi, Shiqi Tan

//Determine code for ghost AI, determine how to create the maze. 
//Want to make a maze that looks like 'CS 355'


#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

// SYMBOLS
#define WALL '#'
#define PACMAN 'C' 
#define EMPTY ' '
#define POINT '.'
#define CELL_SIZE 2  // Each corridor step is 2 map units wide


// MAP SIZE
#define ROWS 30
#define COLS 59

// Tunnel info
#define WIDTH 59
#define TUNNEL_HEIGHT 15


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
    int move_counter;
    int move_timer;
} Ghost;


//creates # of structs for the # of ghosts needed. 
Ghost ghosts[GHOST_AMOUNT];
int blue;
int powerpellet_time = 0;

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
    "###########################################################",
    "# . . . . . . . . . . . . . . . . . . . . . . . . . . . . #",
    "# . ######## . ######### . ####### . ################## . #", 
    "# . ######### . . . . . . . . . . . . . . . . . . . . . . #",
    "# . ######### . ######### . ####### . ##### . ##### . # . #",
    "# . ##     ## . ######### . #     # . ##### . ##### . # . #",
    "# . ##     ## . #       # . ###   # . ##### . ##### . # . #",
    "# . ##     ## . #       # . ###   # . ##### . # . . . # . #",
    "# . ##     ## . #   ##### . ###   # . # . . . ##### . # . #",
    "# . ##  ##### . #   ##### . ##   ## . ##### . ##### . # . #",
    "# . ##  ##### . #   ##### . ###   # . ##### . ##### . # . #",
    "# . ##  ##### . #       # . ###   # . ##### . ##### . . . #",
    "# . ##  ##### . #####   # . ###   # . ##### . . .## . # . #",
    "# . ##  ##### . #####   # . ###   # . . ### . ##### . # . #",
    "# . ##  ##### . #####   # . ###   # . ##### . ##### . # . #",
    "  . ##     ## . #       # . #     # . ##### . ##### . # .  ",
    "# . ##     ## . #       # . ####### . . . . . . . . . . . #",
    "# . ##     ## . #       # . . . . . . . ############### . #",
    "# . ##     ## . ######### . # . ##### . ############### . #",
    "# . ######### . ######### . # . ##### . ############### . #",
    "# . ######### . ######### . # . ##### . ############### . #",
    "# . ######### . . . . . . . # . . # . . . . . . . . . . . #",
    "# . . . . . . . . . ########### . # . ####### . ####### . #",
    "# . ####### . ### . ########### . # . ####### . ####### . #",
    "# . ####### . ### . ########### . # . ####### . ####### . #",
    "# . ####### . ### . ########### . # . ####### . ####### . #",
    "# . ####### . ### . ########### . # . ####### . ####### . #",
    "# . ####### . ### . ########### . # . ####### . ####### . #",
    "# . . . . . . . . . . . . . . . . . . ####### . . . . . . #",  // More open bottom
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
            } else if (ch == 'C') {
                attron(COLOR_PAIR(2));
                mvaddch(y, x, PACMAN);
                attroff(COLOR_PAIR(2));
            } else {
                mvaddch(y, x, ch);
            }
        }
    }
}

// Make a tunnel in the middle of the map
void tunnel(int *x, int y, int width) {
    if (y == TUNNEL_HEIGHT) {

        if (*x < 0) {
        *x = width - 1; 
    }
    else if (*x >= width) {
        *x = 0;   
        
    }
}
}


void draw_point_count(){
    char points_map[6][200] = {
    "           ######*   #####*  ########* ###*   ##* ########* ########* ",
    "           ##***##* ##***##*    ##**** ####*  ##*    ##**** ##******* ",
    "           ######** ##*  ##*    ##*    ##*##* ##*    ##*    ########* ",
    "           ##*****  ##*  ##*    ##*    ##**##*##*    ##*    ******##* ",
    "           ##*       #####*   #######* ##* *####*    ##*    ########* ",
    "           ***       *****    ******** ***  *****    ***    ********* "
    };
    for (int y = 0; y < 6; y++) {
        for (int x = COLS; x < COLS +70; x++) {
            char ch = map[y][x];
            if (ch == '#') {
                attron(COLOR_PAIR(1));
                mvaddch(y, x, ' ');
                attroff(COLOR_PAIR(1));
            } else if (ch == '*') {
                //attron(A_DIM);
                attron(COLOR_PAIR(2));
                mvaddch(y, x, ' ');
                attroff(COLOR_PAIR(2));
                //attroff(A_DIM);
                
            }
        }
    }
}

void point_counter(){
    if (map[pacman_x][pacman_y] == POINT){
        points++;
    }
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
     tunnel(&new_x, new_y, WIDTH);
    // Check boundaries and walls
    if (new_x >= 0 && new_x < COLS && new_y >= 0 && new_y < ROWS && 
        map[new_y][new_x] != WALL) {
        if(map[new_y][new_x] == POINT){
            new_x = new_x;
        } else if(map[new_y][new_x] == EMPTY && ((map[new_y][new_x-1] == POINT) || (map[new_y][new_x+1]) == POINT) ){
            if(map[new_y][new_x-1] == POINT){
                new_x = new_x - 1;
            } else {
                new_x = new_x + 1;
            }
        }
        // Clear old position
        map[pacman_y][pacman_x] = EMPTY;
        
        // Update position
        pacman_x = new_x;
        pacman_y = new_y;
        // Draw Pac-Man at new position
        map[pacman_y][pacman_x] = PACMAN;
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
        } while(map[gy][gx] == WALL || (map[gy][gx] == EMPTY));
        ghosts[i].x = ghosts[i].start_x = gx;
        ghosts[i].y = ghosts[i].start_y = gy;
        ghosts[i].direction = rand() % 4;
        ghosts[i].color_pair = i + 4;
        ghosts[i].speed = 3 + i*2;  // Much higher numbers = slower ghosts
        ghosts[i].move_counter = 0;
        ghosts[i].after_eaten = 0;
    }
}

//Ghosts movement
void move_ghosts() {
    for(int i = 0; i < GHOST_AMOUNT; i++) {
        ghosts[i].move_timer++;
        // Only move when timer reaches speed
        if (ghosts[i].move_timer < ghosts[i].speed) {
            continue;
        }
        // --- Ghost chooses direction ---
        int best_dir = -1;
        int min_dist = 1000;
        for(int d = 0; d < 4; d++) {
            int nx = ghosts[i].x + dx[d];
            int ny = ghosts[i].y + dy[d];
            tunnel(&nx, ny, WIDTH);

            if(nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS && map[ny][nx] != WALL) {
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
        ghosts[i].move_timer = 0;
    }
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
                ghosts[i].speed = 2 + i; //different speed
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

void power_pellets(){
    int rx, ry;
    if (points % 25 == 0){
        do {
                rx = rand() % (COLS-2) + 1;
                ry = rand() % (ROWS-2) + 1;
            } while(map[ry][rx] == WALL || (map[ry][rx] == EMPTY));
        mvaddch(ry, rx, '@');
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
    initialize_ghosts();
    draw_point_count();

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
                move_ghosts();  
                ghosts[i].move_counter = 0;
            }
        }
        
        // Move Pac-Man (now slowed by his speed counter)
        move_pacman();
        power_pellets();
        
        
        blue_ghosts();
        clear();
        draw_map();      
        draw_ghosts();  
        //draw_point_count(); 
        refresh();
        napms(50);  // You can adjust this for overall game speed
    }
    endwin();
    return 0;
}





  



