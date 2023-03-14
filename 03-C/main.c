
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define CACHE_SIZE 52

// Macro to handle special return system
#define return_defer(value) do {    \
    result = value;                 \
    goto defer;                     \
} while(0);

typedef int Errno;

typedef struct {
    char *line;
    size_t line_size;
    bool items[CACHE_SIZE];
} Rucksack;

// Get the size of the file
Errno get_file_size(FILE *file, size_t *file_size)
{
    // Get the current offset of the cursor
    long offset = ftell(file);
    if (0 > offset) return errno;

    // Set the cursor to the end of the file
    if (0 > fseek(file, offset, SEEK_END)) return errno;

    // Get the file size
    long size = ftell(file);
    if (0 > size) return errno;

    // Set the cursor back to where it was
    if (0 > fseek(file, offset, SEEK_SET)) return errno;

    // Set the file size
    *file_size = (size_t) size;

    return errno;
}

// Reads the entire file contents into the provided buffer
Errno read_entire_file(const char *file_path, char **buffer, size_t *buffer_size)
{
    Errno result = 0;
    FILE *file = fopen(file_path, "r");

    // Catch error opening file
    if (!file) {
        fprintf(stderr, "Unable to open file %s!\n", file_path, errno);
        return_defer(errno);
    }

    // Get size of the file
    Errno err = get_file_size(file, buffer_size);
    if (0 > err) return_defer(err);
    
    // Allocate size for the text
    *buffer = (char*) calloc(*buffer_size, sizeof(char));
    
    // Read contents into the buffer
    fread(*buffer, 1, *buffer_size, file);
    if (ferror(file)) return_defer(errno);

    // Make sure to always close the file if an error occurred
defer:
    if (file) fclose(file);
    return errno;
}

// Tokenises the the content by lines
void split_by_lines(char *buffer, size_t buffer_size, char ***lines, size_t *lines_count)
{
    (void) lines;
    size_t line_count = 1;

    // Get number of lines
    for (size_t i = 0; i < buffer_size; i++) {
        if (buffer[i] == '\n') line_count++;
    }

    *lines_count = line_count;

    // Allocate the string array
    *lines = calloc(line_count, sizeof(char*));

    // Populate string array
    size_t line_size = 0;
    size_t current_line = 0;
    for (size_t i = 0; i < buffer_size; i++) {
        // Create new substring when new line char detected or end of string
        if (buffer[i] == '\n' || i == buffer_size - 1) {
            char *line = calloc(line_size + 1, sizeof(char));
            // Copy the contents of the line into the new line string
            strncpy(line, (const char*) (buffer + i - line_size), line_size);
            line[line_size] = '\0';
            
            (*lines)[current_line] = line;

            // Start next line
            line_size = 0;
            current_line++;
            continue;
        };

        line_size++;
    }
}

void part_one(char **lines, const size_t lines_count)
{
    int total_priority = 0;

    // Go through lines and identify duplicate values
    for (size_t i = 0; i < lines_count; i++) {
        Rucksack rucksack = { 
            .line = lines[i], 
            .line_size = strlen(lines[i]),
            .items = {0}
        };

        // Loop through all the items and compare them
        for (size_t j = 0; j < rucksack.line_size; j++) {
            char priority = (rucksack.line[j] ^ 0x20) & 0xBF;
            if (priority > 26) priority -= 6;

            // Only check when we reach the second compartment
            if (j < rucksack.line_size / 2) {
                rucksack.items[priority - 1] = true;
            } else if (j >= rucksack.line_size / 2) {
                // If item exists in the cache, we found it
                if (rucksack.items[priority - 1]) {
                    total_priority += priority;
                    break;
                }
            }
        }
    }

    printf("Total Priority: %d\n", total_priority);
}

void part_two(char **lines, const size_t lines_count)
{
    #define GROUP_SIZE 3
    int total_priority = 0;

    // Go through lines and identify duplicate values
    for (size_t i = 0; i < lines_count; i+= GROUP_SIZE) {
        Rucksack group[GROUP_SIZE] = {0};

        // Loop through group and initialise their rucksacks
        for (size_t j = 0; j < GROUP_SIZE; j++) {
            group[j] = (Rucksack) {
                .line = lines[i + j],
                .line_size = strlen(lines[i + j]),
                .items = {0}
            };
        }

        // Loop through all three lines and cache values
        for (size_t gi = 0; gi < GROUP_SIZE; gi++) {
            for (size_t j = 0; j < group[gi].line_size; j++) {
                char priority = (group[gi].line[j] ^ 0x20) & 0xBF;
                if (priority > 26) priority -= 6;
                group[gi].items[priority - 1] = true;
            }
        }

        // Loop through cache size and compare all three values
        for (size_t j = 0; j < CACHE_SIZE; j++) {
            for (size_t gi = 0; gi < GROUP_SIZE; gi++) {
                if (!group[gi].items[j]) goto next_priority;
            }
            total_priority += j + 1;
next_priority:
            continue;
        }
    }

    printf("Total Priority: %d\n", total_priority);
}

int main(int argc, char **argv)
{
    // Check to make sure file_path is provided
    if (argc < 2) {
        fprintf(stderr, "Not enough arguments provided!\nUsage: ./main example.txt\n");
        exit(1);
    }

    // Try to open file
    char *file_path = argv[1];
    char *buffer = 0;
    size_t buffer_size = 0;

    // Read the contents of the file into the buffer
    Errno err = read_entire_file(file_path, &buffer, &buffer_size);
    if (0 > err) {
        fprintf(stderr, "Unable to read file!\n");
        exit(1);
    }

    char **lines = 0;
    size_t lines_count = 0;

    // Split the buffer by lines
    split_by_lines(buffer, buffer_size, &lines, &lines_count);

    // Run the solutions
    printf("Input File: %s\n\n", file_path);

    printf("==== Part 1 ====\n\n");
    part_one(lines, lines_count);

    printf("\n==== Part 2 ====\n\n");
    part_two(lines, lines_count);

    // Free everything
    free(buffer);

    for (size_t i = 0; i < lines_count; i++) {
        free(lines[i]);
    }

    free(lines);
}
