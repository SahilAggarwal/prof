#ifndef _H_EVENT_LIST
#define _H_EVENT_LIST

#define DEBUGFS_EVENTS_PATH "/sys/kernel/debug/tracing/events/"

#include <linux/perf_event.h>
#include "list.h"

#include "probe.h"

struct event_list_node {
	struct list_head	list;
	struct mmap_pages	*mmap_pages;
	int	   		type;
	__u64			id;
	int			cpu;
	int			group_id;
	unsigned long		flags;
	pid_t			pid;
	struct perf_event_attr	attr;
};


struct event_list_node *event_list__init(pid_t,int);

int event_list__open_events(struct event_list_node *);

int event_list_get_id(char *);

#endif
