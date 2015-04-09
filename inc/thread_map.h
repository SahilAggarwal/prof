#ifndef _H_THREAD_MAP
#define _H_THREAD_MAP

#include "event_map.h"
#include "cpu_map.h"
#include "probe_buff.h"

struct thread {
	pthread_t		tid;
	struct probe_buff	*buff;
	struct  event_map 	*event_map;
};

struct thread_map {
	int 	nr;
	struct  thread	threads[];
};

struct thread_map *
thread_map__init(struct cpu_map *, struct probe_buff *,pid_t);

static int thread_init(struct thread *, struct event_list_map * ,\
			struct probe_buff *, int, pid_t);

int thread_map__start_threads(struct thread_map *);

void *thread_loop(void *arg);

int *int_dup(int);

int thread_start(struct thread *);

#endif
