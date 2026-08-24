# SnakeGame (GameBoy hub)

Despite the repository name, this is **not** a `windows.h` / `conio.h` console snake. The only source file, `i222327_SnakeGame.cpp`, is an **SFML 2.x GameBoy-style hub** that launches **Snake**, **Hangman**, and **Wordle** from one `RenderWindow`. Student id **i222327** appears in the filename.

**Course / author:** OOP-style C++ game assignment · Mohammad Rohaan · roll **22I-2327**  
**Source:** `i222327_SnakeGame.cpp`  
[https://github.com/rohaan2802](https://github.com/rohaan2802)

---

## Table of contents

1. [What it is](#what-it-is)
2. [Architecture](#architecture)
3. [File-by-file](#file-by-file)
4. [Hub (`GameBoy` / `Menu`)](#hub-gameboy--menu)
5. [Snake](#snake)
6. [Hangman](#hangman)
7. [Wordle](#wordle)
8. [I/O and assets](#io-and-assets)
9. [Build and run](#build-and-run)
10. [Known limitations](#known-limitations)
11. [Author](#author)

---

## What it is

`main` constructs `GameBoy G` and calls `G.start()`. That opens `VideoMode(960, 640)`, title `"GameBoy"`, styles `Close | Titlebar`, window position `(500, 200)`. Startup plays `game.wav`. Mouse hit-boxes choose Play / Instructions / Quit, then Hangman / Snake / Wordle. Each minigame is a heap-allocated `Game*` (`SnakeGame`, `HangmanGame`, `WordleGame`). Includes are `<iostream>`, `<cstdlib>`, `<time.h>`, `<SFML/Graphics.hpp>`, `<SFML/Audio.hpp>`. Namespaces: `std`, `sf`.

---

## Architecture

Abstract `Game` requires `startGame`, `endGame`, `render`, `goBack`. Shared types: `Player` (only `string name`), `Screen` (fixed **960×640**), `Grid` (Snake background sprite).

| Class | Role |
|-------|------|
| `Screen` | `resolutionX = 960`, `resolutionY = 640`; getters |
| `Game` | `Player* P`, `int score`, `Screen* S`, fonts/texts; four pure virtuals |
| `Grid` | Loads `Images/grid.png`, scales to the screen, `window.draw` |
| `SnakeSegment` | `float x,y`; `setPosition` / `getX` / `getY` |
| `SnakeGame` | Levels, WASD move, food, collisions, pause, score |
| `WordManager` | Secret word, `guessed[20]`, hint reveal |
| `DisplayManager` | `drawText`, gallows rectangles/circle |
| `HangmanGame` | Categories, letter input, 6 misses, hint button |
| `WordleGame` | 5×5 grid, 10-word list, on-screen QWERTY |
| `Menu` | `inMenu`; `navigate('P'/'Q'/'I')` |
| `GameBoy` | Main loop, textures, sounds, owns `currentGame` |

`SnakeGame::foodTimer` is a **static** `int`, initialized after the class (`int SnakeGame::foodTimer = 0`).

---

## File-by-file

| Path | Role |
|------|------|
| `i222327_SnakeGame.cpp` | Entire program (~1350 lines, one TU) |

Assets are **not** in the Git tree listing (`TREE.txt` is only the `.cpp`). The code still `loadFromFile` relative paths under `Images/` plus WAV files in the working directory. Missing files print `cerr` / `cout` and some paths `exit(EXIT_FAILURE)`.

---

## Hub (`GameBoy` / `Menu`)

Constructor fills a pink `groundRectangle` `Color(254, 149, 146, 255)` and loads `Images/arial.ttf` for instructions. Instructions string titles the hub **GameBoy** and lists Hangman, Snake, and Wordle; it says “Click anywhere to return to the main menu” (return-from-instructions is **not** implemented as a click handler in `start()`).

**Main-menu mouse rectangles** (`FloatRect`):

| Hit-box | Approx. role |
|---------|----------------|
| `(360, 60, 250, 60)` | Play → `option = 'P'` |
| `(360, 165, 250, 60)` | Instructions → `'I'` (also `cout << "Hello"`) |
| `(360, 500, 250, 70)` | Quit → `'Q'` |

**Play screen** (after Play): `Images/background.jpg` scaled to 960×640.

| Hit-box | Game | Extra |
|---------|------|--------|
| `(10, 120, 270, 320)` | `HangmanGame` | Stops menu sound |
| `(350, 120, 270, 320)` | `SnakeGame` | Loads/plays `snake.wav` |
| `(680, 120, 270, 320)` | `WordleGame` | — |

Sprites: `Images/buttons.png` at `(340, 30)` while `M.INMENU()`. Quit closes the window. `currentGame->goBack()` true restores `play = true` (game-select screen), not the first menu.

`Menu::navigate` returns `true` for P/Q/I and sets `inMenu = 0`. There is **no `return` on other characters** (undefined return if called that way). Hub only calls it with those three.

---

## Snake

Constructor: `snakeSize = 3`, `direction = 'D'`, `speed = 0` until a level is clicked. Allocates `SnakeSegment[3]` and a pause panel at `(280, 130)` size `400×300`, fill `Color(43, 51, 28)`.

**Level select** (`Images/snakeLevels.png` at `(200, 100)`):

| Bounds | `speed` | Meaning in `moveSnake` |
|--------|---------|-------------------------|
| Level1 `(270, 400, 100, 35)` | **10** | Move when `moveCounter >= speed` — **slower** |
| Level2 `(410, 400, 120, 35)` | **5** | Medium |
| Level3 `(550, 400, 140, 35)` | **3** | **Faster** |

**Controls (held keys):** `D` right, `A` left, `W` up, `S` down; opposite direction blocked (`D` ignored if `direction == 'A'`, etc.). **`P`** sets `isPaused = 1`. Pause UI: texts `"Resume"` `(430, 220)` and `"Restart"` `(430, 340)`. Restart resets snake flags, `snakeSize = 3`, `direction = 'D'`, `foodTimer = 0` (does not reallocate `SS` if the snake had grown).

**Movement:** head steps **±30** pixels. Body follows previous positions. `generateSnake` places the head in `[100, res−100)` random and stacks segments 30 px left.

**Food:** `Images/food.png`, scale ~45×40. `generateFood` retries until AABB vs 31×31 segments is clear. `detectFoodCollision`: head within ±30 of food sprite. Eat: `isFruit = false`, `incrementSnake()`, **`score += 10`**. HUD `"Score: "` top-right, white bold size 40. `foodTimer` increments every frame; at `>= 100` set to `-5` (respawn cadence).

**Collisions:**

- `detectWallCollision`: head `x < 0` or `>= 960` or `y < 0` or `>= 640`
- `detectSnakeCollision`: body from index **`i = 4`** (first four segments ignored), 20×20 AABB

Game over: `"GAME OVER!"` size 150; **left click** sets `back = 1` (`goBack()`).

Snake sprites: `Images/snake.png`; head scale 30, body 31.

---

## Hangman

`startGame` shows `Images/load1.png` for `sleep(seconds(2))` once. Then a **blocking** category loop: keys **1 / 2 / 3** pick arrays:

| Key | Array | Size (as coded) |
|-----|--------|------------------|
| 1 | `fruits[]` | 51 string literals |
| 2 | `vegetables[]` | 50 |
| 3 | `simpleWords[]` | 50 (comment on Num3 still says “Vegetables”) |

`srand(time(0))`, pick one word, `wordManager.initializeWord`. Plays `sound.wav`. `maxHints = min(length/2, 5)`. Hint button `(750, 500)` 150×50 blue. Letters via `TextEntered` (A–Z folded to lowercase). Miss: `++wrongGuesses`. Win: `happy.wav`, message `"Congratulations! You Won!"`. Lose at **`maxWrongGuesses = 6`**: `sound1.wav`, `"You Lost! Word: " + secret`. Gallows: `DisplayManager::drawHangman` (stand, then head/body/arms/legs for `wrongGuesses > 0 … > 5`). Inner `while (1)` polls events and `render`s; **it does not return to `GameBoy` until** `endGame` Exit (`window.close()`, `back = 1`) or Restart (recursive `startGame`).

`WordManager::getHint` reveals the first unguessed letter (counts as guessed). `guessed[20]` limits word length.

---

## Wordle

Constants: `wordListSize = 10`, `gridSize = 5`, `tileSize = 70`, `keyboardRows = 3`. List: `apple grape peach berry lemon mango plums dates pears melon` (all length 5). Loading art `Images/3.png` (2 seconds). Secret lowercased. Input: letters, Backspace, **Enter** when length == 5. Mouse clicks rebuild key `RectangleShape`s at `190 + col*(tileSize-5)`, `450 + row*tileSize`. Colors: green exact, yellow `secretWord.find`, gray `(150,150,150)`. On-screen keyboard drawn cyan in `render`. `endGame` sets `back = 1` immediately when `gameOver` (no extra click). Own unused `RenderWindow window` member. `draw` and `startGame` both `window.display()` (double display).

---

## I/O and assets

Working directory must resolve:

```text
Images/grid.png, snake.png, food.png, snakeLevels.png, arial.ttf,
Images/buttons.png, background.jpg, load1.png, 3.png
game.wav, snake.wav, sound.wav, happy.wav, sound1.wav
```

No save file. Score exists only in `SnakeGame` RAM.

---

## Build and run

```bash
g++ -std=c++17 i222327_SnakeGame.cpp -o SnakeGame \
  -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
./SnakeGame
```

Visual Studio: add the `.cpp`, link SFML Graphics/Window/System/Audio, set debugger working directory next to `Images/` and the WAV files.

### Hub frame loop (what `GameBoy::start` actually does)

Each iteration: `pollEvent` (close; menu/play clicks), then `window.clear()`, then **one** of: menu buttons, play wallpaper, instructions text, quit (`window.close()`), or `currentGame->startGame(e, window)` plus `goBack()` check, then `window.display()`. Snake’s `startGame` also draws the grid via `render` → `Grid::render` **every** call (reloads `Images/grid.png` if the texture load is inside `render` — `loadFromFile` runs each frame when `Grid::render` is invoked).

Hangman / Wordle `startGame` contain **their own** `while` loops with `window.display()`, so they steal the hub loop until those inner loops exit.

### Hangman gallows stages

`DisplayManager::drawHangman(window, wrongGuesses)` always draws the stand (base, pole, top bar). Extra parts:

| `wrongGuesses >` | Part |
|------------------|------|
| 0 | Head `CircleShape(25)` |
| 1 | Body rectangle |
| 2 | Left arm (−45°) |
| 3 | Right arm (+45°) (comment still says “Left Arm”) |
| 4 | Left leg (−30°) |
| 5 | Right leg (+30°) — game lost at `>= 6` |

### Wordle grid and keys

Tiles at `(290 + col*(70+10), 25 + row*(70+10))`. Keyboard rows `QWERTYUIOP` / `ASDFGHJKL` / `ZXCVBNM`. Enter submits only if `currentGuess.length() == gridSize` (5). Six guesses would not fit: `guesses[5]` and `currentRow == gridSize - 1` ends the game (5 rows).

### Play checklist

1. Hear `game.wav`, click Play on `Images/buttons.png`.  
2. Click Snake / Hangman / Wordle regions.  
3. Snake: pick a level rectangle, WASD, `P` pause, eat food for +10.  
4. Hangman: 1/2/3, type letters, optional Hint.  
5. Wordle: five letters + Enter; green/yellow/gray tiles.

---

## Known limitations

| Item | Source fact |
|------|-------------|
| Not a console snake | No `windows.h` / `conio.h` |
| Hangman `while (1)` | Occupies the hub loop until exit/restart |
| `Menu::navigate` | Missing return on default path |
| Self-collision from `i = 4` | Short snakes never self-hit |
| Static `foodTimer` | Shared across SnakeGame instances |
| Instructions | No click-to-return wired in `start()` |
| Wordle `goBack` | Set as soon as the game ends |
| Assets | Required at runtime; not listed in `TREE.txt` |

---

## Author

**Mohammad Rohaan** · roll **22I-2327** (filename `i222327_SnakeGame.cpp`) · [rohaan2802](https://github.com/rohaan2802)
