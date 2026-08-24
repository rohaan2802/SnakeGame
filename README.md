# SnakeGame (GameBoy hub)

Despite the repo name, this is an SFML **GameBoy-style hub** that launches **Snake**, **Hangman**, and **Wordle** from one executable.

**Source:** `i222327_SnakeGame.cpp` · Student id **i222327** · [rohaan2802](https://github.com/rohaan2802)

---

## Table of contents

1. [Hub architecture](#hub-architecture)
2. [Snake](#snake)
3. [Hangman](#hangman)
4. [Wordle](#wordle)
5. [Build and assets](#build-and-assets)
6. [Play](#play)

---

## Hub architecture

Abstract `Game`: `startGame`, `endGame`, `render`, `goBack`.

| Class | Role |
|-------|------|
| `GameBoy` | Main window loop, menu hit-boxes, owns current `Game*` |
| `Screen` | **960×640** |
| `Grid` | Snake playfield helper |
| `Menu` | Play / Instructions / Quit |
| `WordManager` / `DisplayManager` | Hangman/Wordle words and drawing |
| `SnakeGame` `HangmanGame` `WordleGame` | Concrete games |

Startup + per-game **WAV**: `game.wav`, `snake.wav`. Mouse-driven menus.

**Instructions** overlay describes all three games. **Quit** closes the window. `goBack()` returns to the hub.

---

## Snake

- Grow-on-food, sprite segments (`Images/snake.png`, `food.png`, `grid.png`)  
- **Three difficulties** via `snakeLevels.png` click regions (`speed = 10` etc. in the level branch)  
- Score HUD  
- Pause: Resume / Restart  
- Game-over flow  
- Direction keys handled inside `SnakeGame`

---

## Hangman

Word guess + hangman stages. `WordManager` supplies the secret; `DisplayManager` draws gallows / letters. Keyboard letter input as implemented in `HangmanGame`.

---

## Wordle

Fixed-length guess grid, color feedback (exact/present/absent) as implemented in `WordleGame`. Word list lives with `WordManager`.

---

## Build and assets

C++17 + SFML Graphics/Window/System/Audio.

```bash
g++ -std=c++17 i222327_SnakeGame.cpp -o SnakeGame \
  -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
./SnakeGame
```

Working directory must see:

```text
Images/grid.png, snake.png, food.png, snakeLevels.png, arial.ttf,
Images/buttons.png, background.jpg, …
game.wav, snake.wav
```

(`loadFromFile` paths are relative.)

---

## Play

1. Start sound → main menu.  
2. **Play** → click Hangman / Snake / Wordle regions.  
3. Snake: pick Level 1/2/3, then play; pause for Resume/Restart.  
4. Hangman/Wordle: on-screen prompts.  
5. Instructions / back / Quit.

**Extend:** fourth `Game` subclass + hit-box in `GameBoy::start`; expand word lists; retune Snake speeds.

---

## Author

**rohaan2802** (i222327) · [https://github.com/rohaan2802](https://github.com/rohaan2802)
