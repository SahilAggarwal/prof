#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <pthread.h>

#include "event.h"
#include "output.h"
#include "shared.h"
#include "mmap_page.h"

extern struct wakeup wakeup;

int event_create_thread(struct event_list_node *event_node)
{
	if(pthread_create(&event_node->thread,NULL,event_loop_thread,event_node)) {
		fprintf(stderr,"Failed to start pthread\n");
		return -1;
	}
	return 0;
}

void *event_loop_thread(void *arg)
{
	struct event_list_node *event_node = (struct event_list_node *)arg;
	struct mmap_pages      *mmap_pages = event_node->mmap_pages;
	struct pollfd		pollfd[]   = { {mmap_pages->fd, POLLIN,0},
					       {wakeup.rd_fd, POLLIN | POLLHUP, 0}
					     };

	struct output_records out_records  = {  
						{write_output},
						event_node->e_open.attr,
						event_node->probe_buff,
						event_node->type
					     };
	while(!__sync_fetch_and_or(&wakeup.ctrlc,0)) {
		if(poll(pollfd, 2, -1) == -1) {
			fprintf(stderr,"Failed to start poll\n");
			continue;
		}
		mmap_pages_read(mmap_pages,&out_records.reader);	
	}
	event_read_counters(event_node);
}

void event_read_counters(struct event_list_node *event_node)
{
	struct read_format counters;
	int nread = read(event_node->mmap_pages->fd,&counters,sizeof(counters));

	if(event_node->type & SCHED_SWITCH)
		printf("Context Switches : %d\n",counters.nr_events);
	if(event_node->type & SYS_ENTER_OPEN)
		printf("Files opened     : %d\n",counters.nr_events);
}
