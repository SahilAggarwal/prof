#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

#include "thread.h"
#include "output.h"
#include "mmap_page.h"
#include "shared.h"

extern struct wakeup wakeup;

struct thread *
thread_init(struct probe_buff *buff,pid_t pid)
{
	struct thread *thread 		 = malloc(sizeof(struct thread));
	struct event_list_map *elist_map = event_list_map__init();
	if(elist_map == NULL) {
		fprintf(stderr,"Failed to init event list");
		return NULL;
	}
        thread->buff 			 = buff;
        thread->event_map 		 = event_map__init(elist_map,-1,pid);
        if(thread->event_map == NULL) {
                fprintf(stderr,"Failed to init event map\n");
                return NULL;
        }
        return thread;
}

int thread_start(struct thread *thread) 
{
        if(pthread_create(&thread->tid,NULL,thread_loop,thread)) {
                fprintf(stderr,"Failed to start pthread\n");
                return -1;
        }
        return 0;
}

void *thread_loop(void *arg)
{
	struct thread    *thread    = (struct thread *)arg;
	struct event_map *event_map = thread->event_map;
	struct pollfd    *pollfd    = malloc(sizeof(*pollfd) * (event_map->nr + 1));

	int i;
	for(i=0; i<event_map->nr; i++) {
		pollfd[i].fd     = event_map->events[i].mmap_pages->fd;
		pollfd[i].events = POLLIN;
	}
	pollfd[i].fd     = wakeup.rd_fd;
	pollfd[i].events = POLLIN | POLLHUP;

	struct output out = {
				{write_output},
			     };

	while(!__sync_fetch_and_or(&wakeup.ctrlc,0)) {
                if(poll(pollfd, event_map->nr + 1, -1) == -1) {
                        fprintf(stderr,"Failed to start poll\n");
                        continue;
                }
		int i=0;
		for(;i < event_map->nr; i++) {
			//if(pollfd[i].revents & POLLIN) {
				if(pollfd[i].fd == event_map->events[i].mmap_pages->fd) {
					out.e_type = event_map->events[i].e_type;
					out.attr   = event_map->events[i].e_open.attr;
					out.probe_buff   = thread->buff;
					mmap_pages_read(event_map->events[i].mmap_pages,
							&out.reader);
				}
				else {
                                	printf("Samething wrong, contact my developer\n");
                        	}
			//}
		}
        }
	sem_wait(&thread->buff->sem);
	thread_get_counters(event_map);
	sem_post(&thread->buff->sem);
}

void thread_get_counters(struct event_map *event_map)
{
	struct read_format counters;
	int i=0;
	for(; i < event_map->nr; i++) {
		int cpu = event_map->events[i].e_open.cpu;
		int fd  = event_map->events[i].mmap_pages->fd;
		read(fd,&counters,sizeof(counters));

		switch(event_map->events[i].e_type) {
	
		case SCHED_SWITCH       : printf("CPU : %3d | Sched Switches   : %d\n",
                                          cpu,counters.nr_events);
                                          break;
	
		case CONTEXT_SWITCH	: printf("CPU : %3d | Context Switches : %d\n",
				   	  cpu,counters.nr_events);
					  break;

		case SYS_ENTER_OPEN	: printf("CPU : %3d | Files opened     : %d\n",
				     	  cpu,counters.nr_events);
					  break;

		case SYS_ENTER_WRITE	: printf("CPU : %3d | Files writen     : %d\n",
					  cpu,counters.nr_events);
					  break;

		case SYS_ENTER_LSEEK	: printf("CPU : %3d | Lseeks           : %d\n",
					  cpu,counters.nr_events);
					  break;

		case SYS_ENTER_READ	: printf("CPU : %3d | Files Read       : %d\n",
					  cpu,counters.nr_events);
					  break;

		case SYS_EXIT_READ	: printf("CPU : %3d | Exit Reads       : %d\n",
					  cpu,counters.nr_events);
					  break;

		case SYS_CLONE		: printf("CPU : %3d | Clone            : %d\n",
					  cpu,counters.nr_events);
					  break;
		
		case SYS_ENTER		: printf("CPU : %3d | Syscalls         : %d\n",
					  cpu,counters.nr_events);
					  break;
		
		case SYS_ENTER_MMAP	: printf("CPU : %3d | Mmap             : %d\n",
					  cpu,counters.nr_events);
					  break;
		}
	}
}
