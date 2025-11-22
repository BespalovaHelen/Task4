#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    	long lines;
    	long words;
    	long chars;
    	char *filename;
} FileStats;

/* Подсчёт статистики одного файла */
int count_file_stats(const char *filename, FileStats *stats) {
    	FILE *file = fopen(filename, "r");
    	if (!file) {
        	perror("wc");
        	return -1;
    	}
    
    	int in_word = 0;
    	char ch;
    
    	stats->lines = 0;
    	stats->words = 0;
    	stats->chars = 0;
    	stats->filename = (char *)filename;
    
    	while ((ch = fgetc(file)) != EOF) {
        	stats->chars++;
        
        	if (ch == '\n') {
            		stats->lines++;
        	}
        
        	if (isspace(ch)) {
            		if (in_word) {
                		stats->words++;
                		in_word = 0;
            		}
        	} else {
            		in_word = 1;
        	}
    	}
    
    	/* Если файл заканчивается словом без пробела */
    	if (in_word) {
        	stats->words++;
    	}
    
    	fclose(file);
    	return 0;
}

/* Вывод статистики одного файла */
void print_stats(const FileStats *stats) {
    	printf("%ld %ld %ld %s\n", 
           	stats->lines, 
           	stats->words, 
           	stats->chars, 
           	stats->filename);
}

/* Вывод итоговой статистики */
void print_total_stats(const FileStats *total, int file_count) {
    	if (file_count > 1) {
        	printf("%ld %ld %ld total\n", 
               		total->lines, 
               		total->words, 
               		total->chars);
    	}	
}

int main(int argc, char *argv[]) {
    	FileStats *files_stats = NULL;
    	FileStats total_stats = {0, 0, 0, "total"};
    	int file_count = 0;
    
    	/* Если аргументов нет, читаем из стандартного ввода */
    	if (argc == 1) {
        	FileStats stdin_stats = {0, 0, 0, ""};
        	int in_word = 0;
        	char ch;
        
        	while ((ch = getchar()) != EOF) {
            		stdin_stats.chars++;
            
            		if (ch == '\n') {
                		stdin_stats.lines++;
            		}
            
            		if (isspace(ch)) {
                		if (in_word) {
                    			stdin_stats.words++;
                    			in_word = 0;
                		}
            		} else {
                		in_word = 1;
            		}
        	}
        
        	/* Если ввод заканчивается словом без пробела */
        	if (in_word) {
            		stdin_stats.words++;
        	}
        
        	printf("%ld %ld %ld\n", 
               		stdin_stats.lines, 
               		stdin_stats.words, 
               		stdin_stats.chars);
        	return 0;
    	}
    
    	/* Выделяем память для статистики по файлам */
    	files_stats = malloc((argc - 1) * sizeof(FileStats));
    	if (!files_stats) {
        	perror("malloc");
        	return 1;
    	}
    
    	/* Обрабатываем каждый файл */
    	for (int i = 1; i < argc; i++) {
        	if (count_file_stats(argv[i], &files_stats[file_count]) == 0) {
            		/* Суммируем в общую статистику */
            		total_stats.lines += files_stats[file_count].lines;
            		total_stats.words += files_stats[file_count].words;
            		total_stats.chars += files_stats[file_count].chars;
            		file_count++;
        	}	
    	}
    
    	/* Выводим статистику по каждому файлу */
    	for (int i = 0; i < file_count; i++) {
        	print_stats(&files_stats[i]);
    	}
    
    	/* Выводим общую статистику если файлов несколько */
    	print_total_stats(&total_stats, file_count);
    
    	free(files_stats);
    	return 0;
}
