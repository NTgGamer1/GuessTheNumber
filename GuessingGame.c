#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ATTEMPTS 5
#define NAME_LENGTH 32
#define LEADERBOARD_FILE "leaderboard.txt"
#define STARTING_CAPACITY 10
#define TOP_PLAYERS 10

typedef struct
{
    char name[NAME_LENGTH];
    int total_stars;
    int total_guesses;
} Player;

static void strip_newline(char *text)
{
    text[strcspn(text, "\r\n")] = '\0';
}

/* Returns 1 for valid input, 0 for invalid input, and -1 for EOF. */
static int read_integer(int *number)
{
    char line[64];
    char extra;

    if (fgets(line, sizeof line, stdin) == NULL)
        return -1;

    if (strchr(line, '\n') == NULL)
    {
        int character;
        while ((character = getchar()) != '\n' && character != EOF)
        {
        }
    }

    if (sscanf(line, "%d %c", number, &extra) == 1)
        return 1;
    return 0;
}

static int read_name(char *name, int size)
{
    char line[128];

    if (fgets(line, sizeof line, stdin) == NULL)
        return 0;

    if (strchr(line, '\n') == NULL)
    {
        int character;
        while ((character = getchar()) != '\n' && character != EOF)
        {
        }
    }

    strip_newline(line);
    if (line[0] == '\0')
    {
        strcpy(name, "Anonymous");
    }
    else
    {
        strncpy(name, line, size - 1);
        name[size - 1] = '\0';
    }

    for (char *character = name; *character != '\0'; character++)
    {
        if (*character == '|')
            *character = '_';
    }

    return 1;
}

static int read_guess(int *guess)
{
    int result = read_integer(guess);

    if (result == -1)
        return -1;
    if (result == 0)
    {
        printf("That is not a number.\n");
        return 0;
    }
    if (*guess < 1 || *guess > 100)
    {
        printf("Out of range. Enter a number from 1 to 100.\n");
        return 0;
    }
    return 1;
}

static void sort_leaderboard(Player *players, int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            int player_should_move =
                players[j].total_stars > players[i].total_stars ||
                (players[j].total_stars == players[i].total_stars &&
                 players[j].total_guesses < players[i].total_guesses);

            if (player_should_move)
            {
                Player temporary = players[i];
                players[i] = players[j];
                players[j] = temporary;
            }
        }
    }
}

static void save_leaderboard(Player *players, int count)
{
    FILE *file = fopen(LEADERBOARD_FILE, "w");
    if (file == NULL)
    {
        printf("Could not open %s for writing.\n", LEADERBOARD_FILE);
        return;
    }

    sort_leaderboard(players, count);

    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s|%d|%d\n",
                players[i].name,
                players[i].total_stars,
                players[i].total_guesses);
    }

    if (fclose(file) != 0)
        printf("Could not finish saving %s.\n", LEADERBOARD_FILE);
}

static Player *load_leaderboard(int *count, int *capacity)
{
    *count = 0;
    *capacity = STARTING_CAPACITY;
    Player *players = (Player *)malloc((size_t)*capacity * sizeof(Player));

    if (players == NULL)
    {
        printf("Memory allocation failed.\n");
        *capacity = 0;
        return NULL;
    }

    FILE *file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL)
        return players;

    char line[128];
    while (fgets(line, sizeof line, file) != NULL)
    {
        char name[NAME_LENGTH];
        int total_stars;
        int total_guesses;

        strip_newline(line);

        int separator_count = 0;
        for (char *character = line; *character != '\0'; character++)
        {
            if (*character == '|')
                separator_count++;
        }

        if (separator_count == 2 &&
            sscanf(line, "%31[^|]|%d|%d", name, &total_stars,
                   &total_guesses) == 3 &&
            name[0] != '\0' &&
            total_stars >= 0 &&
            total_guesses >= 0)
        {
            int duplicate = 0;
            for (int i = 0; i < *count; i++)
            {
                if (strcmp(players[i].name, name) == 0)
                {
                    duplicate = 1;
                    break;
                }
            }

            if (duplicate)
                continue;

            if (*count == *capacity)
            {
                int new_capacity = *capacity * 2;
                Player *new_players =
                    (Player *)realloc(players, (size_t)new_capacity * sizeof(Player));

                if (new_players == NULL)
                {
                    printf("Memory allocation failed. Some records could not be loaded.\n");
                    break;
                }

                players = new_players;
                *capacity = new_capacity;
            }

            strcpy(players[*count].name, name);
            players[*count].total_stars = total_stars;
            players[*count].total_guesses = total_guesses;
            (*count)++;
        }
    }

    fclose(file);
    sort_leaderboard(players, *count);
    return players;
}

