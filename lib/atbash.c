#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void atbasherror(char* error_text) {
	printf("Critical error in atbash.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* AT_encode(char* pln, char* key) {
	int len = strlen(pln);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);

	for (int i = 0; i < len; i++) res[i] = 'a' + (((25 * (pln[i] - 'a')) + 25) % 26);

	return res;
} 

char* AT_decode(char* enc, char* key) {
	int len = strlen(enc);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);

	for (int i = 0; i < len; i++) res[i] = 'a' + (((25 * (enc[i] - 'a')) + 25) % 26);

	return res;
}