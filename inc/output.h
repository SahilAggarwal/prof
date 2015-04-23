#pragma once
#ifndef _H_OUTPUT
#define _H_OUTPUT

#include <linux/perf_event.h>
#include "buffer.h"
#include "probe_buff.h"

#define PRINT_COUNTER(type)							\
	printf("%-5s : %3d | %-20s : %d\n","CPU",cpu,#type,counters.nr_events);

struct output {
	struct buf_reader 	reader;
	struct perf_event_attr 	attr;
	struct probe_buff	*probe_buff;
	int			e_type;
};

struct read_format {
        __u64 nr_events;
        __u64 time_enabled;
        __u64 time_running;
        __u64 id;
};

struct perf_record_sample_raw {
	__u32 size;
	char data[];
};

__u64 write_output(void *buf, __u64 size, void *);

#endif
