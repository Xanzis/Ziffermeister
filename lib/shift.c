#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shifterror(char* error_text) {
	printf("Critical error in shift.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* SH_encode(char* pln, char* key) {
	// key is the first character of key (0-26)

	int shift = atoi(key) % 26;

	int len = strlen(pln);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);

	strcpy(res, pln);

	for (int i = 0; i < len; i++) {
		res[i] += shift;
		if (res[i] > 'z') res[i] -= 26;
	}

	return res;
} 

char* SH_decode(char* enc, char* key) {
	int shift = atoi(key) % 26;

	int len = strlen(enc);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);

	strcpy(res, enc);

	for (int i = 0; i < len; i++) {
		res[i] -= shift;
		if (res[i] < 'a') res[i] += 26;
	}

	return res;
}