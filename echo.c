#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int escapes = 0; /* флаг -e */
	int no_n = 0; /* флаг -n */
	int disable_escapes = 0; /* флаг -E */

	/*обработка флагов*/
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(argv[i], "-n") == 0) {
				no_n = 1;
			}
			else if (strcmp(argv[i], "-e") == 0) {
				escapes = 1;
			}
			else if (strcmp(argv[i], "-E") == 0) {
                                disable_escapes = 1;
			}
		}
	}

	if (disable_escapes) {
		escapes = 0;
	}

	/*выводим аргументы*/
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') continue;

		if (escapes) {
			char *arg = argv[i];
			int stop_output = 0; /* для \c */

			while (*arg && !stop_output) {
				if (*arg == '\\' && *(arg + 1)) {
					switch (*(arg + 1)) {
						case 'n': putchar('\n'); break; /*перевод строки*/
						case 't': putchar('\t'); break; /*табуляция*/
						case 'a': putchar('\a'); break; /* звуковой сигнал (BEL)*/
                        			case 'b': putchar('\b'); break; /*backspace*/
						case 'c': stop_output = 1; break; /*остановка вывода*/
						case 'e': putchar('\x1B'); break; /*escape-символ*/
                        			case 'f': putchar('\f'); break; /*прогон страницы*/
                                		case 'r': putchar('\r'); break; /*возврат каретки*/
                                          	case 'v': putchar('\v'); break; /*вертикальная табуляция*/		
						case '\\': putchar('\\'); break; /*один слэш*/
						default: putchar(*arg); putchar(*(arg + 1)); break;
					}
					arg += 2;
					if (stop_output) break; /* прерываем если встретили \c */
				} else {
					putchar(*arg);
					arg++;
				}
			}
			if (stop_output) {
                		if (!no_n) {
                    			printf("\n");
                		}
                		return 0;
            		}
		} 
		else {
			printf("%s", argv[i]);
		}

		if (i < argc - 1) {
			printf(" ");
		}
	}

	if (!no_n) {
		printf("\n");
	}

	return 0;
}
