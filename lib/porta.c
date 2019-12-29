#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char PT_alphabets[] = "nopqrstuvwxyzabcdefghijklmopqrstuvwxyznmabcdefghijklpqrstuvwxyznolmabcdefghijkqrstuvwxyznopklmabcdefghijrstuvwxyznopqjklmabcdefghistuvwxyznopqrijklmabcdefghtuvwxyznopqrshijklmabcdefguvwxyznopqrstghijklmabcdefvwxyznopqrstufghijklmabcdewxyznopqrstuvefghijklmabcdxyznopqrstuvwdefghijklmabcyznopqrstuvwxcdefghijklmabznopqrstuvwxybcdefghijklma";

char* PT_encode(char* pln, char* key) {
	unsigned char* res = (char *) malloc((strlen(pln) + 2) * sizeof (char));
	memset(res, '\0', (strlen(pln) + 2));

	char* k = key;
	for (int i = 0; pln[i]; i++) {
		res[i] = PT_alphabets[(((*k - 'a') / 2) * 26) + (pln[i] - 'a')];
		if (!*++k) k = key;
	}

	return res;
}


char* PT_decode(char* enc, char* key) {
	unsigned char* res = (char *) malloc((strlen(enc) + 2) * sizeof (char));
	memset(res, '\0', (strlen(enc) + 2));

	char* k = key;
	for (int i = 0; enc[i]; i++) {
		res[i] = PT_alphabets[(((*k - 'a') / 2) * 26) + (enc[i] - 'a')];
		if (!*++k) k = key;
	}
	
	return res;
}