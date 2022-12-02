#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>

#define SNAKE_TILE '#'
#define APPLE_TILE '@'

typedef enum {
  UP = KEY_UP,
  DOWN = KEY_DOWN,
  LEFT = KEY_LEFT,
  RIGHT = KEY_RIGHT
} Direction;

int max_x, max_y;
Direction current_dir = RIGHT;

typedef struct Tile {
  char x, y;
  struct Tile *prev, *next;
} Tile;

Tile* snake_head = NULL;
Tile* snake_tail = NULL;

struct {
  char x, y;
} apple = {0, 0};

void snake_enqueue(int y, int x) {
  Tile* tile = malloc(sizeof(Tile));
  tile->y = (y % max_y + max_y) % max_y;
  tile->x = (x % max_x + max_x) % max_x;
  tile->prev = NULL;
  tile->next = snake_head;

  if (snake_head != NULL)
    snake_head->prev = tile;
  else
    snake_tail = tile;

  snake_head = tile;
}

void snake_dequeue(void) {
  if (snake_tail != NULL) {
    Tile* tile = snake_tail;
    snake_tail = tile->prev;
    if (tile->prev != NULL)
      tile->prev->next = NULL;
    else
      snake_head = NULL;
    free(tile);
  }
}

void snake_reset(void) {
  while (snake_head != NULL) {
    Tile* next = snake_head->next;
    free(snake_head);
    snake_head = next;
  }
  snake_tail = NULL;
}

void place_apple(void) {
  apple.y = rand() % max_y;
  apple.x = rand() % max_x;
}

void init_game(void) {
  snake_enqueue(0, 0);
  place_apple();
}

void reset_game(void) {
  snake_reset();
  init_game();
  current_dir = RIGHT;
}

int main(void) {
  srand(time(NULL));
  setlocale(LC_ALL, "");

  initscr();
  noecho();
  keypad(stdscr, true);
  timeout(100);
  curs_set(0);

  getmaxyx(stdscr, max_y, max_x);

  init_game();

  for (;;) {
    bool should_deq = true;
    clear();

    mvaddch(apple.y, apple.x, APPLE_TILE);

    Tile* ptr = snake_head;
    while (ptr != NULL) {
      if ((ptr != snake_head) && (ptr->x == snake_head->x) &&
          (ptr->y == snake_head->y)) {
        reset_game();
        break;
      }
      mvaddch(ptr->y, ptr->x, SNAKE_TILE);
      ptr = ptr->next;
    }

    if (snake_head->y == apple.y && snake_head->x == apple.x) {
      place_apple();
      should_deq = false;
    }

    switch (current_dir) {
      case RIGHT:
        snake_enqueue(snake_head->y, snake_head->x + 1);
        break;
      case LEFT:
        snake_enqueue(snake_head->y, snake_head->x - 1);
        break;
      case UP:
        snake_enqueue(snake_head->y - 1, snake_head->x);
        break;
      case DOWN:
        snake_enqueue(snake_head->y + 1, snake_head->x);
        break;
    }

    if (should_deq) snake_dequeue();

    int c = getch();
    if (c == 'r')
      reset_game();
    else if (c == 'q')
      break;
    else if ((c == KEY_UP) || (c == KEY_DOWN) || (c == KEY_LEFT) ||
             (c == KEY_RIGHT))
      current_dir = c;

    refresh();
  }

  endwin();
}
