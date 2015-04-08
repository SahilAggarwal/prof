#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (void) {
        int n = 0;
        char *p;
	int i = 0;
	sleep(2);
	if ((p = malloc(4096*5)) == NULL) {
		printf("malloc failure after %d MiB\n", n);
		return 0;
	}
	memset (p, 0, (4096*5));
}
