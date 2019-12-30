#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

// Cypher includes

#include "lib/shift.h"
#include "lib/playfair.h"
#include "lib/railfence.h"
#include "lib/vigenere.h"
#include "lib/autokey.h"
#include "lib/beaufort.h"
#include "lib/porta.h"
#include "lib/atbash.h"
#include "lib/columnar.h"

#include "lib/analysis.h"

// Program mode defines

#define STR_KEY 1
#define NUM_KEY 2

// Cypher definitions - woo modularity!

/*
char* CYPHERS[] = {"shift", "playfair", "railfence", "vigenere", "autokey", "beaufort", "porta", "atbash", "columnar", NULL};
int KEY_TYPES[] = {NUM_KEY, STR_KEY, NUM_KEY, STR_KEY, STR_KEY, STR_KEY, STR_KEY, NUM_KEY, STR_KEY};
char* (*ENC_FUNCS[])(char*, char*) = {SH_encode, PF_encode, RF_encode, VG_encode, AK_encode, BF_encode, PT_encode, AT_encode, CO_encode};
char* (*DEC_FUNCS[])(char*, char*) = {SH_decode, PF_decode, RF_decode, VG_decode, AK_decode, BF_decode, PT_decode, AT_decode, CO_decode};
*/

char* CYPHERS[] = {"shift", "playfair", "railfence", "vigenere", "atbash", "columnar", NULL};
int KEY_TYPES[] = {NUM_KEY, STR_KEY, NUM_KEY, STR_KEY, NUM_KEY, STR_KEY};
char* (*ENC_FUNCS[])(char*, char*) = {SH_encode, PF_encode, RF_encode, VG_encode, AT_encode, CO_encode};
char* (*DEC_FUNCS[])(char*, char*) = {SH_decode, PF_decode, RF_decode, VG_decode, AT_decode, CO_decode};


// Key definitions

char* ZF_key_data;
char** ZF_key_locs;
int ZF_key_count;

/*
char* ZF_num_keys[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", NULL};
int ZF_num_key_count = 26;
*/
char* ZF_num_keys[] = {"3", "4", "5", "12", NULL};
int ZF_num_key_count = 4;

// Pointer to input text

char* ZF_text;

// Results and logging data

char* ZF_result = NULL;
char ZF_results[10][1024];
float ZF_scores[10] = {-1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX, -1 * FLT_MAX};
int ZF_logged = 0;

void ziffermeistererror(char* error_text) {
	printf("Critical error in ziffermeister.c\nError message follows:\n%s\n", error_text);
	exit(1);
}

char sanitize_char(char a) {
	if(!a) return '\0';
	if(a == '\n') return '\n';
	if(isalpha(a)) return tolower(a);
	return -1;
}

int num_funcs() {
	int i = 0;
	while (CYPHERS[++i]);
	return i;
}

int func_limit(int func_idx) {
	if (KEY_TYPES[func_idx] == STR_KEY) return ZF_key_count;
	else if (KEY_TYPES[func_idx] == NUM_KEY) return ZF_num_key_count;
}

char* get_key(int func_num, int key_idx) {
	if (KEY_TYPES[func_num] == STR_KEY) return ZF_key_locs[key_idx];
	else if (KEY_TYPES[func_num] == NUM_KEY) return ZF_num_keys[key_idx];
	else ziffermeistererror("Informative error message");
	return NULL;
}

void load_key_file(char* k) {
	if (access(k, F_OK) == -1) {
		// k is not a filename. Load up ZF_key_data and ZF_key_locs with a single key
		ZF_key_data = (char *) malloc((strlen(k) + 1) * sizeof (char));
		ZF_key_locs = (char **) malloc(2 * sizeof (char *));
		memset(ZF_key_data, '\0', strlen(k) + 1);
		strcpy(ZF_key_data, k);
		ZF_key_locs[0] = ZF_key_data;
		ZF_key_locs[1] = NULL;
		return;
	}

	// k is a valid filename. Load the file and record each newline-seperated key
	FILE *fp = fopen(k, "r");
	if (!fp) ziffermeistererror("Error opening file");

	fseek(fp, 0L, SEEK_END); 
    long int file_size = ftell(fp); 
    rewind(fp);

	ZF_key_data = (char *) malloc((file_size + 2) * sizeof (char));
	int data_i = 0, locs_i = 0;

	char c;
	int return_count = 0;
	while ((c = fgetc(fp)) != EOF) {
		c = sanitize_char(c);
		if (c != -1) {
			if (c == '\n') {
				ZF_key_data[data_i] = '\0';
				return_count++;
			}
			else ZF_key_data[data_i] = c;
			data_i++;
		}
	}
	ZF_key_data[data_i] = '\0';
	fclose(fp);

	ZF_key_locs = (char **) malloc((return_count + 2) * sizeof (char *));

	int wasnull = 1;
	for (int i = 0; i < data_i; i++) {
		if (wasnull && ZF_key_data[i]) {
			ZF_key_locs[locs_i] = ZF_key_data + i;
			locs_i++;
			wasnull = 0;
		}
		else if (ZF_key_data[i] == '\0') wasnull = 1;
	}
	ZF_key_locs[locs_i] = NULL;
	ZF_key_count = locs_i;
}

