#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_MOVIES 100
#define FILE_NAME "movies.txt"

// Color definitions
#ifdef _WIN32
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_RED 12
#else
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_RED 31
#endif

typedef struct {
    char title[100];
    int watched;
} Movie;

Movie movies[MAX_MOVIES];
int movieCount = 0;

void setColor(int color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#else
    printf("\033[%dm", color);
#endif
}

void resetColor() {
#ifdef _WIN32
    setColor(7);
#else
    printf("\033[0m");
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void loadMoviesFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    while (movieCount < MAX_MOVIES && !feof(file)) {
        char line[150];
        if (fgets(line, sizeof(line), file)) {
            char title[100];
            int watched;
            if (sscanf(line, " %99[^,],%d", title, &watched) == 2) {
                strcpy(movies[movieCount].title, title);
                movies[movieCount].watched = watched;
                movieCount++;
            }
        }
    }
    fclose(file);
}

void saveMoviesToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error saving file!\n");
        return;
    }
    for (int i = 0; i < movieCount; i++) {
        fprintf(file, "%s,%d\n", movies[i].title, movies[i].watched);
    }
    fclose(file);
}

void displayMovies() {
    clearScreen();
    printf("\n===== MOVIE LIST =====\n");
    for (int i = 0; i < movieCount; i++) {
        printf("%d. ", i + 1);
        setColor(COLOR_GREEN);
        printf("%s", movies[i].title);
        resetColor();
        printf(" [");
        movies[i].watched ? setColor(COLOR_YELLOW) : setColor(COLOR_RED);
        printf("%s", movies[i].watched ? "Watched" : "To Watch");
        resetColor();
        printf("]\n");
    }
    printf("\nPress Enter to return to the menu...");
    getchar();
}

void addMovie(int watched) {
    if (movieCount >= MAX_MOVIES) {
        printf("Movie list is full!\nPress Enter to return...");
        getchar();
        return;
    }

    clearScreen();
    printf("Enter movie title (max 99 characters): ");
    char title[100];
    if (!fgets(title, sizeof(title), stdin)) {
        printf("Error reading input!\n");
        return;
    }

    size_t len = strlen(title);
    if (len > 0 && title[len-1] == '\n') {
        title[len-1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Error: Title exceeds 99 characters.\nPress Enter to return...");
        getchar();
        return;
    }

    if (strlen(title) == 0) {
        printf("Error: Title cannot be empty.\nPress Enter to return...");
        getchar();
        return;
    }

    strcpy(movies[movieCount].title, title);
    movies[movieCount].watched = watched;
    movieCount++;

    saveMoviesToFile(FILE_NAME);
    printf("Movie added successfully!\nPress Enter to return...");
    getchar();
}

void moveToWatched();  // Function prototype

void deleteMovie() {
    clearScreen();
    if (movieCount == 0) {
        printf("Movie list is empty!\nPress Enter to return...");
        getchar();
        return;
    }

    displayMovies();
    printf("Enter movie number to delete (1-%d): ", movieCount);
    char input[10];
    fgets(input, sizeof(input), stdin);

    char *endptr;
    long index = strtol(input, &endptr, 10);
    if (endptr == input || (*endptr != '\n' && *endptr != '\0')) {
        printf("Invalid input. Please enter a number.\nPress Enter to return...");
        getchar();
        return;
    }

    if (index < 1 || index > movieCount) {
        printf("Invalid number! Enter 1-%d.\nPress Enter to return...", movieCount);
        getchar();
        return;
    }

    for (int i = index - 1; i < movieCount - 1; i++) {
        movies[i] = movies[i + 1];
    }
    movieCount--;

    saveMoviesToFile(FILE_NAME);
    printf("Movie deleted successfully!\nPress Enter to return...");
    getchar();
}

void moveToWatched() {
    clearScreen();
    if (movieCount == 0) {
        printf("Movie list is empty!\nPress Enter to return...");
        getchar();
        return;
    }

    // Display only unwatched movies
    printf("\n===== TO WATCH MOVIES =====\n");
    int unwatchedCount = 0;
    int unwatchedIndices[MAX_MOVIES];

    for (int i = 0; i < movieCount; i++) {
        if (!movies[i].watched) {
            printf("%d. ", unwatchedCount + 1);
            setColor(COLOR_GREEN);
            printf("%s", movies[i].title);
            resetColor();
            printf(" [");
            setColor(COLOR_RED);
            printf("To Watch");
            resetColor();
            printf("]\n");

            unwatchedIndices[unwatchedCount] = i;
            unwatchedCount++;
        }
    }

    if (unwatchedCount == 0) {
        printf("No movies in 'To Watch' list!\nPress Enter to return...");
        getchar();
        return;
    }

    printf("\nEnter movie number to mark as watched (1-%d): ", unwatchedCount);
    char input[10];
    fgets(input, sizeof(input), stdin);

    char *endptr;
    long index = strtol(input, &endptr, 10);
    if (endptr == input || (*endptr != '\n' && *endptr != '\0')) {
        printf("Invalid input. Please enter a number.\nPress Enter to return...");
        getchar();
        return;
    }

    if (index < 1 || index > unwatchedCount) {
        printf("Invalid number! Enter 1-%d.\nPress Enter to return...", unwatchedCount);
        getchar();
        return;
    }

    // Mark the selected movie as watched
    int actualIndex = unwatchedIndices[index - 1];
    movies[actualIndex].watched = 1;

    saveMoviesToFile(FILE_NAME);
    printf("Movie marked as watched successfully!\nPress Enter to return...");
    getchar();
}

void showMenu() {
    printf("\n===== MOVIE MANAGEMENT SYSTEM =====\n");
    printf("1 - Add to 'To Watch'\n");
    printf("2 - Add to 'Watched'\n");
    printf("3 - Display all movies\n");
    printf("4 - Move to 'Watched'\n");
    printf("5 - Delete movie\n");
    printf("0 - Exit\n");
    printf("===================================\n");
    printf("Choose an option: ");
}

int main() {
    loadMoviesFromFile(FILE_NAME);
    char input[10];
    int option;

    do {
        clearScreen();
        showMenu();
        fgets(input, sizeof(input), stdin);

        char *endptr;
        option = strtol(input, &endptr, 10);
        if (endptr == input || (*endptr != '\n' && *endptr != '\0')) {
            printf("Invalid option! Press Enter...");
            getchar();
            continue;
        }

        switch (option) {
            case 1: addMovie(0); break;
            case 2: addMovie(1); break;
            case 3: displayMovies(); break;
            case 4: moveToWatched(); break;
            case 5: deleteMovie(); break;
            case 0: printf("Exiting...\n"); break;
            default: printf("Invalid option! Press Enter..."); getchar();
        }
    } while (option != 0);

    return 0;
}