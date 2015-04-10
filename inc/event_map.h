#ifndef _H_EVENT_LIST
#define _H_EVENT_LIST
#define DEBUGFS_EVENTS_PATH "/sys/kernel/debug/tracing/events/"

#include <linux/perf_event.h>
#include "list.h"

#define SCHED_WAKEUP    1<<0
#define SCHED_SWITCH    1<<1
#define SYS_ENTER_OPEN  1<<2
#define SYS_ENTER_READ  1<<3
#define SYS_EXIT_READ	1<<4
#define SYS_ENTER_WRITE	1<<5
#define SYS_ENTER_LSEEK 1<<6


struct event_open {
        __u64                   id;
        int                     cpu;
        int                     group_id;
        unsigned long           flags;
        pid_t                   pid;
	struct perf_event_attr  *attr;
};

struct event {
	struct event_open       e_open;
	int                     type;
	struct mmap_pages	*mmap_pages;
	struct probe_buff	*probe_buff;
};

struct event_map {
	int 		nr;
	struct event 	events[];
};

struct event_list {
	struct 	list_head list;
	int  	id;
	int 	type;
};

struct event_list_map {
        int 		  nr;
        struct event_list *elist;
};


FILE *event_map__outfile();

struct event_map *event_map__init(struct event_list_map *,int, pid_t);

int event_map__open_events(struct event_map *);

int event__get_id(char *);

struct event_list_map *event_list_map__init();

#endif
