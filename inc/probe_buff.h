#ifndef _H_PROBE_BUFF
#define _H_PROBE_BUFF

#include <semaphore.h>

struct probe_buff {
        int	                size;
        void                    *base;
	int			nwrite;
	int			fd;
        sem_t		        sem;

	int (*write)(struct probe_buff *,char *,int);
};

struct probe_buff *probe_buff__init();

int probe_buff__write(struct probe_buff *,char *,int);

int probe_buff__flush(struct probe_buff *);

int probe_buff__get_file();

#endif


