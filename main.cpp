#include "raylib.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <list>

enum Direction { Up, Right, Down, Left };

struct Segment {
  Vector2 position;
};

struct Snake {
  Direction direction;
  std::list<Segment> segments;
};

const int TICKS_PER_SECOND = 4;
const float TICK_TIME = 1.0 / TICKS_PER_SECOND;
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;
const int BLOCK_SIZE = 20;
const int BLOCKS_IN_WIDTH = SCREEN_WIDTH / BLOCK_SIZE;
const int BLOCKS_IN_HEIGHT = SCREEN_HEIGHT / BLOCK_SIZE;

struct GameState {
  double timeSinceLastTick = 0;
  Direction desiredDirection = Up;
  Snake snake;
  Vector2 applePosition;
};

Snake CreateSnake(int length) {
  Snake snake;
  snake.direction = Up;
  snake.segments = std::list<Segment>();

  for (int i = 0; i < 3; i++) {
    Segment segment;
    const int offset = i * BLOCK_SIZE;
    const int x = SCREEN_WIDTH / 2.0;
    const int y = (SCREEN_HEIGHT / 2.0) + offset;
    segment.position = {x, static_cast<float>(y)};
    snake.segments.push_back(segment);
  }

  return snake;
}

void DrawSnake(const Snake &snake) {
  for (const Segment &segment : snake.segments) {
    DrawRectangle(segment.position.x, segment.position.y, BLOCK_SIZE,
                  BLOCK_SIZE, GREEN);
  }
}

void DrawApple(const Vector2 &position) {
  DrawRectangle(position.x, position.y, BLOCK_SIZE, BLOCK_SIZE, RED);
}

Vector2 GetApplePosition(GameState &state) {
  int x = std::rand() % BLOCKS_IN_WIDTH;
  int y = std::rand() % BLOCKS_IN_HEIGHT;

  while (true) {
    if (!std::any_of(state.snake.segments.begin(), state.snake.segments.end(),
                     [&](const Segment &s) {
                       return s.position.x == x && s.position.y == y;
                     })) {
      break;
    }

    x = std::rand() % BLOCKS_IN_WIDTH;
    y = std::rand() % BLOCKS_IN_HEIGHT;
  }

  printf("GetApplePosition %d, %d", x, y);
  return Vector2{static_cast<float>(x * BLOCK_SIZE),
                 static_cast<float>(y * BLOCK_SIZE)};
}

Vector2 GetDirection(const Direction &direction) {
  switch (direction) {
  case Up:
    return {0, -1};
  case Down:
    return {0, 1};
  case Right:
    return {1, 0};
  case Left:
    return {-1, 0};
  default:
    return {0, 0};
  }
}

bool isSameAxis(const Direction &direction, const Direction &otherDirection) {
  return ((direction == Up || direction == Down) &&
          (otherDirection == Up || otherDirection == Down)) ||
         ((direction == Left || direction == Right) &&
          (otherDirection == Left || otherDirection == Right));
}

Snake UpdateSnake(GameState &state) {
  Snake newSnake;
  Direction decidedDirection =
      isSameAxis(state.snake.direction, state.desiredDirection)
          ? state.snake.direction
          : state.desiredDirection;

  const Vector2 direction = GetDirection(decidedDirection);
  newSnake.direction = decidedDirection;

  std::list<Segment>::const_iterator iterator = state.snake.segments.cbegin();
  const Segment head = *iterator;

  const Vector2 delta = {BLOCK_SIZE * direction.x, BLOCK_SIZE * direction.y};

  Vector2 newPosition = {head.position.x + delta.x, head.position.y + delta.y};

  // Screen wrapping x
  if (newPosition.x < 0) {
    newPosition.x = SCREEN_WIDTH - BLOCK_SIZE;
  } else if (newPosition.x >= SCREEN_WIDTH) {
    newPosition.x = 0;
  }

  // Screen wrapping y
  if (newPosition.y < 0) {
    newPosition.y = SCREEN_HEIGHT - BLOCK_SIZE;
  } else if (newPosition.y >= SCREEN_HEIGHT) {
    newPosition.y = 0;
  }

  Segment newHead;
  newHead.position = newPosition;

  newSnake.segments = state.snake.segments;
  newSnake.segments.push_front(newHead);

  if (std::any_of(newSnake.segments.begin(), newSnake.segments.end(),
                  [&](const Segment &s) {
                    return s.position.x == state.applePosition.x &&
                           s.position.y == state.applePosition.y;
                  })) {
    state.applePosition = GetApplePosition(state);
  } else {
    newSnake.segments.pop_back();
  }

  return newSnake;
}

void Update(GameState &state) {
  state.timeSinceLastTick += GetFrameTime();

  if (state.timeSinceLastTick >= TICK_TIME) {
    state.snake = UpdateSnake(state);

    state.timeSinceLastTick -= TICK_TIME;
  }
}

void HandleInput(GameState &state) {
  if (IsKeyPressed(KEY_UP)) {
    state.desiredDirection = Up;
  } else if (IsKeyPressed(KEY_DOWN)) {
    state.desiredDirection = Down;
  } else if (IsKeyPressed(KEY_RIGHT)) {
    state.desiredDirection = Right;
  } else if (IsKeyPressed(KEY_LEFT)) {
    state.desiredDirection = Left;
  }
}

void Draw(const GameState &state) {
  BeginDrawing();

  ClearBackground(LIGHTGRAY);

  DrawApple(state.applePosition);
  DrawSnake(state.snake);

  EndDrawing();
}

int main(void) {
  GameState state;
  state.snake = CreateSnake(3);
  state.desiredDirection = Up;
  state.timeSinceLastTick = 0;
  Vector2 initialApplePosition = GetApplePosition(state);
  printf("Initial apple position: %f, %f", initialApplePosition.x,
         initialApplePosition.y);
  state.applePosition = initialApplePosition;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snek");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    HandleInput(state);

    Update(state);

    Draw(state);
  }

  CloseWindow();

  return 0;
}
