# SnakeGame

An SFML **GameBoy-style hub** (`GameBoy`) that launches three mini-games from one executable: **Snake**, **Hangman**, and **Wordle**. Primary source: `i222327_SnakeGame.cpp`.

## Overview

Despite the repo name, this project is a multi-game console UI:

- Abstract `Game` interface (`startGame`, `endGame`, `render`, `goBack`)
- Concrete games: `SnakeGame`, `HangmanGame`, `WordleGame`
- Shared helpers: `Screen` (960×640), `Grid`, `Menu`, `WordManager`, `DisplayManager`
- Entry point creates `GameBoy` and runs the main SFML window loop

Assets are loaded from `Images/` (grid, snake, food, backgrounds, fonts) and WAV files (`game.wav`, `snake.wav`).

## Features

### Hub (GameBoy)
- Mouse-driven main menu: Play, Instructions, Quit
- Game select screen for Hangman / Snake / Wordle
- Instructions overlay describing all three games
- Startup and per-game sound effects

### Snake
- Classic grow-on-food snake with sprite segments
- Three difficulty levels (different speeds) via clickable level UI (`snakeLevels.png`)
- Score display, pause menu (Resume / Restart), game-over flow
- Grid background from `Images/grid.png`

### Hangman
- Word guessing with hangman visuals managed by `HangmanGame` + `WordManager` / `DisplayManager`

### Wordle
- Word-guessing puzzle flow implemented in `WordleGame`

## Tech stack

| Component | Technology |
|-----------|------------|
| Language | C++ |
| Graphics / audio | SFML (`Graphics`, `Audio`) |
| Resolution | 960×640 (`Screen`) |

## Project structure

```
SnakeGame/
└── i222327_SnakeGame.cpp   # All classes + main()
```

Expected runtime assets (referenced in code; supply beside the binary):

```
Images/grid.png, snake.png, food.png, snakeLevels.png, arial.ttf,
Images/buttons.png, background.jpg, …
game.wav, snake.wav
```

## How to build / run

### Requirements

- C++ compiler with SFML 2.x

### Example (g++)

```bash
g++ -std=c++17 i222327_SnakeGame.cpp -o SnakeGame \
  -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
./SnakeGame
```

Ensure `Images/` and audio files are on the working-directory search path used by `loadFromFile`.

## Usage

1. Run the app — dramatic start sound plays; main menu buttons appear.
2. Click **Play** → choose Hangman, Snake, or Wordle by clicking the corresponding region.
3. **Snake:** pick Level 1/2/3, then control the snake (direction keys as handled in `SnakeGame`); pause for Resume/Restart; grow by eating food.
4. **Hangman / Wordle:** follow on-screen prompts for letter/word input.
5. Use each game’s back/menu path (`goBack()`) to return to the hub.
6. Instructions button shows the in-app help text; Quit closes the window.

## How to extend / modify

- Add a fourth game by subclassing `Game` and wiring a new hit-box + `new YourGame` in `GameBoy::start`.
- Tune Snake speeds in the level-selection branch (`speed = 10`, etc.).
- Replace assets under `Images/` without changing code if filenames stay the same.
- Expand word lists inside `WordManager` for Hangman/Wordle.

## Author

**rohaan2802** (student id referenced in filename: i222327) — [https://github.com/rohaan2802](https://github.com/rohaan2802)
