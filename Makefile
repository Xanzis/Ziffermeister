CC = gcc
RM = rm
MODS = lib/playfair.c lib/shift.c lib/railfence.c lib/vigenere.c lib/autokey.c lib/beaufort.c lib/porta.c lib/atbash.c

all: zmstr

zmstr: ziffermeister.c $(MODS)
	$(CC) -o zmstr ziffermeister.c lib/analysis.c $(MODS)

clean:
	$(RM) zmstr

debug: ziffermeister.c $(MODS)
	$(CC) -o zmstr ziffermeister.c lib/analysis.c $(MODS) -fsanitize=address

info: ziffermeister.c $(MODS)
	$(CC) -o zmstr -pg -O9 ziffermeister.c lib/analysis.c $(MODS)

opt: ziffermeister.c $(MODS)
	$(CC) -o zmstr -O9 ziffermeister.c lib/analysis.c $(MODS)