#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "perf_event.h"
#include "mmap_page.h"

static long
perf_event_open(struct perf_event_attr *attr,pid_t pid,
                int cpu,int group_id,unsigned long flags)
{
        return syscall(__NR_perf_event_open,attr,pid,cpu,group_id,flags);
}

struct mmap_pages *
perf_event__open(struct event_list_node *event)
{
	int fd = -1;

	event->attr.disabled 		= 1;
	event->attr.size 		= sizeof(event->attr);
	event->attr.type 		= PERF_TYPE_TRACEPOINT;
	event->attr.config		= event->id;
	event->attr.sample_period	= 1;
	event->attr.sample_type 	= PERF_SAMPLE_TIME 	|
					  PERF_SAMPLE_RAW  	|
					  PERF_SAMPLE_TID  	;

	fd = perf_event_open(&event->attr,event->pid,event->cpu	\
				,event->group_id,event->flags);

	if(fd < 0) {
		fprintf(stderr,"Failed to open event FD\n");
		return NULL;
	}
	printf("FD: %d\n",fd);
	struct mmap_pages *mmap_pages = NULL;
	mmap_pages = mmap_pages__new(fd,1);
	if(mmap_pages != NULL) {
		mmap_pages->attr = event->attr;
		fcntl(fd, F_SETFL, O_NONBLOCK);
	}
	return mmap_pages;
}
