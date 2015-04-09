#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <glib.h>

#include "thread_map.h"
#include "output.h"
#include "mmap_page.h"
#include "shared.h"

GHashTable *fd_to_type;
extern struct wakeup wakeup;

struct thread_map *
thread_map__init(struct cpu_map *cpu_map,struct probe_buff *buff,pid_t pid)
{
	struct thread_map *thread_map = malloc(sizeof(*thread_map) + 
					sizeof(struct thread)*cpu_map->nr);
	
	struct event_list_map *elist_map = event_list_map__init();

	fd_to_type = g_hash_table_new(g_int_hash,g_int_equal);
	thread_map->nr = cpu_map->nr;

	int i;
	for(i=0; i < thread_map->nr; i++) {
		if(thread_init(&thread_map->threads[i],elist_map, \
		   buff,cpu_map->cpus[i],pid)) {
			printf("Failed to init thread\n");
			return NULL;
		}
	}
	return thread_map;
}

int thread_map__start_threads(struct thread_map *thread_map) 
{
	int i;
	for(i=0; i< thread_map->nr; i++) {
		if(thread_start(&thread_map->threads[i])) {
			fprintf(stderr,"Failed to start thread\n");
			return -1;
		}
	}
	return 0;
}

static int
thread_init(struct thread *thread,struct event_list_map *elist_map, \
	    struct probe_buff *buff,int cpu, pid_t pid)
{
        thread->buff = buff;
        thread->event_map = event_map__init(elist_map,cpu,pid);
        if(thread->event_map == NULL) {
                fprintf(stderr,"Faield to init event map\n");
                return -1;
        }
        return 0;
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
				event_map->events[0].e_open.attr,
				thread->buff
			     };

	while(!__sync_fetch_and_or(&wakeup.ctrlc,0)) {
                if(poll(pollfd, event_map->nr + 1, -1) == -1) {
                        fprintf(stderr,"Failed to start poll\n");
                        continue;
                }
		int i=0;
		for(;i < event_map->nr; i++) {
			if(pollfd[i].revents & POLLIN) {
				int *index = g_hash_table_lookup(fd_to_type, &pollfd[i].fd);
				out.e_type = event_map->events[*index].type;
				mmap_pages_read(event_map->events[*index].mmap_pages,
						&out.reader);
			}
		}
        }
	
}

