#ifndef _H_THREAD
#define _H_THREAD

#include "event_map.h"
#include "cpu_map.h"
#include "probe_buff.h"

struct thread {
	pthread_t		tid;
	struct probe_buff	*buff;
	struct  event_map 	*event_map;
};

struct thread *thread_init(struct probe_buff *,pid_t);

void *thread_loop(void *arg);

int thread_start(struct thread *);

void thread_get_counters(struct event_map *);

#endif
