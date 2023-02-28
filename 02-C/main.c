
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

// Modulo function as % is remainder and isn't the same thing
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void compute_strategy(FILE *file, 
                      int *opponent_total_score, 
                      int *player_total_score, 
                      bool is_part_two)
{
    // File handling
    char *line_buffer = NULL;
    size_t line_buffer_size = 0;
    ssize_t line_size;
    int line_count = 0;

    // Reset counters
    *opponent_total_score = 0;
    *player_total_score = 0;

    // Get first line
    line_size = getline(&line_buffer, &line_buffer_size, file);

    // Loop over line
    while (line_size >= 0) {
        line_count++;

        // If line is empty then go to next line
        if (!strcmp(line_buffer, "\n")) {
            goto next_line;
        }

        // If not enough characters then go to next line
        if (strlen(line_buffer) < 3) goto next_line;

        char code_a = line_buffer[0];
        char code_b = line_buffer[2];
        int player_a_score = 0;
        int player_b_score = 0;

        // Scissors > Paper (3 > 2)
        // Paper > Rock (2 > 1)
        // Rock > Scissors (1 > 3)
        // n > (n - 2) % 3 + 1

        // Calculate move score for opponent
        switch (code_a) {
            case ('A'):
                player_a_score = 1;
                break;
            case ('B'):
                player_a_score = 2;
                break;
            case ('C'):
                player_a_score = 3;
                break;
        }

        // Calculate move score for player
        switch (code_b) {
            case ('X'):
                player_b_score = is_part_two ? mod(player_a_score - 2, 3) + 1 : 1;
                break;
            case ('Y'):
                player_b_score = is_part_two ? player_a_score : 2;
                break;
            case ('Z'):
                player_b_score = is_part_two ? mod(player_a_score, 3) + 1 : 3;
                break;
        }

        // Draw condition
        if (player_a_score == player_b_score) {
            player_a_score += 3;
            player_b_score += 3;
        }

        // Opponent win condition
        if (mod(player_a_score - 2, 3) + 1 == player_b_score) player_a_score += 6;

        // Player win condition
        if (mod(player_b_score - 2, 3) + 1 == player_a_score) player_b_score += 6;

        // Add round scores to totals
        *opponent_total_score += player_a_score;
        *player_total_score += player_b_score;

next_line:
        line_size = getline(&line_buffer, &line_buffer_size, file);
    }

    // Clean up
    free(line_buffer);
    line_buffer = NULL;
    rewind(file);
}

void part_one(FILE *file)
{
    int opponent_total_score = 0;
    int player_total_score = 0;

    compute_strategy(file, &opponent_total_score, &player_total_score, false);

    printf("Opponent Total Score: %d\n", opponent_total_score);
    printf("Player Total Score: %d\n", player_total_score);
}

void part_two(FILE *file)
{
    int opponent_total_score = 0;
    int player_total_score = 0;

    compute_strategy(file, &opponent_total_score, &player_total_score, true);

    printf("Opponent Total Score: %d\n", opponent_total_score);
    printf("Player Total Score: %d\n", player_total_score);
}

int main(int argc, char** argv)
{
    // Exit if not enough arguments
    if (argc < 2) {
        fprintf(stderr, "Too few arguments given!\nUsage: $ ./main example.txt\n");
        exit(1);
    }
    
    FILE* file = fopen(argv[1], "r");

    // Exit if can't open file
    if (!file) {
        fprintf(stderr, "Error Opening file: %s\n", argv[1]);
        exit(1);
    }

    printf("Input File: %s\n\n", argv[1]);

    printf("==== Part 1 ====\n\n");
    part_one(file);

    printf("\n==== Part 2 ====\n\n");
    part_two(file);

    // Clean up
    fclose(file);

    return 0;
}
