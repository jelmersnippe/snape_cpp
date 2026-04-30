#include "raylib.h"

#include <cstdio>
#include <list>

enum Direction { Up, Right, Down, Left };

struct Segment {
  Vector2 position;
};

struct Snake {
  Direction direction;
  std::list<Segment> segments;
};

const int ticksPerSecond = 4;
const int screenWidth = 400;
const int screenHeight = 400;
const int blockSize = 20;
double lastUpdate = GetTime();
Direction requestDirection = Up;

void DrawSnake(const Snake &snake) {
  for (const Segment &segment : snake.segments) {
    DrawRectangle(segment.position.x, segment.position.y, blockSize, blockSize,
                  GREEN);
  }
}

Snake CreateSnake(int length) {
  Snake snake;
  snake.direction = Up;
  snake.segments = std::list<Segment>();

  for (int i = 0; i < 3; i++) {
    Segment segment;
    const float offset = i * blockSize;
    const float x = screenWidth / 2.0;
    const float y = (screenHeight / 2.0) + offset;
    segment.position = {x, y};
    snake.segments.push_back(segment);
  }

  return snake;
}

Vector2 GetDirection(Direction &direction) {
  switch (direction) {
  case Up:
    return {0, -1};
  case Down:
    return {0, 1};
  case Right:
    return {1, 0};
  case Left:
    return {-1, 0};
  }
}

void UpdateSnake(Snake &snake, Direction requestDirection) {
  snake.direction = requestDirection;
  std::list<Segment>::const_iterator iterator = snake.segments.cbegin();
  const Segment head = *iterator;

  snake.segments.pop_back();

  const Vector2 direction = GetDirection(snake.direction);

  const Vector2 delta = {blockSize * direction.x, blockSize * direction.y};

  Vector2 newPosition = {head.position.x + delta.x, head.position.y + delta.y};

  // Screen wrapping x
  if (newPosition.x < 0) {
    newPosition.x = screenWidth - blockSize;
  } else if (newPosition.x >= screenWidth) {
    newPosition.x = 0;
  }

  // Screen wrapping y
  if (newPosition.y < 0) {
    newPosition.y = screenHeight - blockSize;
  } else if (newPosition.y >= screenHeight) {
    newPosition.y = 0;
  }

  Segment segment;
  segment.position = newPosition;

  snake.segments.push_front(segment);
}

void Update(Snake &snake, Direction requestDirection) {
  const double now = GetTime();
  if ((now - lastUpdate) > 1.0 / ticksPerSecond) {
    printf("Updating because %f has passed\n", now - lastUpdate);

    UpdateSnake(snake, requestDirection);

    lastUpdate = now;
  }
}

void HandleInput(void) {
  if (IsKeyPressed(KEY_UP) && requestDirection != Up &&
      requestDirection != Down) {
    requestDirection = Up;
  } else if (IsKeyPressed(KEY_DOWN) && requestDirection != Up &&
             requestDirection != Down) {
    requestDirection = Down;
  } else if (IsKeyPressed(KEY_RIGHT) && requestDirection != Left &&
             requestDirection != Right) {
    requestDirection = Right;
  } else if (IsKeyPressed(KEY_LEFT) && requestDirection != Left &&
             requestDirection != Right) {
    requestDirection = Left;
  }
}

void Draw(const Snake &snake) {
  BeginDrawing();

  ClearBackground(LIGHTGRAY);

  DrawSnake(snake);

  EndDrawing();
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "basic window");

  SetTargetFPS(60);

  Snake snake = CreateSnake(3);

  while (!WindowShouldClose()) {
    HandleInput();

    Update(snake, requestDirection);

    Draw(snake);
  }

  CloseWindow();

  return 0;
}
