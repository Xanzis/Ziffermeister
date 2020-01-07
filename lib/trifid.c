#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char TR_cube[27];
int TR_lookup[27];

void trifiderror(char* error_text) {
	printf("Critical error in trifid.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

void build_cube(char* key) {
	char notused[27] = "abcdefghijklmnopqrstuvwxyz+";
	char cur;
	int idx = 0;

	for (int i = 0; key[i]; i++) {
		cur = key[i];
		if (strchr(notused, cur)) {
			TR_cube[idx] = cur;
			notused[cur - 'a'] = ' ';
			TR_lookup[cur - 'a'] = idx;
			idx++;
		}
	}

	// fill out remaining alphabet
	for (int i = 0; i < 27; i++) {
		if (notused[i] != ' ') {
			TR_cube[idx] = notused[i];
			TR_lookup[i] = idx;
			idx++;
		}
	}
}

char* TR_encode(char* pln, char* key) {
	int len = strlen(pln);
	int outlen = len * 3;
	char* res = (char *) malloc((outlen + 1) * sizeof (char));
	memset(res, '\0', outlen + 1);

	build_cube(key);

	int idx = 0;
	for (int i = 0; i < len; i++) {
		int loc = TR_lookup[pln[i] - 'a'];
		res[idx + 2] = 'a' + (loc % 3);
		loc /= 3;
		res[idx + 1] = 'a' + (loc % 3);
		loc /= 3;
		res[idx] = 'a' + (loc % 3);
		idx += 3;
	}

	return res;
}

char* TR_decode(char* enc, char* key) {
	int len = strlen(enc);
	int outlen = len / 3;
	char* res = (char *) malloc((outlen + 1) * sizeof (char));
	memset(res, '\0', outlen + 1);

	build_cube(key);

	int idx = 0;
	for (int i = 0; (i + 2) < len; i += 3) {
		int loc = ((enc[i] - 'a') % 3) * 9;
		loc += ((enc[i + 1] - 'a') % 3) * 3;
		loc += (enc[i + 2] - 'a') % 3;

		res[idx] = TR_cube[loc];
		idx++;
	}

	return res;
}