#pragma once
#ifndef _H_OUTPUT
#define _H_OUTPUT

#include <linux/perf_event.h>
#include "buffer.h"
#include "probe_buff.h"

struct output {
	struct buf_reader 	reader;
	struct perf_event_attr 	attr;
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
	int			prev_pid;
	int			next_pid;
	int			next_cpu;
	char			prev_prio;
	char 			prev_state;
	char			next_prio;
	char			next_state;
};

struct sched_wakeup {
	struct trace_common	common;
	char 			comm[16];
	pid_t			pid;
	int			prio;
	int			success;
	int			target_cpu;
};

struct sys_clone {
	struct trace_common	common;
	long long		ret_ip;
	long long		func;
	int			ret;
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

struct sys_enter_mmap {
	struct trace_common	common;
	int			nr;
	long long		addr;
	long long		len;
	long long		prot;
	long long		flags;
	long long		fd;
	long long		offset;
};

struct mm_page_alloc {
	struct trace_common	common;
	long long		page;
	int			order;
	int			gdp_flag;
	int			mig_type;
};

struct block_rq_issue {
	struct trace_common	common;
	int			dev;
	long long		sector;
	int			nr_sector;
	int			bytes;
	char			rwbs[8];
	char			comm[16];
	char 			cmd[4];
};

struct block_rq_complete {
	struct trace_common	common;
	int			dev;
	long long		sector;
	int			nr_sector;
	int			error;
	char			rwbs[8];
	char			cmd[4];
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
