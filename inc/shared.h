#ifndef _H_SHARED
#define _H_SHARED

#define USAGE_ERROR	usage()
#define MAX_NR_ARGS 10

#define ARR_SIZE(arr) 	(sizeof(arr)/sizeof(arr[0]))

#define NR_EVENTS ARR_SIZE(events)

struct wakeup {
	int 	ctrlc;
	int	rd_fd;
	int	wr_fd;
};

void usage();

int valid_opts(int,int,int);

void panic(char *);

char *join(char *, char *);

#endif
