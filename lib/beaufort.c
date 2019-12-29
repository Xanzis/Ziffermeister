#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void beauforterror(char* error_text) {
	printf("Critical error in vigenere.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* BF_encode(char* pln, char* key) {
	unsigned char* res = (char *) malloc((strlen(pln) + 2) * sizeof (char));
	memset(res, '\0', (strlen(pln) + 2));

	char* k = key;
	for (int i = 0; pln[i]; i++) {
		res[i] = 'a' + ((*k - 'a') - (pln[i] - 'a'));
		if (res[i] < 'a') res[i] += 26;
		if (!*++k) k = key;
	}

	return res;
}

char* BF_decode(char* enc, char* key) {
	unsigned char* res = (char *) malloc((strlen(enc) + 2) * sizeof (char));
	memset(res, '\0', (strlen(enc) + 2));

	char* k = key;
	for (int i = 0; enc[i]; i++) {
		res[i] = 'a' + ((*k - 'a') - (enc[i] - 'a'));
		if (res[i] < 'a') res[i] += 26;
		if (!*++k) k = key;
	}
	return res;
}