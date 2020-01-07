#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Shuffles letters in groups of length key_len, according to the alphabetical order of the letters in the key
// Not really something anyone uses. Can safely exclude from most searches

void shuffleerror(char* error_text) {
	printf("Critical error in shuffle.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* SU_encode(char* pln, char* key_orig) {
	int len = strlen(pln);
	int key_len = strlen(key_orig);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);
	char key[key_len];
	strcpy(key, key_orig);
	key[key_len] = '\0';
	int col_order[key_len];
	for (int i = 0; i < key_len; i++) col_order[i] = i;

	// find the shuffled order
	int j, k, c;
	for (int i = 1; i < key_len; i++) {
		k = key[i];
		c = col_order[i];
		j = i - 1;
		while(j >= 0 && key[j] > k) {
			key[j + 1] = key[j];
			col_order[j + 1] = col_order[j];
			j--;
		}
		key[j + 1] = k;
		col_order[j + 1] = c;
	}

	// build the output

	int idx = 0;
	for (int chunk = 0; ((chunk + 1) * key_len) <= len; chunk++) {
		for (int i = 0; i < key_len; i++) {
			res[idx] = pln[(chunk * key_len) + col_order[i]];
			idx++;
		}
	}

	// The order of the last len%key_len characters is ambigous to decode anyway, so let's just append them to keep the length even

	while (idx < len) {
		res[idx] = pln[idx];
		idx++;
	}

	return res;
} 

char* SU_decode(char* enc, char* key_orig) {
	int len = strlen(enc);
	int key_len = strlen(key_orig);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);
	char key[key_len + 1];
	strcpy(key, key_orig);
	key[key_len] = '\0';
	int col_order[key_len];
	for (int i = 0; i < key_len; i++) col_order[i] = i;

	// find the shuffled order
	int j, k, c;
	for (int i = 1; i < key_len; i++) {
		k = key[i];
		c = col_order[i];
		j = i - 1;
		while(j >= 0 && key[j] > k) {
			key[j + 1] = key[j];
			col_order[j + 1] = col_order[j];
			j--;
		}
		key[j + 1] = k;
		col_order[j + 1] = c;
	}

	// build the output

	int idx = 0;
	for (int chunk = 0; ((chunk + 1) * key_len) <= len; chunk++) {
		for (int i = 0; i < key_len; i++) {
			res[(chunk * key_len) + col_order[i]] = enc[idx];
			idx++;
		}
	}

	// The order of the last len%key_len characters is ambigous to decode anyway, so let's just append them to keep the length even

	while (idx < len) {
		res[idx] = enc[idx];
		idx++;
	}
	return res;
}