#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int symb_link = 0;  /* флаг символической ссылки*/
    char *original = NULL;
    char *new_link = NULL;

    if (argc == 3) {
        /* ln original new_link */
        original = argv[1];
        new_link = argv[2];
    } else if (argc == 4 && strcmp(argv[1], "-s") == 0) {
        /* ln -s original new_link */
        symb_link = 1;
        original = argv[2];
        new_link = argv[3];
    } else {
        fprintf(stderr, "Неверные аргументы\n");
        return 1;
    }
    
    /* создание ссылки*/
    if (symb_link) {
        /* символическая ссылка */
        if (symlink(original, new_link) == -1) {
            perror("ln");
            return 1;
        }
        printf("Создание символической ссылки: '%s' -> '%s'\n", new_link, original);
    } else {
        /* жесткая ссылка */
        if (link(original, new_link) == -1) {
            perror("ln");
            return 1;
        }
        printf("Создание жесткой ссылки: '%s' -> '%s'\n", new_link, original);
    }
    
    return 0;
}
