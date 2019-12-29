#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void railfenceerror(char* error_text) {
	printf("Critical error in railfence.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* RF_encode(char* pln, char* key) {
	int rail_count = atoi(key);
	int length = strlen(pln);
	char* res = (char *) malloc((length + 1) * sizeof (char));
	memset(res, '\0', (length + 1));

	char* pln_cpy = (char *) malloc((length + 1) * sizeof (char));
	memset(pln_cpy, '\0', (length + 1));
	strcpy(pln_cpy, pln);

	int a, b;
	int period = (2 * rail_count) - 2;

	if (rail_count == 1) {
		free(pln_cpy);
		strcpy(res, pln);
		return res;
	}

	for (int rail = 0; rail < rail_count; rail++) {
		for (int i = 0; (i - rail) < length; i += period) {
			a = i - rail;
			b = i + rail;

			if (a >= 0) {
				res[strlen(res)] += pln_cpy[a];
				pln_cpy[a] = '\0';
			}
			if (b < length) {
				res[strlen(res)] += pln_cpy[b];
				pln_cpy[b] = '\0';
			}
		}
	}

	free(pln_cpy);

	return res;
}


char* RF_decode(char* enc, char* key) {
	int rail_count = atoi(key);
	int length = strlen(enc);
	char* res = (char *) malloc((length + 1) * sizeof (char));
	memset(res, '\0', (length + 1));

	if (rail_count == 1) {
		strcpy(res, enc);
		return res;
	}

	int a, b;
	int period = (2 * rail_count) - 2;

	int enc_idx = 0;

	for (int rail = 0; rail < rail_count; rail++) {
		for (int i = 0; (i - rail) < length; i += period) {
			a = i - rail;
			b = i + rail;

			if (a >= 0) {
				if (!res[a]){
					res[a] = enc[enc_idx];
					enc_idx++;
				}
			}
			if (b < length) {
				if (!res[b]){
					res[b] = enc[enc_idx];
					enc_idx++;
				}
			}
		}
	}

	if (strlen(res) != length) railfenceerror("invalid railfence input");

	return res;
}