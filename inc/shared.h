#ifndef _H_SHARED
#define _H_SHARED

#define USAGE_ERROR	usage()
#define MAX_NR_ARGS 10

#define NR_EVENTS ARR_SIZE(events)

struct wakeup {
	int 	ctrlc;
	int	rd_fd;
	int	wr_fd;
};

void usage();

int valid_opts(int,int);

void panic(char *);

char *join(char *, char *);

#endif
