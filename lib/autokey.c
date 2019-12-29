#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void autokeyerror(char* error_text) {
	printf("Critical error in autokey.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* AK_encode(char* pln, char* key) {
	unsigned char* res = (char *) malloc((strlen(pln) + 2) * sizeof (char));
	memset(res, '\0', (strlen(pln) + 2));

	char* k = key;
	for (int i = 0; pln[i]; i++) {
		res[i] = pln[i] + (*k - 'a');
		if (res[i] > 'z') res[i] -= 26;
		if (!*++k) k = pln;
	}

	return res;
}

char* AK_decode(char* enc, char* key) {
	unsigned char* res = (char *) malloc((strlen(enc) + 2) * sizeof (char));
	memset(res, '\0', (strlen(enc) + 2));

	char* k = key;
	for (int i = 0; enc[i]; i++) {
		res[i] = enc[i] - (*k - 'a');
		if (res[i] < 'a') res[i] += 26;
		if (!*++k) k = res;
	}
	return res;
}