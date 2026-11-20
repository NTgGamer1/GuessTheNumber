# Number Guessing Game (C)

A simple terminal guessing game written in C. The program chooses a random number from 1 to 100. The player has five scoring attempts and then continues in unlimited mode until the number is found.

## Game Rules

1. Choose **Play** from the main menu.
2. Select whether you have played before or are a new player.
3. Returning players can search for their name and select it from similar leaderboard names.
4. New players enter a new name.
5. Enter a number from 1 to 100.
6. The game shows whether the secret number is higher or lower.
7. The first five valid guesses are scoring attempts.
8. After five unsuccessful attempts, unlimited guessing begins.
9. The game ends only after the correct number is guessed.

## Scoring

| Correct attempt | Stars |
|---|---:|
| 1st | 5 |
| 2nd | 4 |
| 3rd | 3 |
| 4th | 2 |
| 5th | 1 |

A correct guess after the fifth attempt earns zero stars.

Stars from completed games are cumulative. Guesses made in all completed games are also cumulative, including guesses made in unlimited mode.

## Leaderboard

There is only one record for each player name. If a player plays again, their existing stars and guess count are updated.

The leaderboard is sorted by:

1. Higher total stars.
2. Fewer total guesses when stars are equal.

Only the top 10 players are displayed.

The leaderboard file format is:

```text
Name|TotalStars|TotalGuesses
```

Example:

```text
Nikhil|42|83
Aman|35|64
Rahul|35|92
```

The file is named `leaderboard.txt`. If it is missing, the game starts with an empty leaderboard.

## Compile and Run

On Linux or macOS:

```bash
gcc -std=c11 -Wall -Wextra -pedantic GuessingGame.c -o game
./game
```

On Windows with MinGW:

```bash
gcc -std=c11 -Wall -Wextra -pedantic GuessingGame.c -o game.exe
game.exe
```

## C Concepts Used

- Variables and data types
- `if`, `else`, and `else-if` conditions
- `for` and `while` loops
- Functions
- Arrays
- Strings
- Structures
- Pointers
- File handling
- Random number generation
- Input validation
- Basic sorting
- Dynamic memory allocation with `malloc`, `realloc`, and `free`
