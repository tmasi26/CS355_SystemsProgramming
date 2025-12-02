//Pacman game for final project CS 355
//Authors: Tessa Masi, Shiqi Tan

//This game of Pacman plays 1 round with:
    // 4 ghosts (2 DFS search, 1 random movement, and 1 60/30 DFS, random movement)
    // 4 power pellets
    // ~ 3 fruit
    // a tunnel
    // flashing Pacman sign
    // U WIN / GAME OVER sign
    // 3 lives

// IMPORT LIBRARIES
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// SYMBOLS
#define WALL '#'
#define PACMAN 'C' 
#define EMPTY ' '
#define POINT '.'
#define CELL_SIZE 2         // Each corridor step is 2 map units wide
#define WIDTH 59            //Location of east tunnel
#define TUNNEL_HEIGHT 15    //tunnel height

// MAP SIZE
#define ROWS 30
#define COLS 60

//4 Ghosts created
#define GHOST_AMOUNT 4
//4 Power pellets created
# define PELLET_AMOUNT 4

//GHOST structure
//Shiqi
typedef struct {
    int x, y;  
    int prev_x, prev_y; 
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

//PowerPellets structure
//Shiqi
typedef struct{
    int x, y;
    int exist;
    //exist = 1 means powerpellets are still exist
    //exist = 0 means powerpellets are eaten by pacman
}PowerPellet;

// set a game stauts
int win = 0;

//Creates 4 pellets
PowerPellet pellets[PELLET_AMOUNT];
// if blue = 0, ghosts normal. blue = 1, ghosts in scared mode
int blue;
//bluemode time, initialized to zero. Set to 10s when blue mode in action
int powerpellet_time = 0;
//holds locations of where fruit have spawned to memory
int fruit_location[12][2];
//number of fruits collected
int fruit_count = 0;
//if pacman eat a ghosts, there is some extra points
int ghosts_eaten = 0;

//Shiqi
//Directional changes if ghost needs to turn around
int opposite(int dir){
    if(dir == 0) return 1;
    if(dir == 1) return 0;
    if(dir == 2) return 3;
    return 2;
}

//ghost direction
// {down, up, left, right}
int dx[4] = {0, 0, -1, 1}; // {0, 0, left, right}
int dy[4] = {-1, 1, 0, 0}; // {down, up, 0, 0}

//Check if game is running
int running = 1;
//Running total of points
int points = 0;
//Number of lives
int lives = 5;

// PAC-MAN Position and movement (global variables)
int pacman_x = 17;
int pacman_y = 3;
int pacman_dx = 0;
int pacman_dy = 0;
int pacman_speed = 3;  // Higher number = slower movement
int pacman_move_counter = 0;

// Spawn fruits occasionally. Track last_spawned so we don't spawn every frame.
int last_fruit_spawn_points = -1;

//CREATE BASIC MAP (global)
//Shiqi + Tess
//It says CS 355 :)
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
    "  . ##  ##### . #####   # . ###   # . ##### . ##### . # .  ",
    "  . ##     ## . #       # . #     # . ##### . ##### . # .  ",
    "  . ##     ## . #       # . ####### . . . . . . . . . . .  ",
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
    "# . . . . . . . . . . . . . . . . . . ####### . . . . . . #",
    "###########################################################"
};

//Tess + Shiqi
//This method draws out the map with the correct colors
void draw_map() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            char ch = map[y][x];
            //if #, replace with blue wall
            if (ch == '#') {
                attron(COLOR_PAIR(1));
                mvaddch(y, x, ' ');
                attroff(COLOR_PAIR(1));
            //if C, make yellow
            }else if (ch == 'C') {
                attron(COLOR_PAIR(2));
                mvaddch(y, x, PACMAN);
                attroff(COLOR_PAIR(2));
            //if O keep white
            } else if (ch == 'O'){
                attron(COLOR_PAIR(3));
                mvaddch(y, x, 'O');
                attroff(COLOR_PAIR(3));
            }
            else {
                mvaddch(y, x, ch);
            }
        }
    }
}

