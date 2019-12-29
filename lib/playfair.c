#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define to_coor(IDX) ((PF_coor) {IDX / 5, IDX % 5})
#define to_idx(ROW, COL) ((ROW * 5) + COL)

typedef struct PF_coor PF_coor;
struct PF_coor {
	int row;
	int col;
};

typedef struct PF_pair PF_pair;
struct PF_pair {
	char a;
	char b;
};

char PF_s[25];
char* PF_square[] = {PF_s, PF_s + 5, PF_s + 10, PF_s + 15, PF_s + 20};
PF_coor PF_coor_lookup[26];
int PF_idx_lookup[25];

void playfairerror(char* error_text) {
	printf("Critical error in playfair.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

void build_square(char* key) {
	// Sets PF_square for a given key

	char notused[27] = "abcdefghi klmnopqrstuvwxyz";
	char cur;

	int idx = 0;
	//printf("key %s\n", key);

	for (int i = 0; key[i]; i++) {
		cur = key[i];
		if (cur == 'j') cur = 'i';
		if (strchr(notused, cur)) {
			// if the character hasn't been used yet
			PF_s[idx] = cur;
			notused[cur - 'a'] = ' ';

			PF_idx_lookup[cur - 'a'] = idx;

			idx++;
		}
	}

	// done entering key
	// fill out remaining alphabet

	for (int i = 0; i < 26; i++) {
		if (notused[i] != ' ') {
			PF_s[idx] = notused[i];
			PF_idx_lookup[i] = idx;
			idx++;
		}
	}
}

void print_square() {
	for (int row = 0; row < 5; row++) {
		for (int col = 0; col < 5; col ++) {
			printf("%c", PF_square[row][col]);
		}
		printf("\n");
	}
}

PF_pair run_pair(PF_pair pair, int sign) {
	// sign of 1 for encryption, -1 for decryption

	PF_coor acoor = to_coor(PF_idx_lookup[pair.a - 'a']);
	PF_coor bcoor = to_coor(PF_idx_lookup[pair.b - 'a']);

	PF_pair res;
	int temp;

	if (acoor.row == bcoor.row) {
		temp = acoor.col + sign;
		if (temp >= 5) temp -= 5;
		if (temp < 0) temp += 5;
		res.a = PF_s[to_idx(acoor.row, temp)];

		temp = bcoor.col + sign;
		if (temp >= 5) temp -= 5;
		if (temp < 0) temp += 5;
		res.b = PF_s[to_idx(bcoor.row, temp)];

		return res;
	}

	else if (acoor.col == bcoor.col) {
		temp = acoor.row + sign;
		if (temp >= 5) temp -= 5;
		if (temp < 0) temp += 5;
		res.a = PF_s[to_idx(temp, acoor.col)];

		temp = bcoor.row + sign;
		if (temp >= 5) temp -= 5;
		if (temp < 0) temp += 5;
		res.b = PF_s[to_idx(temp, bcoor.col)];

		return res;
	}

	else {
		res.a = PF_s[to_idx(acoor.row, bcoor.col)];
		res.b = PF_s[to_idx(bcoor.row, acoor.col)];
	}

	return res;
}

char* PF_encode(char* pln, char* key) {
	build_square(key);
	//print_square(square);

	char* res = (char *) malloc((strlen(pln) + 2) * sizeof (char));
	memset(res, '\0', (strlen(pln) + 2));

	PF_pair cur_pair, res_pair;

	int i = 0;
	int good = 1;

	while (good) {
		cur_pair.a = pln[i];
		if (!pln[i + 1]) cur_pair.b = 'x';
		else cur_pair.b = pln[i + 1];

		res_pair = run_pair(cur_pair, 1);
		res[i] = res_pair.a;
		res[i + 1] = res_pair.b;

		good = pln[i + 1] && pln[i + 2];
		i += 2;
	}

	return res;
}

char* PF_decode(char* enc, char* key) {
	build_square(key);
	//print_square(square);

	char* res = (char *) malloc((strlen(enc) + 2) * sizeof (char));
	memset(res, '\0', (strlen(enc) + 2));

	PF_pair cur_pair, res_pair;

	int good = 1;
	int i = 0;

	while (good) {
		cur_pair.a = enc[i];
		if (!enc[i + 1]) cur_pair.b = 'x';
		else cur_pair.b = enc[i + 1];

		res_pair = run_pair(cur_pair, -1);
		res[i] = res_pair.a;
		res[i + 1] = res_pair.b;

		if (!enc[i + 1]) good = 0;
		else if (!enc[i + 2]) good = 0;
		i += 2;
	}

	return res;
}