static int find_player(Player *players, int count, const char *name)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(players[i].name, name) == 0)
            return i;
    }
    return -1;
}

static void show_leaderboard(Player *players, int count)
{
    sort_leaderboard(players, count);

    if (count == 0)
    {
        printf("\nNo players have played yet.\n");
        return;
    }

    printf("\n================ LEADERBOARD (Top %d) ================\n",
           TOP_PLAYERS);
    printf("%-4s %-20s %-10s %s\n",
           "Rank", "Player", "Stars", "Guesses");
    printf("-----------------------------------------------------\n");

    int shown = count < TOP_PLAYERS ? count : TOP_PLAYERS;
    for (int i = 0; i < shown; i++)
    {
        printf("%-4d %-20s %-10d %d\n",
               i + 1,
               players[i].name,
               players[i].total_stars,
               players[i].total_guesses);
    }
    printf("=====================================================\n");
}

static void play_game(Player **players, int *count, int *capacity)
{
    char name[NAME_LENGTH];

    printf("\nEnter your name: ");
    if (!read_name(name, sizeof name))
    {
        printf("Could not read the player name.\n");
        return;
    }

    int secret_number = rand() % 100 + 1;
    int attempts = 0;
    int game_guesses = 0;
    int stars = 0;
    int won = 0;

    printf("\nI picked a number between 1 and 100.\n");
    printf("You have 5 scoring attempts.\n");

    while (!won)
    {
        int guess;

        if (attempts < MAX_ATTEMPTS)
        {
            printf("\nScoring attempt %d/%d: ", attempts + 1, MAX_ATTEMPTS);
        }
        else
        {
            printf("\nUnlimited attempt: ");
        }

        int result = read_guess(&guess);
        if (result == -1)
        {
            printf("\nInput ended. This game was not completed or saved.\n");
            return;
        }
        if (result == 0)
            continue;

        attempts++;
        game_guesses++;

        if (guess == secret_number)
        {
            won = 1;
            if (attempts <= MAX_ATTEMPTS)
                stars = MAX_ATTEMPTS - attempts + 1;
            else
                stars = 0;
        }
        else if (guess > secret_number)
        {
            printf("Wrong. Too HIGH.");
        }
        else
        {
            printf("Wrong. Too LOW.");
        }

        if (!won && attempts == MAX_ATTEMPTS)
        {
            printf("\nYou have used all 5 scoring attempts.");
            printf("\nUnlimited mode is now active. Keep guessing!");
        }
        else if (!won)
        {
            printf("\n");
        }
    }

    printf("\nCorrect! The number was %d.\n", secret_number);
    printf("Stars earned this game: %d\n", stars);
    printf("Guesses in this game: %d\n", game_guesses);

    int player_index = find_player(*players, *count, name);

    if (player_index == -1)
    {
        if (*count == *capacity)
        {
            int new_capacity = (*capacity == 0) ? STARTING_CAPACITY : *capacity * 2;
            Player *new_players =
                (Player *)realloc(*players,
                                  (size_t)new_capacity * sizeof(Player));

            if (new_players == NULL)
            {
                printf("Memory allocation failed. Score was not saved.\n");
                return;
            }

            *players = new_players;
            *capacity = new_capacity;
        }

        strcpy((*players)[*count].name, name);
        (*players)[*count].total_stars = stars;
        (*players)[*count].total_guesses = game_guesses;
        (*count)++;
        player_index = *count - 1;
    }
    else
    {
        (*players)[player_index].total_stars += stars;
        (*players)[player_index].total_guesses += game_guesses;
    }

    save_leaderboard(*players, *count);
    printf("Score saved to %s.\n", LEADERBOARD_FILE);
}

int main(void)
{
    srand((unsigned)time(NULL));

    int count = 0;
    int capacity = 0;
    Player *players = load_leaderboard(&count, &capacity);

    if (players == NULL)
        return 1;

    for (;;)
    {
        int choice;

        printf("\n===== NUMBER GUESSING GAME =====\n");
        printf("1. Play\n");
        printf("2. View leaderboard\n");
        printf("3. Exit\n");
        printf("Choice: ");

        int result = read_integer(&choice);
        if (result == -1)
        {
            printf("\nGoodbye.\n");
            break;
        }
        if (result == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        if (choice == 1)
        {
            play_game(&players, &count, &capacity);
        }
        else if (choice == 2)
        {
            show_leaderboard(players, count);
        }
        else if (choice == 3)
        {
            printf("Goodbye.\n");
            break;
        }
        else
        {
            printf("Pick 1, 2 or 3.\n");
        }
    }

    free(players);
    return 0;
}
