#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


#include "lib/analysis.h"

char* AL_text;

int AL_counts[26];
int AL_digraphs[676];

void analysererror(char* error_text) {
	printf("Critical error in analyser.c\nError message follows:\n");
	printf("%s\n", error_text);
	exit(1);
}

void print_counts() {
	int notdone = 1;
	printf("\n--- letter counts ---\n");
	while (notdone) {
		int max = 0;
		int maxi = -1;
		for (int i = 0; i < 26; i++) {
			if (AL_counts[i] > max) {
				max = AL_counts[i];
				maxi = i;
			}
		}
		if (maxi == -1) {
			notdone = 0;
		}
		else {
			printf("  %c:  %d\n", maxi + 'a', max);
			AL_counts[maxi] = 0;
		}
	}
	printf("---\n\n");
}

void print_digraphs() {
	int notdone = 1;
	printf("\n--- digraph counts ---\n");
	int unique = 0, total = 0;
	while (notdone) {
		int max = 0;
		int maxi = -1;
		for (int i = 0; i < 676; i++) {
			if (AL_digraphs[i] > max) {
				max = AL_digraphs[i];
				maxi = i;
			}
		}
		if (maxi == -1) {
			notdone = 0;
		}
		else {
			printf("  %c%c:  %d\n", (maxi / 26) + 'a', (maxi % 26) + 'a', max);
			total += max;
			AL_digraphs[maxi] = 0;
			unique++;
		}
	}
	printf("Total digraph count %d unique digraphs %d\n", total, unique);
	printf("---\n\n");
}

void set_counts(char* str) {
	char* c = str;
	while(*c) {
		AL_counts[*c -'a']++;
		c++;
	}
}

void set_digraphs(char* str, int stride) {
	// stride is stepover. 1 for continuous, 2 for individual pairs
	int a, b;
	int length = strlen(str);
	for (int i = 0; (i + 1) < length; i += stride) {
		a = str[i] - 'a';
		b = str[i + 1] - 'a';
		AL_digraphs[(a * 26) + b]++;
	}
}

char sanitize_char(char a) {
	if(!a) return '\0';
	if(a == '\n') return '\n';
	if(isalpha(a)) return tolower(a);
	return -1;
}

void load_text(char* t) {
	if (access(t, F_OK) == -1) {
		// t is not a filename. Load up ZF_text with the given text
		AL_text = (char *) malloc((strlen(t) + 1) * sizeof (char));
		memset(AL_text, '\0', strlen(t) + 1);
		strcpy(AL_text, t);
		return;
	}

	FILE *fp = fopen(t, "r");
	if (!fp) analysererror("Error opening file");

	AL_text = (char *) malloc(1 * sizeof (char));
	int text_idx = 0;

	char c = fgetc(fp);
	while ((c != EOF) && (c != '\n')) {
		c = sanitize_char(c);
		if (c != -1) {
			AL_text[text_idx] = c;
			text_idx++;
			AL_text = (char *) realloc(AL_text, text_idx + 1);
		}
		c = fgetc(fp);
	}
	fclose(fp);

	AL_text[text_idx] = '\0';
	printf("Text:\n%s\n", AL_text);
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		printf("Usage: ./anlys t\n");
		analysererror("OUT OF CHEESE ERROR PLEASE REBOOT FROM START");
	}
	load_text(argv[1]);
	printf("Text loaded\n%s\n", AL_text);

	set_counts(AL_text);
	set_digraphs(AL_text, 1);

	print_counts();
	print_digraphs();

	printf("English score: %f\n\n", AN_quad(AL_text));

	printf("Index of Coincidence: %f\n\n", AN_ioc(AL_text));

	printf("Periodicity analysis...\n");
	float maxout = 0;
	for (int p = 1; p < 26; p++) {
		float out = AN_periodicity(AL_text, p);
		if (out > maxout) maxout = out;
	}
	float scale = 50.0 / maxout;
	for (int p = 1; p < 26; p++) {
		float out = AN_periodicity(AL_text, p);
		printf("  period %2d score %10f ", p, out);
		for (int i = 0; i < (out * scale); i++) printf("|");
		printf("\n");
	}
}