void free_keys() {
	free(ZF_key_data);
	free(ZF_key_locs);
}

void load_text(char* t) {
	if (access(t, F_OK) == -1) {
		// t is not a filename. Load up ZF_text with the given text
		ZF_text = (char *) malloc((strlen(t) + 1) * sizeof (char));
		memset(ZF_text, '\0', strlen(t) + 1);
		strcpy(ZF_text, t);
		return;
	}

	FILE *fp = fopen(t, "r");
	if (!fp) ziffermeistererror("Error opening text file");

	ZF_text = (char *) malloc(1 * sizeof (char));
	int text_idx = 0;

	char c = fgetc(fp);
	while ((c != EOF) && (c != '\n')) {
		c = sanitize_char(c);
		if (c != -1) {
			ZF_text[text_idx] = c;
			text_idx++;
			ZF_text = (char *) realloc(ZF_text, text_idx + 1);
		}
		c = fgetc(fp);
	}
	fclose(fp);

	ZF_text[text_idx] = '\0';
	printf("Text:\n%s\n", ZF_text);
}

float score_result(char* str) {
	// Can configure this with the various options in analysis.h to match use case
	return AN_quad(str);
}

float log_result(char* r) {
	// Checks if score is high enough to merit logging. If so, inserts into top results
	// ZF_logged indicates whether this logging occurred
	// Returns the score
	int res_idx = 0;
	float score = score_result(r);
	if (score > ZF_scores[9]) {
		ZF_logged = 1;
		while(score <= ZF_scores[res_idx]) res_idx++;
		for (int i = 9; i > res_idx; i--) {
			ZF_scores[i] = ZF_scores[i - 1];
			strcpy(ZF_results[i], ZF_results[i - 1]);
		}
		ZF_scores[res_idx] = score;
		strcpy(ZF_results[res_idx], r);
	}
	else ZF_logged = 0;

	return score;
}

void run_one(char* (*f)(char*, char*), char* key, int logging, int verbose) {
	// Run one function with the given key on ZF_text.
	// Sadly the logging and verbose inputs are a little ad-hoc for the function's separate use cases
	char* result = (*f)(ZF_text, key);
	if (logging) {
		float sc = log_result(result);
		if ((sc > AN_CUTOFF) && verbose) printf("func: %p key: %20s out: %s score: %f var: %f\n", f, key, result, sc, AN_ioc(result));
	}
	else printf("out: %s score: %f\n", result, score_result(result));
	free(result);
}

char* run_one_basic(char* (*f)(char*, char*), char* text, char* key, int ext, int free_at_end) {
	// Run one function with the given key on the text.
	// ext(ernal) and free_at_end indicate what to do with the result
	char* res;

	if (ext) {
		if (ZF_result) {
			free(ZF_result);
			ZF_result = NULL;
		}
		ZF_result = (*f)(text, key);
		res = ZF_result;
	}
	else res = (*f)(text, key);

	log_result(res);
	if ((!ext) && free_at_end) {
		free(res);
		res = NULL;
	}
	return res;
}

void run_all() {
	for (int func_num = 0; CYPHERS[func_num]; func_num++) {
		printf("Running cypher %s ...\n", CYPHERS[func_num]);

		if (KEY_TYPES[func_num] == STR_KEY) {
			for (int k = 0; ZF_key_locs[k]; k++) run_one(DEC_FUNCS[func_num], ZF_key_locs[k], 1, 1);
		}
		else if (KEY_TYPES[func_num] == NUM_KEY) {
			for (int k = 0; ZF_num_keys[k]; k++) run_one(DEC_FUNCS[func_num], ZF_num_keys[k], 1, 1);
		}
	}
	for (int i = 0; i < 10; i++) printf("Result #%d: %s score %f\n", i, ZF_results[i], ZF_scores[i]);
}

