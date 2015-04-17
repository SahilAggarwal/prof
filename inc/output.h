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
