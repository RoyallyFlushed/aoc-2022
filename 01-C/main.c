
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void compute_highest_calories(FILE *file, 
                              int depth, 
                              unsigned int *highest_elves, 
                              unsigned int *highest_calories)
{
    // File handling
    char *line_buffer = NULL;
    size_t line_buffer_size = 0;
    ssize_t line_size;
    int line_count = 0;

    // Counters
    unsigned int elf = 1;
    unsigned int elf_calories = 0;

    // Zero initialise the arrays
    for (int i = 0; i < depth; i++) {
        highest_elves[i] = 0;
        highest_calories[i] = 0;
    }

    // Get first line of file
    line_size = getline(&line_buffer, &line_buffer_size, file);

    // Loop through the file
    while (line_size >= 0) {
        line_count++;

        // Check if line is empty
        if (!strcmp(line_buffer, "\n")) {
            elf++;
            elf_calories = 0;
            goto next_line;
        }

        int calorie = atoi(line_buffer);

        // Add calories and check highest
        if (calorie > 0) elf_calories += calorie;
        
        for (int i = 0; i < depth; i++) {
            if (elf_calories > highest_calories[i]) {
                // Shift everything down from chosen spot if we're on a different elf
                if (elf != highest_elves[i]) {
                    for (int j = depth - 1; j > i; j--) {
                        highest_elves[j] = highest_elves[j - 1];
                        highest_calories[j] = highest_calories[j - 1];
                    }
                }

                highest_elves[i] = elf;
                highest_calories[i] = elf_calories;

                break;
            }
        }
        
next_line:
        line_size = getline(&line_buffer, &line_buffer_size, file);
    }

    // Clean up
    free(line_buffer);
    line_buffer = NULL;
    rewind(file);
}

void part_1(FILE *file)
{
    // Set depth
    int depth = 1;

    // Set variables
    unsigned int highest_elves[depth];
    unsigned int highest_calories[depth];

    // Run computation
    compute_highest_calories(file, depth, highest_elves, highest_calories);

    // Output result
    printf("Elf: %u     Calories: %u\n", highest_elves[0], highest_calories[0]);
}

void part_2(FILE *file)
{
    // Set depth
    int depth = 3;

    // Set variables
    unsigned int highest_elves[depth];
    unsigned int highest_calories[depth];

    // Run computation
    compute_highest_calories(file, depth, highest_elves, highest_calories);

    // Sum top 3 calorie totals
    unsigned int total_calories = 0;

    for (int i = 0; i < depth; i++) {
        total_calories += highest_calories[i];
    }

    // Output results
    for (int i = 0; i < depth; i++) {
        printf("Position: %d    -   Elf: %u     Calories: %u\n", 
                i + 1, highest_elves[i], highest_calories[i]);
    }

    printf("Total Calories: %u\n", total_calories);
}

int main(int argc, char **argv)
{
    // Check if enough args have been given
    if (argc < 2) {
        fprintf(stderr, "Too few arguments given!\n Usage: $ ./main FILE_PATH\n");
        exit(1);
    }

    // Get file ready to read
    FILE *file = fopen(argv[1], "r");

    // Handle failure to open file
    if (!file) {
        fprintf(stderr, "Error opening file '%s'\n", argv[1]);
        exit(1);
    }

    // Output metadata
    printf("Input File: %s\n\n", argv[1]);

    // Run solutions
    printf("==== Part 1 ====\n\n");
    part_1(file);

    printf("\n==== Part 2 ====\n\n");
    part_2(file);

    // Clean up
    fclose(file);

    return 0;
}