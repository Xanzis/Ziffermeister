#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void columnarerror(char* error_text) {
	printf("Critical error in shift.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* CO_encode(char* pln, char* key) {
	int len = strlen(pln);
	int keylen = strlen(key);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);

	int table[keylen][(len / keylen) + 2];
	for (int i = 0; i < keylen; i++) memset(table[i], 0, ((len / keylen) + 2) * sizeof (int));

	for (int i = 0; )

	return res;
} 

char* CO_decode(char* enc, char* key) {
	int len = strlen(enc);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);

	for (int i = 0; i < len; i++) res[i] = 'a' + (((25 * (enc[i] - 'a')) + 25) % 26);

	return res;
}