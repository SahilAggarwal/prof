#ifndef _H_EVENT
#define _H_EVENT

#include "event_list.h"

struct read_format {
	__u64 nr_events;
	__u64 time_enabled;
	__u64 time_running;
	__u64 id;
};

int event_create_thread(struct event_list_node *);

void *event_loop_thread(void *arg);

void event_read_counters(struct event_list_node *);

#endif
