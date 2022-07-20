#define _POSIX_C_SOURCE 200201L
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define SIZE_X 28
#define SIZE_Y 31
#define DOT_COUNT 675

struct Player
{
    int x;
    int y;
    int score;
    unsigned char icon;
};

struct Map_size
{
    int x;
    int y;
};

struct Player computer;
struct Player user;
struct Map_size map_size;
const char space = ' ';
int timer = 90;
int computer_direction = KEY_LEFT;

char game_map[SIZE_Y * (SIZE_X + 1)] =
    "############################\n"
    "#.......####...............#\n"
    "#........................###\n"
    "#.#........................#\n"
    "#............###......#....#\n"
    "#..........................#\n"
    "#......##.............####.#\n"
    "#.##...##.............####.#\n"
    "#..........................#\n"
    "#......#...................#\n"
    "#......##.......#####......#\n"
    "#..........................#\n"
    "#.........#...........#....#\n"
    "##.....##.#......#....#....#\n"
    "#..........................#\n"
    "##.....##..............###.#\n"
    "#..............#...........#\n"
    "#..........................#\n"
    "#....#.............##......#\n"
    "#...##.............#.......#\n"
    "#............##............#\n"
    "#.####.......##.......#....#\n"
    "#.####.......##............#\n"
    "#..........................#\n"
    "#######....................#\n"
    "#..........................#\n"
    "#..........................#\n"
    "#............##............#\n"
    "#..........................#\n"
    "#............##............#\n"
    "############################";

char intro[] =
    "\n"
    " _______  __   __  _______    __   __  __   __  __   __  __   __  __   __    ______    _______  _______  _______  \n"
    "|       ||  | |  ||       |  |  | |  ||  | |  ||  |_|  ||  |_|  ||  | |  |  |    _ |  |   _   ||       ||       | \n"
    "|_     _||  |_|  ||    ___|  |  |_|  ||  | |  ||       ||       ||  |_|  |  |   | ||  |  |_|  ||       ||    ___| \n"
    "  |   |  |       ||   |___   |       ||  |_|  ||       ||       ||       |  |   |_||_ |       ||       ||   |___  \n"
    "  |   |  |       ||    ___|  |_     _||       ||       ||       ||_     _|  |    __  ||       ||      _||    ___| \n"
    "  |   |  |   _   ||   |___     |   |  |       || ||_|| || ||_|| |  |   |    |   |  | ||   _   ||     |_ |   |___  \n"
    "  |___|  |__| |__||_______|    |___|  |_______||_|   |_||_|   |_|  |___|    |___|  |_||__| |__||_______||_______| \n"

    ;
void firstSetup()
{
    getmaxyx(stdscr, map_size.y, map_size.x);

    computer.y = SIZE_Y - 1;
    computer.x = SIZE_X - 2;
    computer.score = 0;
    computer.icon = '$';

    user.x = 1;
    user.y = 2;
    user.score = 0;
    user.icon = '@';
}

void displayPlayers()
{
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(2, COLOR_BLACK, COLOR_CYAN);

    attron(COLOR_PAIR(1));
    mvprintw(computer.y, computer.x, &computer.icon);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(user.y, user.x, &user.icon);
    attroff(COLOR_PAIR(2));
}

int checkWalls(int playerX, int playerY, int key)
{
    switch (key)
    {
    case KEY_UP:
        if (mvinch((playerY - 1), playerX) == '#')
            return FALSE;
        break;
    case KEY_DOWN:
        if (mvinch((playerY + 1), playerX) == '#')
            return FALSE;
        break;
    case KEY_LEFT:
        if (mvinch(playerY, (playerX - 1)) == '#')
            return FALSE;
        break;
    case KEY_RIGHT:
        if (mvinch(playerY, (playerX + 1)) == '#')
            return FALSE;
        break;
    default:
        break;
    }
    return TRUE;
}

void playerMove(struct Player *player, int key)
{

    switch (key)
    {
    case KEY_UP:
        mvprintw(player->y, player->x, &space);
        player->y--;
        break;
    case KEY_DOWN:
        mvprintw(player->y, player->x, &space);
        player->y++;
        break;
    case KEY_LEFT:
        mvprintw(player->y, player->x, &space);
        player->x--;
        break;
    case KEY_RIGHT:
        mvprintw(player->y, player->x, &space);
        player->x++;
        break;
    default:
        break;
    }
    if (mvinch(player->y, player->x) == '.')
        player->score++;
}

void userMove()
{
    int key;
    if ((key = getch()) != ERR)
    {
        if (!checkWalls(user.x, user.y, key))
            return;

        playerMove(&user, key);
    }
    else
    {
        return;
    }
}

int getDirection()
{
    clock_t rndClck = clock();
    if (rndClck % 7 == 0)
        return KEY_UP;
    else if (rndClck % 9 == 0)
        return KEY_DOWN;
    else if (rndClck % 5 == 0)
        return KEY_LEFT;
    else
        return KEY_RIGHT;
}

void computerMove(long gameTimeInMs)
{

    if (!checkWalls(computer.x, computer.y, computer_direction))
        computer_direction = getDirection();
    else
    {
        if (gameTimeInMs % 300 == 0)

            playerMove(&computer, computer_direction);
    }
}

void gameOver()
{
    erase();
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_CYAN);
    if (computer.score < user.score)
    {
        mvprintw(0, 0, "The time is up! Congratulations, you've won with %d points!", user.score);
    }
    else
    {
        mvprintw(0, 0, "The time is up! Sorry, you've lost by %d points.", computer.score - user.score);
    }

    attron(A_STANDOUT);
    mvprintw(3, 0, "Press e to exit.");
    attroff(A_STANDOUT);
}

void game()
{
    clock_t end = clock() + (timer * CLOCKS_PER_SEC);
    int timeLeft;

    mvprintw(1, 0, game_map);
    displayPlayers();
    if (map_size.y > 40 && map_size.x > 118)
    {
        start_color();
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);

        attron(COLOR_PAIR(3));
        mvprintw(map_size.y - 10, 5, intro);
        attroff(COLOR_PAIR(3));
    }
    refresh();

    while (1)
    {
        timeLeft = ((end - clock()) / CLOCKS_PER_SEC);
        if (computer.score + user.score == DOT_COUNT)
        {
            break;
        }
        if (timeLeft <= 0)
        {
            break;
        }

        attron(COLOR_PAIR(3));
        mvprintw(0, 0, "User-score: %d    Computer-score: %d    Time-left: %d", user.score, computer.score, timeLeft);
        attroff(COLOR_PAIR(3));

        userMove();
        computerMove(((end - clock()) / (CLOCKS_PER_SEC / 1000000)));
        displayPlayers();

        refresh();
    }

    char escape;
    while (escape = getch() != 'e')
    {
        gameOver();
    }
}

int main(int argc, char **argv)
{
    WINDOW *w = initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    nodelay(w, TRUE);

    if (argc == 2)
    {
        timer = atoi(argv[1]);
        if (timer <= 0)
        {
            printf("You've enter a wrong parameter. Please, try again. ");
            endwin();
            return 0;
        }
    }

    firstSetup();

    if (map_size.y < SIZE_Y - 2 || map_size.x < 2 * SIZE_X)
    {
        printf("Please, resize your window and try again.");
        return 0;
    }

    game();

    endwin();
    return 0;
}
