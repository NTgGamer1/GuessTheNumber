# # Number Guessing Game (C)

A terminal game written in C. The program picks a secret number between 1 and 100, and you have 5 attempts to find it. Every game is saved to a leaderboard file, and the top 10 can be viewed from the menu.

## Features

- Secret number from 1 to 100, chosen randomly each round
- 5 attempts per game, with a "Too HIGH" or "Too LOW" hint after each wrong guess
- Attempt counter shown on every turn
- The answer is revealed after the fifth failed attempt
- Star rating based on how quickly you win
- Persistent leaderboard stored in a text file
- Leaderboard shows the top 10 entries, sorted by stars and then by attempts
- Input validation, so letters and out-of-range numbers do not cost an attempt

## Requirements

- A C compiler such as `gcc` or `clang`
- Any OS with a terminal (Linux, macOS, Windows with MinGW or WSL)

## Build and Run

```bash
gcc -Wall -Wextra -o game guessing_game.c
./game
```

On Windows (MinGW): `gcc guessing_game.c -o game.exe` then `game.exe`.

## How to Play

1. Choose **1. Play** from the menu.
2. Enter your name.
3. Type a number from 1 to 100 and press Enter.
4. Use the High/Low hint to narrow it down. You have 5 attempts in total.
5. When the round ends, your result and stars are saved automatically.
6. Choose **2. View leaderboard** to see the top 10 scores.

### Example Session

```
===== NUMBER GUESSING GAME =====
1. Play
2. View leaderboard
3. Exit
Choice: 1

Enter your name: Alice

I picked a number between 1 and 100.
You have 5 attempts. Make your first guess.

Attempt 1/5: 50
Wrong. Too LOW.

Attempt 2/5: 75
Wrong. Too HIGH.
```

## Scoring

| Result                  | Stars |
|-------------------------|-------|
| Correct on attempt 1    | 5     |
| Correct on attempt 2    | 4     |
| Correct on attempt 3    | 3     |
| Correct on attempt 4    | 2     |
| Correct on attempt 5    | 1     |
| Not guessed in 5 tries  | 0     |

Leaderboard ranking: more stars rank higher. If stars are equal, fewer attempts rank higher.

## Leaderboard File

Scores are appended to `leaderboard.txt` in the folder where you run the program. Each line has the format:

```
name|attempts|stars|secret_number
```

Example: `Alice|3|3|47`

Delete `leaderboard.txt` to reset the leaderboard.

## C Concepts Used

| Concept       | Where it is used                                                                 |
|---------------|----------------------------------------------------------------------------------|
| Arrays        | `guesses[]` stores each guess. `Entry[]` holds leaderboard records.              |
| Pointers      | `gp` walks the guesses array. The sort and file loader use pointer arithmetic.   |
| Sorting       | Bubble sort ranks leaderboard entries (`sort_entries`).                          |
| File handling | `fopen`, `fgets`, `sscanf`, `fprintf` read and append `leaderboard.txt`.         |
| Structs       | `Entry` groups name, attempts, stars and secret number.                          |
| Libraries     | `stdio.h`, `stdlib.h`, `string.h`, `time.h`                                      |

## Project Structure

```
.
├── guessing_game.c    # full source code
├── README.md          # this file
└── leaderboard.txt    # created automatically after the first game
```

## Known Limitations

- Every game is stored, including failed ones, so one player can appear multiple times. The board does not keep only each player's best score.
- Only the first 1000 saved games are loaded. Anything beyond that is ignored.
- The leaderboard is a plain text file with no protection. Anyone can edit it and fake a score.
- Names are limited to 31 characters, and the `|` character is replaced with `_`.
- Bubble sort is used for simplicity. It is fine at this scale but slow for very large histories.
