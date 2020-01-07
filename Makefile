CC = gcc
RM = rm
MODS = lib/playfair.c lib/shift.c lib/railfence.c lib/vigenere.c lib/autokey.c lib/beaufort.c lib/porta.c lib/atbash.c lib/columnar.c lib/shuffle.c lib/trifid.c

all: zmstr

zmstr: ziffermeister.c $(MODS)
	$(CC) -o zmstr ziffermeister.c lib/analysis.c $(MODS) -lm

clean:
	$(RM) zmstr

debug: ziffermeister.c $(MODS)
	$(CC) -o zmstr ziffermeister.c lib/analysis.c $(MODS) -fsanitize=address -lm

undef: ziffermeister.c $(MODS)
	$(CC) -o zmstr ziffermeister.c lib/analysis.c $(MODS) -fsanitize=undefined -lm

info: ziffermeister.c $(MODS)
	$(CC) -o zmstr -pg -O9 ziffermeister.c lib/analysis.c $(MODS) -lm

opt: ziffermeister.c $(MODS)
	$(CC) -o zmstr -O9 ziffermeister.c lib/analysis.c $(MODS) -lm

gdb: ziffermeister.c $(MODS)
	$(CC) -o zmstr -g -O9 ziffermeister.c lib/analysis.c $(MODS) -lm