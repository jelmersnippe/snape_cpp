#include "raylib.h"

#include <cstdio>
#include <iterator>
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
const float tickTime = 1.0 / ticksPerSecond;
const int screenWidth = 400;
const int screenHeight = 400;
const int blockSize = 20;
double timeSinceLastTick = 0;
Direction desiredDirection = Up;

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

Snake snake = CreateSnake(3);

void DrawSnake() {
  for (const Segment &segment : snake.segments) {
    DrawRectangle(segment.position.x, segment.position.y, blockSize, blockSize,
                  GREEN);
  }
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
  }
}

bool isSameAxis(const Direction &direction, const Direction &otherDirection) {
  return ((direction == Up || direction == Down) &&
          (otherDirection == Up || otherDirection == Down)) ||
         ((direction == Left || direction == Right) &&
          (otherDirection == Left || otherDirection == Right));
}

Snake UpdateSnake(const Snake &snake, Direction desiredDirection) {
  Snake newSnake;
  Direction decidedDirection = isSameAxis(snake.direction, desiredDirection)
                                   ? snake.direction
                                   : desiredDirection;

  const Vector2 direction = GetDirection(decidedDirection);
  newSnake.direction = decidedDirection;

  std::list<Segment>::const_iterator iterator = snake.segments.cbegin();
  const Segment head = *iterator;

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

  Segment newHead;
  newHead.position = newPosition;

  newSnake.segments = snake.segments;
  newSnake.segments.push_front(newHead);
  newSnake.segments.pop_back();

  return newSnake;
}

void Update() {
  timeSinceLastTick += GetFrameTime();

  if (timeSinceLastTick >= tickTime) {
    snake = UpdateSnake(snake, desiredDirection);
    timeSinceLastTick -= tickTime;
  }
}

void HandleInput(void) {
  if (IsKeyPressed(KEY_UP)) {
    desiredDirection = Up;
  } else if (IsKeyPressed(KEY_DOWN)) {
    desiredDirection = Down;
  } else if (IsKeyPressed(KEY_RIGHT)) {
    desiredDirection = Right;
  } else if (IsKeyPressed(KEY_LEFT)) {
    desiredDirection = Left;
  }
}

void Draw() {
  BeginDrawing();

  ClearBackground(LIGHTGRAY);

  DrawSnake();

  EndDrawing();
}

int main(void) {
  InitWindow(screenWidth, screenHeight, "Snek");

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    HandleInput();

    Update();

    Draw();
  }

  CloseWindow();

  return 0;
}