//Tess
// This method draws a flashing pacman banner at the bottom of the screen
void draw_pacman_label() {
    static int last_second = 0;
    static int toggle = 0;
    //This creates the "flashing lights"
    time_t now = time(NULL); //will give the current time
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
    int start_row = ROWS + 1;  // draw at the bottom 
    int start_col = 0;      

    for (int y = 0; y < 6; y++) {
        const char *row = points_map[y];

        for (int x = 0; row[x] != '\0'; x++) {
            char ch = row[x];
            int scr_y = start_row + y;
            int scr_x = start_col + x;
            if (ch == '*') {
                if (toggle)
                    attron(COLOR_PAIR(2)); //switch between blue and orange
                else
                    attron(COLOR_PAIR(5)); //switch between blue and orange

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


//Shiqi
// This method creates a tunnel for pacman to travel through
void tunnel(int *x, int y, int width) {
    if (y == TUNNEL_HEIGHT || (y == TUNNEL_HEIGHT + 1) || (y == TUNNEL_HEIGHT - 1)) {
        if (*x < 0){
            *x = width - 1;
        } else if (*x >= width){
            *x = 1;
        }
    }
}

//Tess
// This method draws the points and lives left at the bottom of the screen
void draw_points_corner() {
    attron(COLOR_PAIR(3));          // Use the same color as the points
    mvprintw(30, 0, "Points: %d", points);  // row 0, col 0
    attroff(COLOR_PAIR(5));
    mvprintw(30, 25, "Lives: %d", lives);  // row 0, col 0
}


//Tess + Shiqi
//This method controls the movement for pacman
//if in a column where there are points, pacman will automatically lock to the 
//middle column
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
        map[new_y][new_x] != WALL) {
        if(map[new_y][new_x] == POINT){
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
        map[pacman_y][pacman_x] = EMPTY;
        
        // Update position
        pacman_x = new_x;
        pacman_y = new_y;
        
        // Draw Pac-Man at new position
        map[pacman_y][pacman_x] = PACMAN;
    }
}

//Shiqi
//This method spawns the power pellets
void initialize_pellets() {
    //pellets put in corners of the map
    int positions[PELLET_AMOUNT][2] = {
        {2, 1},    
        {56, 1},   
        {2, 28},   
        {56, 28}   
    };
    for(int i = 0; i < PELLET_AMOUNT; i++) {
        pellets[i].x = positions[i][0];
        pellets[i].y = positions[i][1];
        pellets[i].exist = 1;
        map[pellets[i].y][pellets[i].x] = 'O'; 
    }
}
//Shiqi
//This method checks to see if pacman ate the power pellet
void check_pellets() {
    for(int i = 0; i < PELLET_AMOUNT; i++) {
        if(pellets[i].exist && pacman_x == pellets[i].x && pacman_y == pellets[i].y){
            pellets[i].exist = 0;
            map[pacman_y][pacman_x] = PACMAN;
            blue = 1; 
            powerpellet_time = time(NULL);

            for(int j = 0; j < GHOST_AMOUNT; j++){
                ghosts[j].scared = 1;
                ghosts[j].after_eaten = 0;
            }
        }
    }
}


//Tess + Shiqi
//This method initializes the ghosts in the game in the corners of the map
void initialize_ghosts(){
    srand(time(NULL));
    for(int i = 0; i < GHOST_AMOUNT; i++){
        int gx, gy;
        ghosts[i].scared = 0;  // Start not scared
        //make sure get a right position in map
        if (i == 0){
            gx = 1;
            gy = 1;
        }else if (i == 1){
            gx = COLS - 3;
            gy = 1;
        }
        else if(i == 2){
            gx = 1;
            gy = ROWS - 3;
        }
        else {
            gx = COLS - 3;
            gy = ROWS - 3;
        }
        ghosts[i].x = ghosts[i].start_x = gx;
        ghosts[i].y = ghosts[i].start_y = gy;
        ghosts[i].direction = rand() % 4;
        ghosts[i].color_pair = i + 4;
        ghosts[i].speed = 3 + i*2;  // Much higher numbers = slower ghosts
        ghosts[i].move_counter = 0;
        ghosts[i].after_eaten = 0;
    }
}

//Tess
//This method resets the ghosts locations if they are either:
//eaten by pacman
//if they eat pacman
void reset_ghost(){
    for(int i = 0; i < GHOST_AMOUNT; i++){
        int gx, gy;
        //ghosts[i].scared = 0;  // Start not scared
        //make sure get a right position in map
        if (i == 0){
            gx = 1;
            gy = 1;
        }else if (i == 1){
            gx = COLS - 3;
            gy = 1;
        }
        else if(i == 2){
            gx = 1;
            gy = ROWS - 2;
        }
        else {
            gx = COLS - 3;
            gy = ROWS - 2;
        }
        ghosts[i].x = ghosts[i].start_x = gx;
        ghosts[i].y = ghosts[i].start_y = gy;
    }
}


//Tess
//Movement of the first two ghosts, DFS. 
void move_ghosts12(int i) {
    //for(int i = 0; i < 2; i++) {
        // Only move when timer reaches speed
        if (ghosts[i].move_counter < ghosts[i].speed) {
            //continue;
        }
        int gx = ghosts[i].x;
        int gy = ghosts[i].y;
        ghosts[i].prev_x = ghosts[i].x;
        ghosts[i].prev_y = ghosts[i].y;
        // --- Ghost chooses direction ---
        int best_dir = -1;
        int min_dist = 1000;
        for(int d = 0; d < 4; d++) {
            int nx = ghosts[i].x + dx[d];
            int ny = ghosts[i].y + dy[d];

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
        ghosts[i].move_counter = 0;
    //}
}


//Shiqi
//Movement of Ghost 3, random movement
void move_ghosts3(int i){
     if (ghosts[i].move_counter < ghosts[i].speed) return;
    int nx = ghosts[i].x + dx[ghosts[i].direction];
    int ny = ghosts[i].y + dy[ghosts[i].direction];
    tunnel(&nx, ny, WIDTH);

    if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS &&
        map[ny][nx] != WALL) {
        ghosts[i].x = nx;
        ghosts[i].y = ny;
    } else {
        // Choose a new valid direction randomly
        int possible[4];
        int count = 0;
        for (int d = 0; d < 4; d++) {
            //reduce go back and forth
            if (d == opposite(ghosts[i].direction)) continue;
            int tx = ghosts[i].x + dx[d];
            int ty = ghosts[i].y + dy[d];
            if (tx >= 0 && tx < COLS && ty >= 0 && ty < ROWS &&
                map[ty][tx] != WALL) {
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


//Shiqi
//Ghost 4, either DFS or random movement
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

//Shiqi
char *game_win[6]= {
    "##*   ##*    ##*  ####*  ##* ########* ####*   *##* ",
    "##*   ##*    ##*  ####*  ##*    ##***  #####*  *##*",
    "##*   ##*    ##* ##**##* ##*    ##*    ##* ##* *##* ",
    "##*   ##*     ####*   ####*     ##*    ##** ##**##* ",
    "########*      ##*     ##*   ########* ##*   #####* ",
    "*********      **      **    ********  ***   *****  ",
    };
    
//Tess
char *game_over[6]= {
    " ######*  #####*  ###*   ###* #######*    #####*  ##*   ##* #######* ######*  ",
    "##****** ##***##* ####* ####* ##*****    ##***##* ##*   ##* ##*****  ##***##* ",
    "##* ###* #######* ##*####*##* #####*     ##***##*  ##* ##*  #####*   #######* ",
    "##* *##* ##***##* ##**##**##* ##***      ##***##*   ####*   ##***    ##*  ##* ",
    "*######* ##*  ##* ##* **  ##* #######*    #####*     ##*    #######* ##*  ##* ",
    " ******* ***  *** ***     *** ********    ******     **     ******** ***  *** "
};

//Shiqi + Tess
//Method presents either "U WIN" or "GAME OVER", then terminates the game. 
void show_message(char *message[], int rows, int start_row, int start_col, int color){
    clear(); 
                refresh();
                //no power pellets
                static int last_second = 0;
                static int toggle = 0;

                time_t now = time(NULL);
                if (now != last_second) {
                    last_second = now;
                    toggle = !toggle;    // switch color every second
                }
                start_row = 20;  // << draw at the bottom ALWAYS
                start_col = 0;         // or center later

                for (int y = 0; y < 6; y++) {
                    const char *row = message[y];

                    for (int x = 0; row[x] != '\0'; x++) {
                        char ch = row[x];
                        int scr_y = start_row + y;
                        int scr_x = start_col + x;
                        if (ch == '*') {
                            if (toggle)
                                attron(COLOR_PAIR(color));
                            else
                                attron(COLOR_PAIR(color+2));

                            mvaddch(scr_y, scr_x, POINT);
                            attroff(COLOR_PAIR(color));
                            attroff(COLOR_PAIR(color+2));
                        }
                        else{
                            attron(A_DIM);
                            mvaddch(scr_y, scr_x, ch);
                            attroff(A_DIM);
                        }
                    }
                    refresh(); 
                }
                sleep(1);
                for (int y = 0; y < 6; y++) {
                    const char *row = message[y];
                    for (int x = 0; row[x] != '\0'; x++) {
                        char ch = row[x];
                        int scr_y = start_row + y;
                        int scr_x = start_col + x;
                        if (ch == '*') {
                            if (toggle){
                                attroff(A_DIM);
                                attron(COLOR_PAIR(color));
                            }else{
                                attroff(A_DIM);
                                attron(COLOR_PAIR(color+2));
                            }
                            mvaddch(scr_y, scr_x, POINT);
                            attroff(COLOR_PAIR(color));
                            attroff(COLOR_PAIR(color+2));
                            attroff(A_DIM);
                        }
                        else{
                            attron(A_DIM);
                            mvaddch(scr_y, scr_x, ch);
                            attroff(A_DIM);
                          }
                    }
                    refresh();
                    sleep(1); 
                }
            refresh();
            getch();
            endwin();
            exit(0);            
}



//Shiqi + Tess
//Checks to see if pacman has encountered a ghost
void check_encounter(){
    for (int i = 0; i < 4; i++){
        if ((ghosts[i].x == pacman_x && ghosts[i].y == pacman_y) || (ghosts[i].prev_x == pacman_x && ghosts[i].prev_y == pacman_y) ){
            //if power pellets, blue mode
            if(ghosts[i].scared){
                ghosts[i].x = ghosts[i].start_x;
                ghosts[i].y = ghosts[i].start_y;
                ghosts[i].scared = 0; //after be eaten, scared should be 0
                ghosts[i].speed = 2 + i; //different speed
                ghosts[i].move_counter = 0;
                ghosts[i].direction = rand()%4;
                ghosts_eaten++;
                int ghost_eaten_point = 20 + 20*(ghosts_eaten-1); 
                points += ghost_eaten_point;
                
                continue;
            } else if (lives > 0){
                //only when ghosts is not scared, the lives will reduce
                if(ghosts[i].scared == 0){
                    lives--;
                }
                reset_ghost();

            } else{ 
                show_message(game_over, 6, 20, 0, 4);
            }     
        }
    }
}


//Shiqi
//draws ghosts colors
void draw_ghosts(){
    for(int i=0; i < GHOST_AMOUNT; i++){
        attron(COLOR_PAIR(ghosts[i].color_pair));
        mvaddch(ghosts[i].y, ghosts[i].x, 'G'); 
        attroff(COLOR_PAIR(ghosts[i].color_pair));
    }
}

//Tess
//spawns fruit
void fruit() {
    // spawn at multiples of 25 once (ignore zero)
    if (points > 0 && points % 50 == 0 && points != last_fruit_spawn_points) {
        int rx, ry;
        // try to find a '.' location to replace with 'B' (power pellet)
        for (int tries = 0; tries < 200; tries++) {
            rx = rand() % (COLS-2) + 1;
            ry = rand() % (ROWS-2) + 1;
            if (map[ry][rx] == POINT) {
                fruit_location[fruit_count][0] = ry;
                fruit_location[fruit_count][1] = rx;
                fruit_count++;

                map[ry][rx] = 'B';   // mark power pellet on map
                last_fruit_spawn_points = points;
                break;
            }
        }
    }
}
//Tess
//checks if pacman ate the fruit
void check_fruit(){
    for (int i = 0; i < 4; i++){
        for (int j  = 0; j < 4; j++){
            if (fruit_location[j][1] == pacman_x && fruit_location[j][0] == pacman_y) {
                points = points + 100;
            }   
        }   
    }
}


//Shiqi
//changes the ghosts to blue if in scared mode
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

//Shiqi
//checks to see if all the dots were eaten by pacman
void check_dots() {
    //traverse each location
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (map[y][x] == POINT) {
                win = 0;
                return;
            }
        }
    }
    win = 1;
}

//Shiqi + Tess
int main() {
    initscr(); //starts ncurses mode
    noecho(); // stops the terminal from printing keys the user types
    curs_set(FALSE); // hides the blinking text cursor
    keypad(stdscr, TRUE); // allows special keys (like arrow keys) to be read by getch()
    nodelay(stdscr, TRUE);  // Make getch() non-blocking, returns immediately
    start_color();//enables ncurses color mode
    use_default_colors(); //allows for transparency-style backgrounds

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
    initialize_pellets();

    while (running) {
        int ch = getch();
        
        // Handle input
        switch(ch) {
            case KEY_UP:    
                pacman_dy = -1; 
                pacman_dx = 0;
                break;
            case KEY_DOWN:  
                pacman_dy = 1;  
                pacman_dx = 0; 
                break;
            case KEY_LEFT:  
                pacman_dx = -1; 
                pacman_dy = 0; 
                break;
            case KEY_RIGHT: 
                pacman_dx = 1;  
                pacman_dy = 0; 
                break;
            case 'q':       
                running = 0;
                break;
        }
        
        
        // Move Pac-Man (now slowed by his speed counter)
        check_pellets();
        move_pacman();
        check_encounter();
        // Move ghosts (they're already slowed by their speed counters)
        for(int i = 0; i < GHOST_AMOUNT; i++){
            ghosts[i].move_counter++;
            if(ghosts[i].move_counter >= ghosts[i].speed){
                if(i == 0 || i == 1){
                    move_ghosts12(i); // follow    
                } else if(i == 2) {
                    move_ghosts3(i); //randomly   
                } else if(i == 3) {
                    move_ghosts4(i);  //follow and randomly   
                }
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
            ghosts_eaten = 0;//reset ghost points
    }

        fruit();
        check_fruit();
        check_encounter();
        blue_ghosts();
        check_dots();

         if (win) {
            show_message(game_win, 6, 10, 0, 0);
            mvprintw(30, 30, "Your Points is: %d\n", points);
        }
        
        clear();

        draw_map();  
        draw_ghosts();  
        draw_pacman_label();  
        draw_points_corner();
        refresh();
        napms(50);  // You can adjust this for overall game speed
    }
    endwin();
    return 0;
}
