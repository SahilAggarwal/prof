#pragma once
#ifndef _H_OUTPUT
#define _H_OUTPUT

#include <linux/perf_event.h>
#include "buffer.h"
#include "probe_buff.h"

struct output {
	struct buf_reader 	reader;
	struct perf_event_attr 	*attr;
	struct probe_buff	*probe_buff;
	int			e_type;
};

struct trace_common {
        unsigned short  common_type;
        unsigned char   common_flags;
        unsigned char   common_preempt_count;
        int             common_pid;
        int             common_padding;
};

struct sched_switch {
	struct trace_common 	common;
	char	 		prev_comm[16];
	pid_t			prev_pid;
	int 			prev_prio;
	long			prev_state;
	char			next_comm[16];
	pid_t			next_pid;
	int			next_prio;
};

struct sched_wakeup {
	struct trace_common	common;
	char 			comm[16];
	pid_t			pid;
	int			prio;
	int			success;
	int			target_cpu;
};

struct sys_enter_open {
	struct trace_common 	common;
	int			nr;
	char			filename[8];
	long long		flags;
	long long		mode;
};

struct sys_enter {
	struct trace_common	common;
	long long		id;
	long long   		args[6];
};

struct sys_enter_read {
	struct trace_common	common;
	int			nr;
	long long		fd;
	char			buf[8];
	long long		count;
};

struct sys_exit_read {
	struct trace_common	common;
	int			nr;
	long long		ret;
};

struct sys_enter_write {
	struct trace_common	common;
	int 			nr;	
	long long		fd;	
	char			buf[8];
	long long		count;
};

struct sys_enter_lseek {
	struct trace_common	common;
	int			nr;
	long long		fd;
	long long		offset;
	long long 		origin;
};

struct getnameprobe {
	struct trace_common	common;
	long long		probe_func;
	long long		probe_ret_ip;
	char			filename[4];	
};

struct read_format {
        __u64 nr_events;
        __u64 time_enabled;
        __u64 time_running;
        __u64 id;
};

struct perf_record_sample_tid {
	__u32 pid;
	__u32 tid;
};

struct perf_record_sample_stream_id {
	__u64 stream_id;
};

struct perf_record_sample_cpu {
	__u32 cpu, res;
};

struct perf_record_sample_time {
	__u64 time;
};

struct perf_record_sample_raw {
	__u32 size;
	char data[];
};

__u64 write_output(void *buf, __u64 size, void *);

#endif
