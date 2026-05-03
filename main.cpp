#include "raylib.h"

#include <algorithm>
#include <ctime>
#include <deque>

enum Direction {
    Up,
    Right,
    Down,
    Left
};

struct GridPoint {
    int x, y;
};

struct Segment {
    GridPoint position;
};

struct Snake {
    Direction direction;
    std::deque<Segment> segments;
};

const int TICKS_PER_SECOND = 4;
const float TICK_TIME = 1.0 / TICKS_PER_SECOND;
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;
const int TILE_SIZE = 20;
const int TILES_IN_WIDTH = SCREEN_WIDTH / TILE_SIZE;
const int TILES_IN_HEIGHT = SCREEN_HEIGHT / TILE_SIZE;

struct GameState {
    double timeSinceLastTick = 0;
    Direction desiredDirection = Up;
    Snake snake;
    GridPoint applePosition;
    bool gameOver;
};

Snake CreateSnake(int length) {
    Snake snake;
    snake.direction = Up;
    snake.segments = std::deque<Segment>();

    for (int i = 0; i < length; i++) {
        Segment segment;
        const int x = TILES_IN_WIDTH / 2;
        const int y = (TILES_IN_HEIGHT / 2) + i;
        segment.position.x = x;
        segment.position.y = y;
        snake.segments.push_back(segment);
    }

    return snake;
}

void DrawSnake(const Snake& snake) {
    for (const Segment& segment : snake.segments) {
        DrawRectangle(segment.position.x * TILE_SIZE, segment.position.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
    }
}

void DrawApple(const GridPoint& position) {
    DrawRectangle(position.x * TILE_SIZE, position.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
}

GridPoint GetApplePosition(GameState& state) {
    int x = std::rand() % TILES_IN_WIDTH;
    int y = std::rand() % TILES_IN_HEIGHT;

    while (true) {
        if (!std::any_of(state.snake.segments.begin(), state.snake.segments.end(),
                         [&](const Segment& s) { return s.position.x == x && s.position.y == y; })) {
            break;
        }

        x = std::rand() % TILES_IN_WIDTH;
        y = std::rand() % TILES_IN_HEIGHT;
    }

    GridPoint point;
    point.x = x;
    point.y = y;
    return point;
}

GridPoint GetDirection(const Direction& direction) {
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

bool isSameAxis(const Direction& direction, const Direction& otherDirection) {
    return ((direction == Up || direction == Down) && (otherDirection == Up || otherDirection == Down)) ||
           ((direction == Left || direction == Right) && (otherDirection == Left || otherDirection == Right));
}

void UpdateSnake(GameState& state) {
    Direction decidedDirection =
        isSameAxis(state.snake.direction, state.desiredDirection) ? state.snake.direction : state.desiredDirection;

    const GridPoint direction = GetDirection(decidedDirection);
    state.snake.direction = decidedDirection;

    const Segment& head = state.snake.segments.front();

    GridPoint newPosition = {head.position.x + direction.x, head.position.y + direction.y};

    // Screen wrapping x
    if (newPosition.x < 0) {
        newPosition.x = TILES_IN_WIDTH - 1;
    } else if (newPosition.x >= TILES_IN_WIDTH) {
        newPosition.x = 0;
    }

    // Screen wrapping y
    if (newPosition.y < 0) {
        newPosition.y = TILES_IN_HEIGHT - 1;
    } else if (newPosition.y >= TILES_IN_HEIGHT) {
        newPosition.y = 0;
    }

    Segment newHead;
    newHead.position.x = newPosition.x;
    newHead.position.y = newPosition.y;

    state.snake.segments.push_front(newHead);

    if (newPosition.x == state.applePosition.x && newPosition.y == state.applePosition.y) {
        state.applePosition = GetApplePosition(state);
    } else {
        state.snake.segments.pop_back();
    }

    for (int i = 1; i < state.snake.segments.size(); i++) {
        const Segment& s = state.snake.segments[i];

        if (s.position.x == newPosition.x && s.position.y == newPosition.y) {
            state.gameOver = true;
            break;
        }
    }
}

void Update(GameState& state) {
    state.timeSinceLastTick += GetFrameTime();

    if (state.timeSinceLastTick >= TICK_TIME) {
        UpdateSnake(state);

        state.timeSinceLastTick -= TICK_TIME;
    }
}

void HandleInput(GameState& state) {
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

void Draw(const GameState& state) {
    BeginDrawing();

    ClearBackground(LIGHTGRAY);

    DrawApple(state.applePosition);
    DrawSnake(state.snake);

    EndDrawing();
}

int main(void) {
    std::srand(static_cast<unsigned>(time(nullptr)));

    GameState state;
    state.snake = CreateSnake(3);
    state.desiredDirection = Up;
    state.timeSinceLastTick = 0;
    state.gameOver = false;
    GridPoint initialApplePosition = GetApplePosition(state);
    state.applePosition = initialApplePosition;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snek");

    SetTargetFPS(60);

    while (!WindowShouldClose() && !state.gameOver) {
        HandleInput(state);

        Update(state);

        Draw(state);
    }

    CloseWindow();

    return 0;
}
