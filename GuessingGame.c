#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ATTEMPTS 5
#define NAME_LEN 32
#define MAX_ENTRIES 1000
#define TOP_N 10
#define LB_FILE "leaderboard.txt"

typedef struct
{
    char name[NAME_LEN];
    int attempts; /* attempts used */
    int stars;    /* 0 to 5 */
    int secret;   /* the number of that round */
} Entry;

/* ---------- input helpers ---------- */

static void strip_newline(char *s)
{
    s[strcspn(s, "\r\n")] = '\0';
}

/* Reads an int from stdin safely. Returns 1 on success, 0 on bad input. */
static int read_int(int *out)
{
    char buf[64];
    char extra;
    if (fgets(buf, sizeof buf, stdin) == NULL)
        return 0;
    if (sscanf(buf, "%d %c", out, &extra) != 1) /* reject "12abc" */
        return 0;
    return 1;
}

/* ---------- scoring ---------- */

/* 1st try = 5 stars ... 5th try = 1 star, failed = 0 stars */
static int calc_stars(int attempts, int won)
{
    if (!won)
        return 0;
    return MAX_ATTEMPTS - attempts + 1;
}

static void print_stars(int n)
{
    for (int i = 0; i < MAX_ATTEMPTS; i++)
        putchar(i < n ? '*' : '-');
}

/* ---------- file handling ---------- */

static int load_entries(Entry *arr, int max)
{
    FILE *fp = fopen(LB_FILE, "r");
    if (!fp)
        return 0; /* no history yet */

    int n = 0;
    char line[128];
    while (n < max && fgets(line, sizeof line, fp))
    {
        Entry *e = arr + n; /* pointer arithmetic */
        if (sscanf(line, "%31[^|]|%d|%d|%d",
                   e->name, &e->attempts, &e->stars, &e->secret) == 4)
            n++;
    }
    fclose(fp);
    return n;
}

static int save_entry(const Entry *e)
{
    FILE *fp = fopen(LB_FILE, "a");
    if (!fp)
    {
        printf("Could not write to %s. Your score was NOT saved.\n", LB_FILE);
        return 0;
    }
    fprintf(fp, "%s|%d|%d|%d\n", e->name, e->attempts, e->stars, e->secret);
    fclose(fp);
    return 1;
}

/* ---------- sorting (bubble sort using pointers) ---------- */

/* returns 1 if *a should come after *b: fewer stars, or same stars but more attempts */
static int ranks_lower(const Entry *a, const Entry *b)
{
    if (a->stars != b->stars)
        return a->stars < b->stars;
    return a->attempts > b->attempts;
}

static void sort_entries(Entry *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int swapped = 0;
        for (Entry *p = arr; p < arr + (n - 1 - i); p++)
        {
            if (ranks_lower(p, p + 1))
            {
                Entry tmp = *p;
                *p = *(p + 1);
                *(p + 1) = tmp;
                swapped = 1;
            }
        }
        if (!swapped)
            break;
    }
}

/* ---------- leaderboard ---------- */

static void show_leaderboard(void)
{
    static Entry entries[MAX_ENTRIES];
    int n = load_entries(entries, MAX_ENTRIES);

    if (n == 0)
    {
        printf("\nNo games played yet. Leaderboard is empty.\n");
        return;
    }

    sort_entries(entries, n);
    int shown = n < TOP_N ? n : TOP_N;

    printf("\n================ LEADERBOARD (Top %d) ================\n", TOP_N);
    printf("%-4s %-20s %-9s %-7s %s\n", "Rank", "Player", "Stars", "Tries", "Number");
    printf("-----------------------------------------------------\n");
    for (int i = 0; i < shown; i++)
    {
        const Entry *e = entries + i;
        printf("%-4d %-20s ", i + 1, e->name);
        print_stars(e->stars);
        printf("     %-7d %d\n", e->attempts, e->secret);
    }
    printf("=====================================================\n");
}

/* ---------- game ---------- */

static void play_game(void)
{
    Entry result;
    int secret = rand() % 100 + 1;
    int guesses[MAX_ATTEMPTS]; /* array of guesses made */
    int *gp = guesses;         /* pointer into that array */
    int won = 0;

    printf("\nEnter your name: ");
    if (fgets(result.name, sizeof result.name, stdin) == NULL)
        return;
    strip_newline(result.name);
    if (result.name[0] == '\0')
        strcpy(result.name, "Anonymous");
    for (char *c = result.name; *c; c++) /* '|' is our file separator */
        if (*c == '|')
            *c = '_';

    printf("\nI picked a number between 1 and 100.\n");
    printf("You have %d attempts. Make your first guess.\n", MAX_ATTEMPTS);

    int attempts = 0;
    while (attempts < MAX_ATTEMPTS)
    {
        int g;
        printf("\nAttempt %d/%d: ", attempts + 1, MAX_ATTEMPTS);

        if (!read_int(&g))
        {
            printf("That is not a number. No attempt used, try again.\n");
            continue;
        }
        if (g < 1 || g > 100)
        {
            printf("Out of range. Must be 1 to 100. No attempt used.\n");
            continue;
        }

        *gp++ = g;
        attempts++;

        if (g == secret)
        {
            won = 1;
            break;
        }
        else if (g > secret)
        {
            printf("Wrong. Too HIGH.\n");
        }
        else
        {
            printf("Wrong. Too LOW.\n");
        }
    }

    result.attempts = attempts;
    result.secret = secret;
    result.stars = calc_stars(attempts, won);

    printf("\n-------------------------------------\n");
    if (won)
    {
        printf("Correct! %d was the number. You took %d attempt(s).\n", secret, attempts);
    }
    else
    {
        printf("You failed. All %d attempts used.\n", MAX_ATTEMPTS);
        printf("The answer was: %d\n", secret);
    }

    printf("Your guesses: ");
    for (int *p = guesses; p < gp; p++)
        printf("%d ", *p);
    printf("\nStars earned: ");
    print_stars(result.stars);
    printf(" (%d/%d)\n", result.stars, MAX_ATTEMPTS);
    printf("-------------------------------------\n");

    if (save_entry(&result))
        printf("Score saved to leaderboard.\n");
}

int main(void)
{
    srand((unsigned)time(NULL));

    for (;;)
    {
        int choice;
        printf("\n===== NUMBER GUESSING GAME =====\n");
        printf("1. Play\n");
        printf("2. View leaderboard\n");
        printf("3. Exit\n");
        printf("Choice: ");

        if (!read_int(&choice))
        {
            printf("Invalid input.\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            play_game();
            break;
        case 2:
            show_leaderboard();
            break;
        case 3:
            printf("Goodbye.\n");
            return 0;
        default:
            printf("Pick 1, 2 or 3.\n");
        }
    }
}