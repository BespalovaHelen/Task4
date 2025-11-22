#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	char *cwd = NULL;
	long max;

	/*макс.длина пути*/
	max = pathconf(".", _PC_PATH_MAX); /* "." - текущая директория*/

	if (max == -1) {
		max = 4000;
	}

	/*динамический буфер*/
	cwd = malloc(max);

	if (cwd == NULL) {
		perror("malloc");
		return 1;
	}

	/*получение текущей директории*/
	if (getcwd(cwd, max) != NULL) {
		printf("%s\n", cwd);
	}
	else {
		perror("pwd");
		free(cwd);
		return 1;
	}

	free(cwd);
	return 0;
}

