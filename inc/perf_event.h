#ifndef _H_PERF_EVENT
#define _H_PERF_EVENT

#include <linux/perf_event.h>
#include <sys/types.h>
#include "event_list.h"

static long perf_event_open(struct perf_event_attr *,pid_t,int,int,unsigned long);

struct mmap_pages *perf_event__open(struct event_open *);

#endif
