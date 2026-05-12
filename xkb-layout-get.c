#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define RULES_PATH_CONCAT "/share/X11/xkb/rules/base.lst"
#define RULES_PATH_CONCAT_STR_LEN 29

#define LAYOUTS_PATH_CONCAT "/xkb_layouts.tsv"
#define LAYOUTS_PATH_CONCAT_STR_LEN 16

#define LAYOUTS_FIND_STR "! layout"

#define KEYBOARD_NAME "at-translated-set-2-keyboard"
#define KEYBOARD_NAME_STR_LEN 28

int main(void)
{
    FILE *fptr = popen("hyprctl devices", "r");
    if (!fptr) {
        printf("Failed to run 'hyprctl devices'");
        return EXIT_FAILURE;
    }

    char *current_layout = NULL;
    char *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, fptr) != -1) {
        // remove leading tabs
        while (*line == '\t') {
            line += 1;
        }
        if (strncmp(line, KEYBOARD_NAME, KEYBOARD_NAME_STR_LEN) == 0) {
            // skip to the line: "active keymap: ..."
            for (int i = 0; i < 3; i++) {
                getline(&line, &line_len, fptr);
            }

            char *delim_addr = strchr(line, ':');
            // skip the delim and the space after it
            delim_addr += 2;
            // remove the newline at the end
            *(delim_addr + strlen(delim_addr) - 1) = '\0';

            current_layout = malloc(strlen(delim_addr));
            strcpy(current_layout, delim_addr);
            break;
        }
    }

    if (!current_layout) {
        printf("Failed to get current keyboard layout");
        return EXIT_FAILURE;
    }

    char *xdg_runtime_dir_path = getenv("XDG_RUNTIME_DIR");
    unsigned long xdg_strlen = strlen(xdg_runtime_dir_path);
    char *layouts_path = malloc(xdg_strlen + LAYOUTS_PATH_CONCAT_STR_LEN + 1);

    strcpy(layouts_path, xdg_runtime_dir_path);
    strcpy(layouts_path + xdg_strlen, LAYOUTS_PATH_CONCAT);

    FILE *layouts_fptr = fopen(layouts_path, "r");

    if (!layouts_fptr){
        layouts_fptr = fopen(layouts_path, "w");
        // or read /usr/share/X11/xkb/rules/base.lst
        fptr = popen("nix eval --raw nixpkgs#xkeyboard-config", "r");
        if (!fptr) {
            printf("Failed to run 'nix eval --raw nixpkgs#xkeyboard-config'");
            return EXIT_FAILURE;
        }
        getline(&line, &line_len, fptr);
        unsigned long base_path_strlen = strlen(line);
        char *rules_path = malloc(base_path_strlen + RULES_PATH_CONCAT_STR_LEN + 1);
        pclose(fptr);

        strcpy(rules_path, line);
        strcpy(rules_path + base_path_strlen, RULES_PATH_CONCAT);

        line = NULL;
        line_len = 0;
        fptr = fopen(rules_path, "r");
        while (getline(&line, &line_len, fptr) != -1) {
            // remove trailing newline
            *(line + strlen(line) - 1) = '\0';
            if (strcmp(line, LAYOUTS_FIND_STR) == 0) {
                while (getline(&line, &line_len, fptr) != -1 && *line != '\n') {
                    char *first_field = malloc(strlen(line) + 1);
                    strcpy(first_field, line);
                    while (isspace(*first_field)) {
                        first_field++;
                    }
                    int end = 0;
                    while (!isspace(*(first_field + end))) {
                        end++;
                    }
                    *(first_field + end) = '\0';
                    char *next_field = first_field + end + 2;
                    while (isspace(*next_field)) {
                        next_field++;
                    }
                    *(next_field + strlen(next_field) - 1) = '\0';
                    fprintf(layouts_fptr, "%s\t%s\n", next_field, first_field);
                    // printf("%s\t%s\n", next_field, first_field);
                }
            }
        }
        fclose(fptr);
        fclose(layouts_fptr);
        layouts_fptr = fopen(layouts_path, "r");
        if (!layouts_fptr) {
            printf("Failed to read layouts");
            return EXIT_FAILURE;
        }
    }

    line = NULL;
    line_len = 0;
    while (getline(&line, &line_len, layouts_fptr) != -1) {
        // break;
        // compare first 'field' with the current layout
        char *delim_addr = strchr(line, '\t');
        size_t delim_offset = delim_addr - line;
        // printf("'%.*s'\t'%.*s'\t'%i'\t'%s'\n", (int)(strlen(line) - 1), line, (int)(strlen(delim_addr) - 1), delim_addr, delim_offset, current_layout);
        // printf("'%i'\n", delim_offset);
        // printf("%i", strncmp(line, current_layout, delim_offset));
        if (strncmp(line, current_layout, delim_offset) == 0) {
            // newline is already included
            printf("%s", delim_addr + 1);
            break;
        }
    }
    return EXIT_SUCCESS;
}
