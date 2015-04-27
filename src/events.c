#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "events.h"

DEFINE_PRINT_FUNC(sample_tid,

	TP_PRINT("TID:%d PID:%d ",
                 data->tid,
                 data->pid
        )
);

DEFINE_PRINT_FUNC(sample_time,
	
	TP_PRINT(" TIME:%ld",
                 data->time
                )
);

DEFINE_PRINT_FUNC(sample_cpu,

	TP_PRINT(" CPU:%d",
                 data->cpu
                )
);

DEFINE_PRINT_FUNC(sample_raw_sched_switch,

	TP_PRINT(" SWITCH PrevComm:%s PrevPid:%d NextComm:%s NextPid:%d\n",
                 data->prev_comm,
                 data->prev_pid,
                 data->next_comm,
                 data->next_pid
                )
);

DEFINE_PRINT_FUNC(sample_raw_newtask,

	TP_PRINT(" NEWTASK Pid:%d Comm: %s\n",
                 data->pid,
                 data->comm
        )
);

DEFINE_PRINT_FUNC(sample_raw_enter_open,

	TP_PRINT(" ENTER_OPEN Mode:%d\n",
                 data->mode
        )
);

DEFINE_PRINT_FUNC(sample_raw_sys_enter,

	TP_PRINT(" SYS_ENTER Id:%d\n",
                 data->id
        )

);

DEFINE_PRINT_FUNC(sample_raw_enter_read,

	TP_PRINT(" ENTER_READ Fd:%d Count:%d\n",
                 data->fd,
                 data->count
        )
);


DEFINE_PRINT_FUNC(sample_raw_exit_read,

	TP_PRINT(" EXIT_READ Ret:%d\n",
                 data->ret
        )
);

DEFINE_PRINT_FUNC(sample_raw_enter_write,

	TP_PRINT(" ENTER_WRITE Fd:%d Count:%d\n",
                 data->fd,
                 data->count
        )

);

DEFINE_PRINT_FUNC(sample_raw_enter_lseek,

	TP_PRINT(" LSEEK Fd:%d Offset:%d Origin:%d\n",
                 data->fd,
                 data->offset,
                 data->origin
        )

);

DEFINE_PRINT_FUNC(sample_raw_enter_mmap,

        TP_PRINT(" MMAP Len:%d Fd:%d Offset:%d\n",
                 data->len,
                 data->fd,
                 data->offset
        )
);

DEFINE_PRINT_FUNC(sample_raw_sys_exit_brk,

        TP_PRINT(" EXIT_BRK\n"
        )
);

DEFINE_PRINT_FUNC(sample_raw_kmalloc,

        TP_PRINT(" KMALLOC BytesReq:%d BytesAlloc:%d\n",
                 data->bytes_req,
                 data->bytes_alloc
        )
);

DEFINE_PRINT_FUNC(sample_raw_kmem_cache,

        TP_PRINT(" KMEM_CACHE BytesReq:%d BytesAlloc:%d\n",
                 data->bytes_req,
                 data->bytes_alloc
        )
);

DEFINE_PRINT_FUNC(sample_raw_page_fault,

        TP_PRINT(" PAGE_FAULT Type:%s\n",
                 data->flag & VM_FAULT_MAJOR ? "MAJOR":"MINOR"
        )
);

DEFINE_PRINT_FUNC(sample_raw_page_alloc,

        TP_PRINT(" PAGE_ALLOC\n"
        )
);

DEFINE_PRINT_FUNC(sample_raw_page_alloc_zone,

        TP_PRINT(" PAGE_ALLOC_ZONE Order:%d MigrateType:%d\n",
                 data->order,
                 data->migrate_type
        )
);

DEFINE_PRINT_FUNC(sample_raw_page_free,

        TP_PRINT(" PAGE_FREE Order:%d\n",
                 data->order
        )
);

DEFINE_PRINT_FUNC(sample_raw_page_free_batched,

        TP_PRINT(" PAGE_FREE_BATCHED Order:%d\n",
                 data->order
        )
);

DEFINE_PRINT_FUNC(sample_raw_block_issue,

        TP_PRINT(" BLOCK_ISSUE Dev:%d,%d Nr_sectors:%d Rwbs:%s\n",
                 (unsigned int) data->dev >> 20,
                 (unsigned int) (data->dev & ( 1U << 20 ) - 1),
                 data->nr_sector,
                 data->rwbs
        )
);

DEFINE_PRINT_FUNC(sample_raw_block_insert,

        TP_PRINT(" BLOCK_INSERT Dev:%d,%d Nr_sectors:%d Rwbs:%s\n",
                 (unsigned int) data->dev >> 20,
                 (unsigned int) (data->dev & ( 1U << 20 ) - 1),
                 data->nr_sector,
                 data->rwbs
        )
);


func get_output_function(char *event, char **event_title) {
	if(strstr(event,"sched_switch")) {
		*event_title = "Sched Switches";
		return sample_raw_sched_switch_entry;
	}
	else if(strstr(event,"newtask")) {
		*event_title = "New Tasks";
		return sample_raw_newtask_entry;
	}
	else if(strstr(event,"sys_enter_open")) {
		*event_title = "File Opened";
		return sample_raw_enter_open_entry;
	}
	else if(strstr(event,"sys_enter/")) {
		*event_title = "Syscalls";
		return sample_raw_sys_enter_entry;
	}
	else if(strstr(event,"sys_enter_read")) {
		*event_title = "File Reads";
		return sample_raw_enter_read_entry;
	}
	else if(strstr(event,"sys_exit_read")) {
		*event_title = "Exit Reads";
		return sample_raw_exit_read_entry;
	}
	else if(strstr(event,"sys_enter_write")) {
		*event_title = "File Writes";
		return sample_raw_enter_write_entry;
	}
	else if(strstr(event,"sys_enter_lseek")) {
		*event_title = "Lseeks";
		return sample_raw_enter_lseek_entry;
	}
	else if(strstr(event,"sys_enter_mmap")) {
		*event_title = "Mmaps";
		return sample_raw_enter_mmap_entry;
	}
	else if(strstr(event,"sys_exit_brk")) {
		*event_title = "Exit Brk";
		return sample_raw_sys_exit_brk_entry;
	}
	else if(strstr(event,"kmalloc")) {
		*event_title = "Kmalloc";
		return sample_raw_kmalloc_entry;
	}
	else if(strstr(event,"kmem_cache_alloc")) {
		*event_title = "Kmem Cache";
		return sample_raw_kmem_cache_entry;
	}
	else if(strstr(event,"mm_page_fault_handle")) {
		*event_title = "Page Fault";
		return sample_raw_page_fault_entry;
	}
	else if(strstr(event,"mm_page_alloc/")) {
		*event_title = "Page Alloc";
		return sample_raw_page_alloc_entry;
	}
	else if(strstr(event,"mm_page_alloc_zone_locked")) {
		*event_title = "Page Zone";
		return sample_raw_page_alloc_zone_entry;
	}
	else if(strstr(event,"mm_page_free")) {
		*event_title = "Page Free";
		return sample_raw_page_free_entry;
	}
	else if(strstr(event,"mm_page_free_batched")) {
		*event_title = "Page Free Batched";
		return sample_raw_page_free_batched_entry;
	}
	else if(strstr(event,"block_rq_issue")) {
		*event_title = "Block Issue";
		return sample_raw_block_issue_entry;
	}
	else if(strstr(event,"block_rq_insert")) {
		*event_title = "Block Insert";
		return sample_raw_block_insert_entry;
	}
}
