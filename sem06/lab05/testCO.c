#include <stdio.h>

int main()
{
	FILE *f1 = fopen("alphabet_out.txt", "w");
	FILE *f2 = fopen("alphabet_out.txt", "w");

	for (char c = 'a'; c <= 'z'; ++c) {
		if (c % 2) {
			fprintf(f1, "%c", c);
		} else {
			fprintf(f2, "%c", c);
		}
	}

	fclose(f1);
	fclose(f2);

	return 0;
}
