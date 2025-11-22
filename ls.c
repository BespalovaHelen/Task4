#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

typedef struct {
    int recursive;    /* -R */
    int long_format;  /* -l */
    int show_group;   /* -g */
} Flags;

/* Функция получения имени владельца */
char* get_owner_name(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

/* Функция получения имени группы */
char* get_group_name(gid_t gid) {
    struct group *gr = getgrgid(gid);
    return gr ? gr->gr_name : "unknown";
}

/* Функция получения строки с правами доступа */
void get_permissions(mode_t mode, char *str) {
    /* Определяем тип файла */
    if (S_ISDIR(mode)) str[0] = 'd';
    else if (S_ISLNK(mode)) str[0] = 'l';
    else if (S_ISCHR(mode)) str[0] = 'c';
    else if (S_ISBLK(mode)) str[0] = 'b';
    else if (S_ISFIFO(mode)) str[0] = 'p';
    else if (S_ISSOCK(mode)) str[0] = 's';
    else str[0] = '-';

    /* Права для владельца */
    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ? 'x' : '-';
    
    /* Права для группы */
    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';
    
    /* Права для остальных */
    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';
    
    str[10] = '\0';
}

/* Функция вывода информации о файле */
void print_file_info(const char *path, const char *filename, Flags flags) {
    struct stat file_stat;
    char full_path[PATH_MAX];
    
    /* Строим полный путь к файлу */
    if (strcmp(path, ".") == 0) {
        snprintf(full_path, sizeof(full_path), "%s", filename);
    } else {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, filename);
    }
    
        if (lstat(full_path, &file_stat) == -1) {
        perror("ls");
        return;
    }
    
    if (flags.long_format) {
        char permissions[11];
        get_permissions(file_stat.st_mode, permissions);
        
        /* Выводим права доступа и количество ссылок */
        printf("%s %2ld ", permissions, file_stat.st_nlink);
        
        /* Выводим имя владельца */
        printf("%-8s ", get_owner_name(file_stat.st_uid));
        
        /* Выводим имя группы, если указан -g */
        if (flags.show_group) {
            printf("%-8s ", get_group_name(file_stat.st_gid));
        }
        
        /* Выводим размер для обычных файлов */
        if (S_ISREG(file_stat.st_mode)) {
            printf("%8ld ", file_stat.st_size);
        } else {
            printf("%8s ", "");
        }
        
        /* Выводим время изменения */
        char time_buf[20];
        struct tm *timeinfo = localtime(&file_stat.st_mtime);
        strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", timeinfo);
        printf("%s ", time_buf);
    }
    
    /* Выводим имя файла */
    printf("%s", filename);
    
    /* Обрабатываем символические ссылки */
    if (S_ISLNK(file_stat.st_mode)) {
        char link_target[PATH_MAX];
        ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';
            printf(" -> %s", link_target);
        }
    }
    
    printf("\n");
}

/* Функция сравнения для сортировки имен файлов */
int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/* Функция для вывода содержимого каталога */
void list_directory(const char *path, Flags flags, int is_first_call) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("ls");
        return;
    }
    
    struct dirent *entry;
    char **entries = NULL;
    size_t entry_count = 0;
    size_t capacity = 100;
    
    /* Выделяем память для массива имен */
    entries = malloc(capacity * sizeof(char *));
    if (!entries) {
        perror("malloc");
        closedir(dir);
        return;
    }
    
    /* Читаем все записи каталога */
    while ((entry = readdir(dir)) != NULL) {
        /* Пропускаем специальные записи . и .. */
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        /* Пропускаем скрытые файлы (начинающиеся с .) */
        if (entry->d_name[0] == '.') {
            continue;
        }
        
        /* Увеличиваем массив если нужно */
        if (entry_count >= capacity) {
            capacity *= 2;
            char **new_entries = realloc(entries, capacity * sizeof(char *));
            if (!new_entries) {
                perror("realloc");
                break;
            }
            entries = new_entries;
        }
        
        /* Копируем имя файла */
        entries[entry_count] = malloc(strlen(entry->d_name) + 1);
        if (entries[entry_count]) {
            strcpy(entries[entry_count], entry->d_name);
            entry_count++;
        }
    }
    
    closedir(dir);
    
    /* Сортируем имена файлов */
    if (entry_count > 0) {
        qsort(entries, entry_count, sizeof(char *), compare_strings);
    }
    
    /* Выводим информацию о файлах */
    for (size_t i = 0; i < entry_count; i++) {
        print_file_info(path, entries[i], flags);
        
        /* Рекурсивный обход для подкаталогов если указан -R */
        if (flags.recursive) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entries[i]);
            
            struct stat st;
            if (lstat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
                printf("\n%s:\n", full_path);
                list_directory(full_path, flags, 0);
            }
        }
        
        free(entries[i]);
    }
    
    free(entries);
}

int main(int argc, char *argv[]) {
    Flags flags = {0, 0, 0};
    
    /* Обрабатываем аргументы командной строки */
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            /* Обрабатываем флаги */
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch (argv[i][j]) {
                    case 'R': flags.recursive = 1; break;
                    case 'l': flags.long_format = 1; break;
                    case 'g': flags.show_group = 1; break;
                    default:
                        fprintf(stderr, "ls: invalid option -- '%c'\n", argv[i][j]);
                        fprintf(stderr, "Usage: ls [-R] [-l] [-g]\n");
                        return 1;
                }
            }
        } else {
            fprintf(stderr, "ls: file arguments not supported in this implementation\n");
            return 1;
        }
    }
    
    /* Если указан -g, но не указан -l, включаем подробный вывод */
    if (flags.show_group && !flags.long_format) {
        flags.long_format = 1;
    }
    
    /* Выводим содержимое текущего каталога */
    list_directory(".", flags, 1);
    
    return 0;
}
