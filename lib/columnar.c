#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void columnarerror(char* error_text) {
	printf("Critical error in columnar.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

char* CO_encode(char* pln, char* key_orig) {
	int len = strlen(pln);
	int key_len = strlen(key_orig);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);
	char key[key_len];
	strcpy(key, key_orig);
	key[key_len] = '\0';

	int col_len = (len / key_len) + 2;
	int table[key_len][col_len];
	// table is [col][row]

	int pos;

	int col_order[key_len];
	for (int i = 0; i < key_len; i++) col_order[i] = i;

	for (int i = 0; i < key_len; i++) {
		for (int j = 0; j < col_len; j++) {
			pos = i + (j * key_len); // index of letter that ends up in this table location
			if (pos < len) table[i][j] = pos;
			else table[i][j] = -1;
		}
	}

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

	// build the final output
	int res_idx = 0;
	int* cur_col;
	for (int col = 0; col < key_len; col++) {
		cur_col = table[col_order[col]];
		for (int row = 0; row < col_len; row++) {
			if (cur_col[row] != -1) {
				res[res_idx] = pln[cur_col[row]];
				res_idx++;
			}
		}
	}

	return res;
} 

char* CO_decode(char* enc, char* key_orig) {
	int len = strlen(enc);
	int key_len = strlen(key_orig);
	char* res = (char *) malloc((len + 1) * (sizeof (char)));
	memset(res, '\0', len + 1);
	char key[key_len];
	strcpy(key, key_orig);
	key[key_len] = '\0';

	int col_len = (len / key_len) + 2;
	int table[key_len][col_len];
	// table is [col][row]

	int pos;

	int col_order[key_len];
	for (int i = 0; i < key_len; i++) col_order[i] = i;

	for (int i = 0; i < key_len; i++) {
		for (int j = 0; j < col_len; j++) {
			pos = i + (j * key_len); // index of letter that ends up in this table location
			if (pos < len) table[i][j] = pos;
			else table[i][j] = -1;
		}
	}

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

	// build the final output
	int enc_idx = 0;
	int* cur_col;
	for (int col = 0; col < key_len; col++) {
		for (int row = 0; row < col_len; row++) {
			if (table[col_order[col]][row] != -1) {
				res[table[col_order[col]][row]] = enc[enc_idx];
				enc_idx++;
			}
		}
	}
	return res;
}