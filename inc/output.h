#pragma once
#ifndef _H_OUTPUT
#define _H_OUTPUT

#include <linux/perf_event.h>
#include "buffer.h"
#include "probe_buff.h"

#define VM_FAULT_OOM    0x0001
#define VM_FAULT_SIGBUS 0x0002
#define VM_FAULT_MAJOR  0x0004
#define VM_FAULT_WRITE  0x0008  /* Special case for get_user_pages */
#define VM_FAULT_HWPOISON 0x0010        /* Hit poisoned small page */
#define VM_FAULT_HWPOISON_LARGE 0x0020  /* Hit poisoned large page. Index encoded in upper bits */

#define VM_FAULT_NOPAGE 0x0100  /* ->fault installed the pte, not return page */
#define VM_FAULT_LOCKED 0x0200  /* ->fault locked the returned page */
#define VM_FAULT_RETRY  0x0400  /* ->fault blocked, must retry */

#define VM_FAULT_HWPOISON_LARGE_MASK 0xf000 /* encodes hpage index for large hwpoison */

#define VM_FAULT_ERROR  (VM_FAULT_OOM | VM_FAULT_SIGBUS | VM_FAULT_HWPOISON | \
                         VM_FAULT_HWPOISON_LARGE)

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
};

struct sched_switch {
	struct trace_common 	common;
	char			prev_comm[16];
	int			prev_pid;
	int			prev_prio;
	long long		prev_state;
	char			next_comm[16];
	int			next_pid;
	int			next_prio;
};

struct task_newtask {
	struct trace_common	common;
	int			pid;
	char			comm[16];
	unsigned long		flags;
	short			oom_score;
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

struct page_fault_handle {
	struct trace_common 	common;
	int			flag;
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