void run_all_2n() {
	for (int func1 = 0; CYPHERS[func1]; func1++) {
		if (KEY_TYPES[func1] == NUM_KEY) {
			for (int i = 0; i < ZF_num_key_count; i++) {
				run_one_basic(DEC_FUNCS[func1], ZF_text, ZF_num_keys[i], 1, 1);
				// stores output in ZF_result
				for (int func2 = 0; CYPHERS[func2]; func2++) {
					if (KEY_TYPES[func2] == NUM_KEY) {
						for (int j = 1; j < ZF_num_key_count; j++) {
							run_one_basic(DEC_FUNCS[func2], ZF_result, ZF_num_keys[j], 0, 1);
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < 10; i++) printf("Result #%d: %s score %f\n", i, ZF_results[i], ZF_scores[i]);
}

void run_all_2h() {
	int limit1, limit2;
	char* keya,* keyb;
	char* out;

	for (int func1 = 0; CYPHERS[func1]; func1++) {
		limit1 = (KEY_TYPES[func1] == STR_KEY) ? ZF_key_count : ZF_num_key_count;
		printf("Running primary cypher <%s> ...\n", CYPHERS[func1]);
		for (int i = 0; i < limit1; i++) {
			if (KEY_TYPES[func1] == STR_KEY) keya = ZF_key_locs[i];
			else                             keya = ZF_num_keys[i];
			run_one_basic(DEC_FUNCS[func1], ZF_text, keya, 1, 1);

			for (int func2 = 0; CYPHERS[func2]; func2++) {
				if (KEY_TYPES[func1] != KEY_TYPES[func2]) {
					limit2 = (KEY_TYPES[func2] == STR_KEY) ? ZF_key_count : ZF_num_key_count;
					for (int j = 0; j < limit2; j++) {
						if (KEY_TYPES[func2] == STR_KEY) keyb = ZF_key_locs[j];
						else                             keyb = ZF_num_keys[j];
						out = run_one_basic(DEC_FUNCS[func2], ZF_result, keyb, 0, 0);
						if (ZF_logged) {
							printf("   Logged: cypher <%12s> keys <%20s><%20s> result %s\n", CYPHERS[func2], keya, keyb, out);
						}
						free(out);
					}
				}
			}
		}
	}

	printf("\n");
	for (int i = 0; i < 10; i++) printf("Result #%d: %s score %f\n", i, ZF_results[i], ZF_scores[i]);
}

void run_all_2a() {
	int limit1, limit2;
	char* keya,* keyb;
	char* out;

	for (int func1 = 0; CYPHERS[func1]; func1++) {
		limit1 = (KEY_TYPES[func1] == STR_KEY) ? ZF_key_count : ZF_num_key_count;
		printf("Running primary cypher <%s> ...\n", CYPHERS[func1]);
		for (int i = 0; i < limit1; i++) {
			if (KEY_TYPES[func1] == STR_KEY) keya = ZF_key_locs[i];
			else                             keya = ZF_num_keys[i];
			run_one_basic(DEC_FUNCS[func1], ZF_text, keya, 1, 1);

			for (int func2 = 0; CYPHERS[func2]; func2++) {
				limit2 = (KEY_TYPES[func2] == STR_KEY) ? ZF_key_count : ZF_num_key_count;

				for (int j = 0; j < limit2; j++) {
					if (KEY_TYPES[func2] == STR_KEY) keyb = ZF_key_locs[j];
					else                             keyb = ZF_num_keys[j];
					out = run_one_basic(DEC_FUNCS[func2], ZF_result, keyb, 0, 0);

					if (ZF_logged) {
						printf("   Logged: cypher <%10s> keys <%15s><%15s> result %s\n", CYPHERS[func2], keya, keyb, out);
					}
					free(out);
				}
			}
		}
	}

	printf("\n");
	for (int i = 0; i < 10; i++) printf("Result #%d: %s score %f\n", i, ZF_results[i], ZF_scores[i]);
}

void run_all_n(int n) {
	if (n <= 1) ziffermeistererror("run_all_n: nope.");

	int funcs[n];
	int k_idxs[n];
	int limits[n];
	char *inter[n-1];
	memset(k_idxs, 0, n * sizeof (int));
	memset(funcs, 0, n * sizeof (int));
	memset(inter, 0, (n - 1) * sizeof (char *));
	int recompute = 0;
	char* out;
	char* k;

	int f_total = num_funcs();

	int carry = 0;

	for (int i = 0; i < n; i++) {
		limits[i] = func_limit(funcs[i]);
		// redundant I know but this is a foolproof intialization
	}

	// Strategy - recompute as little as possible. Cycle through all keys then function of lowest layer,
	//     Carrying upwards when the functions roll over. Recompute variable shows the most significant
	//     string that needs recomputing.

	while (!carry) {
		// compute to the second-to-last decrypt
		while(recompute < (n - 1)) {
			if (recompute == 0) {
				if (inter[0]) free(inter[0]);
				k = get_key(funcs[0], k_idxs[0]);
				inter[0] = (*DEC_FUNCS[funcs[0]])(ZF_text, k);
				recompute++;
			}
			else {
				if (inter[recompute]) free(inter[recompute]);
				k = get_key(funcs[recompute], k_idxs[recompute]);
				inter[recompute] = (*DEC_FUNCS[funcs[recompute]])(inter[recompute - 1], k);
				recompute++;
			}
		}

		// recompute is now n-1 (the last item to compute)
		// compute the final decode
		k = get_key(funcs[recompute], k_idxs[recompute]);
		out = run_one_basic(DEC_FUNCS[funcs[n-1]], inter[n - 2], k, 0, 0);
		if (ZF_logged) {
			printf("Logged: %s\n   From: ", out);
			for (int i = 0; i < n; i++) {
				printf("%2d f:<%s>k:<%s> ", i, CYPHERS[funcs[i]], get_key(funcs[i], k_idxs[i]));
			}
			printf("\n");
		}
		free(out);

		//increment and set recomputes
		carry = 1;
		
		for (int i = n-1; i >= 0; i--) {
			// carry into key increments
			if (carry) {
				k_idxs[i]++;
				recompute = i;
				if (k_idxs[i] >= limits[i]) {
					k_idxs[i] = 0;
					carry = 1;
				}
				else carry = 0;
			}

			if (carry) {
				// increment the function and roll over if function count is exceeded
				// set limit[i] to the appropriate value for the new function
				funcs[i]++;
				recompute = i; //redundant but improves readability
				if (funcs[i] >= f_total) {
					funcs[i] = 0;
					carry = 1;
				}
				else carry = 0;
				limits[i] = func_limit(funcs[i]);
			}
		}
		// uhhh that should be it. If that loop ends with carry = 1 the while loop wil end.
	}

	printf("\n");
	for (int i = 0; i < 10; i++) printf("Result #%d: %s score %f\n", i, ZF_results[i], ZF_scores[i]);
}

void run_func(int func_num, int mode) {
	for (int k = 0; ZF_key_locs[k]; k++) {
		printf("\nRunning cypher <%s> with key <%s> ...\n", CYPHERS[func_num], ZF_key_locs[k]);
		if (mode) run_one(ENC_FUNCS[func_num], ZF_key_locs[k], 0, 1);
		else run_one(DEC_FUNCS[func_num], ZF_key_locs[k], 0, 1);
	}
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: zmstr c t k (m)\n   c: the cypher to be used. zmstr runall t k will run all available cyphers\n   t: the filename of the plaintext/cyphertext file\n   k: either a key or a file containing a newline-seperated list of keys\n   m: encryption or decryption ('e'/'d') - do not use with runall\n");
		ziffermeistererror("OUT OF CHEESE ERROR PLEASE REBOOT FROM START");
	}

	if (strcmp(argv[0], "./zmstr")) ziffermeistererror("you blithering fool, you compiled to the wrong name");
	if (argc < 4) ziffermeistererror("Incorrect number of arguments supplied");
	
	load_text(argv[2]);
	printf("Text loaded\n");
	load_key_file(argv[3]);
	printf("Key(s) loaded\n");
	for (int i = 0; i < 10; i++) memset(ZF_results[i], '\0', 1024);

	char* result;
	float score;

	// 1st argument runall runs every cypher with every key on the supplied text
	if (!strcmp(argv[1], "runall")) run_all();

	// runall-2n runs two layers of every numerical cypher with every key
	else if (!strcmp(argv[1], "runall-2n")) run_all_2n();

	// runall-2h runs every str/num or num/str two-layer cypher pairing
	else if (!strcmp(argv[1], "runall-2h")) run_all_2h();

	// runall-2a runs every two-layer cypher pairing
	else if (!strcmp(argv[1], "runall-2a")) run_all_2a();

	// same thing but 4
	else if (!strcmp(argv[1], "runall-4")) run_all_n(4);

	// Any other 1st argument - searches for a cypher with that title and runs it
	else {
		if (argc != 5) ziffermeistererror("Incorrect number of arguments supplied");
		// Find the function number
		int func_num = 0;
		while (strcmp(argv[1], CYPHERS[func_num])) {
			func_num++;
			if(!CYPHERS[func_num]) ziffermeistererror("requested cypher is not supported");
		}
		printf("Cypher identified (funcnum %d)\n", func_num);
		run_func(func_num, (argv[4][0] == 'e') ? 1 : 0 );
	}

	free(ZF_text);
	free_keys();
	if (ZF_result) free(ZF_result);

	return 1;
}