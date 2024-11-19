#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>
#include <queue>
using namespace std;

/* CONFIG START */

// set to EASY, NORMAL, or HARD (replace word after difficulty)
#define DIFFICULTY NORMAL

// set to 0 or 1 (0 = OFF, 1 = ON)
#define WALL_HACKS 0

// player's speed (must be a float, end the number with f)
// 5.0f is normal speed
#define SPEED 5.0f

/* CONFIG END */

// MAZE_W and MAZE_H must be odd and at least 3
#define MAZE_W DIFFICULTY.first
#define MAZE_H DIFFICULTY.second

#define EASY make_pair(11, 11)
#define NORMAL make_pair(21, 21)
#define HARD make_pair(31, 31)

#define WALL_SIZE 32

const int SCREEN_WIDTH = MAZE_W * WALL_SIZE;
const int SCREEN_HEIGHT = MAZE_H * WALL_SIZE;

// 0 = blocked
// 1 = free
bool maze[MAZE_H][MAZE_W];

random_device rd;
mt19937 mt_rand(rd());
// fixed seed
// mt19937 mt_rand(342);

struct Wall {
    int x, y, x2, y2;
};

int Exit, Entrance;

void GenerateMaze() {
    // reset the maze
    memset(maze, 0, sizeof(maze));

    vector<Wall> walls;
    int x = 1, y = 1;
    walls.push_back({x, y, x, y});

    while(!walls.empty()) {
        int i = mt_rand() % walls.size();
        auto wall = walls[i];
        walls.erase(walls.begin()+i);

        int x = wall.x2, y = wall.y2;
        if(maze[y][x]) continue;
        
        maze[y][x] = maze[wall.y][wall.x] = 1;
        if(x >= 3 && !maze[y][x-2])
            walls.push_back({x-1, y, x-2, y});
        if(y >= 3 && !maze[y-2][x])
            walls.push_back({x, y-1, x, y-2});
        if(x < MAZE_W-3 && !maze[y][x+2])
            walls.push_back({x+1, y, x+2, y});
        if(y < MAZE_H-3 && !maze[y+2][x])
            walls.push_back({x, y+1, x, y+2});
    }

    vector<int> Exits, Entrances;
    for(int i = 1; i < MAZE_W-1; i++) {
        if(maze[1][i]) Exits.push_back(i);
        if(maze[MAZE_H-2][i]) Entrances.push_back(i);
    }

    Exit = Exits[mt_rand()%Exits.size()];
    Entrance = Entrances[mt_rand()%Entrances.size()];

    maze[0][Exit] = 1;
    maze[MAZE_H-1][Entrance] = 1;
}

vector<pair<int,int>> dirs{{1,0},{-1,0},{0,1},{0,-1}};
deque<pair<int,int>> moves;
bool visited[MAZE_H][MAZE_W];

// returns -1 if x is negative, 0 if x is 0, 1 if x is positive
int sign(int x) {
    return (x > 0) - (x < 0);
}

bool SolveMaze(int y, int x) {
    visited[y][x] = 1;
    if(y == 0 && x == Exit) {
        return true;
    }

    if(abs(y-0) + abs(x-Exit) == 1) {
        moves.push_back({sign(0-y), sign(Exit-x)});
        return true;
    }

    auto dirs_c = dirs;
    shuffle(dirs_c.begin(), dirs_c.end(), mt_rand);

    for(auto& dir : dirs_c) {
        int ny = y + dir.first;
        int nx = x + dir.second;
        if(ny < 0 || nx < 0 || ny >= MAZE_H || nx >= MAZE_W)
            continue;
        if(!maze[ny][nx] || visited[ny][nx])
            continue;

        moves.push_back(dir);
        if(SolveMaze(ny, nx)) {
            return true;
        }
        if(!WALL_HACKS)
            moves.push_back({-dir.first, -dir.second});
        else
            moves.pop_back();
    }

    return false;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    window = SDL_CreateWindow("Maze", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GenerateMaze();
    SolveMaze(MAZE_H-1, Entrance);

    SDL_Event e;
    bool quit = false;

    float playerX = Entrance * WALL_SIZE, playerY = (MAZE_H-1) * WALL_SIZE;
    pair<int,int> playerDir;
    float move = 0;

    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) quit = true;
        }

        if(move > 0) {
            playerY += min(move, SPEED) * playerDir.first;
            playerX += min(move, SPEED) * playerDir.second;
            move -= SPEED;
        } else if(!moves.empty()) {
            playerDir = moves.front();
            moves.pop_front();
            move = WALL_SIZE;
        } else {
            // reset
            GenerateMaze();
            playerX = Entrance * WALL_SIZE, playerY = (MAZE_H-1) * WALL_SIZE;
            memset(visited, 0, sizeof(visited));
            SolveMaze(MAZE_H-1, Entrance);
            SDL_Delay(1000);
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0XFF, 0xFF);
        SDL_RenderFillRect(renderer, NULL);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        for(int i = 0; i < MAZE_H; i++) {
            for(int j = 0; j < MAZE_W; j++) {
                if(maze[i][j]) continue;
                SDL_Rect rect{j*WALL_SIZE, i*WALL_SIZE, WALL_SIZE, WALL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
        SDL_FRect player{playerX, playerY, WALL_SIZE, WALL_SIZE};
        SDL_RenderFillRectF(renderer, &player);
        
        SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0xFF);
        SDL_Rect finishLine{Exit * WALL_SIZE, 0, WALL_SIZE, WALL_SIZE/4};
        SDL_RenderFillRect(renderer, &finishLine);

        SDL_RenderPresent(renderer);
        SDL_Delay(33);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
