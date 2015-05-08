 #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syscallent.h"

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

DEFINE_PRINT_FUNC(sched_switch,

	TP_PRINT(" SWITCH PrevComm:%s PrevPid:%d NextComm:%s NextPid:%d\n",
                 data->prev_comm,
                 data->prev_pid,
                 data->next_comm,
                 data->next_pid
                )
);

DEFINE_PRINT_FUNC(newtask,

	TP_PRINT(" NEWTASK Pid:%d Comm: %s\n",
                 data->pid,
                 data->comm
        )
);

DEFINE_PRINT_FUNC(enter_open,

	TP_PRINT(" ENTER_OPEN Mode:%d\n",
                 data->mode
        )
);

DEFINE_PRINT_FUNC(getname,

	TP_PRINT(" Filename: %s\n",
		get_str(arg)
	)
);

DEFINE_PRINT_FUNC(sys_enter,

	TP_PRINT(" SYS_ENTER Id:%d -> %s\n",
                 data->id,
		 syscalls[data->id] 
        )

);

DEFINE_PRINT_FUNC(enter_read,

	TP_PRINT(" ENTER_READ Fd:%d Count:%d\n",
                 data->fd,
                 data->count
        )
);


DEFINE_PRINT_FUNC(exit_read,

	TP_PRINT(" EXIT_READ Ret:%d\n",
                 data->ret
        )
);

DEFINE_PRINT_FUNC(enter_write,

	TP_PRINT(" ENTER_WRITE Fd:%d Count:%d\n",
                 data->fd,
                 data->count
        )

);

DEFINE_PRINT_FUNC(enter_lseek,

	TP_PRINT(" LSEEK Fd:%d Offset:%d Origin:%d\n",
                 data->fd,
                 data->offset,
                 data->origin
        )

);

DEFINE_PRINT_FUNC(enter_mmap,

        TP_PRINT(" MMAP Len:%d Fd:%d Offset:%d\n",
                 data->len,
                 data->fd,
                 data->offset
        )
);

DEFINE_PRINT_FUNC(sys_exit_brk,

        TP_PRINT(" EXIT_BRK\n"
        )
);

DEFINE_PRINT_FUNC(kmalloc,

        TP_PRINT(" KMALLOC BytesReq:%d BytesAlloc:%d\n",
                 data->bytes_req,
                 data->bytes_alloc
        )
);

DEFINE_PRINT_FUNC(kmem_cache,

        TP_PRINT(" KMEM_CACHE BytesReq:%d BytesAlloc:%d\n",
                 data->bytes_req,
                 data->bytes_alloc
        )
);

DEFINE_PRINT_FUNC(page_fault,

        TP_PRINT(" PAGE_FAULT Type:%s\n",
                 data->ret & VM_FAULT_MAJOR ? "MAJOR":"MINOR"
        )
);

DEFINE_PRINT_FUNC(page_alloc,

        TP_PRINT(" PAGE_ALLOC\n"
        )
);

DEFINE_PRINT_FUNC(page_alloc_zone,

        TP_PRINT(" PAGE_ALLOC_ZONE Order:%d MigrateType:%d\n",
                 data->order,
                 data->migrate_type
        )
);

DEFINE_PRINT_FUNC(page_free,

        TP_PRINT(" PAGE_FREE Order:%d\n",
                 data->order
        )
);

DEFINE_PRINT_FUNC(page_free_batched,

        TP_PRINT(" PAGE_FREE_BATCHED Order:%d\n",
                 data->order
        )
);

DEFINE_PRINT_FUNC(page_swap_in,

	TP_PRINT(" SWAP_IN Ret:%d\n",
		 data->ret
	)
);

DEFINE_PRINT_FUNC(page_swap_out,

        TP_PRINT(" SWAP_OUT Ret:%d Pfn:%lu\n",
		 data->pfn,
                 data->ret
        )
);

DEFINE_PRINT_FUNC(block_issue,

        TP_PRINT(" BLOCK_ISSUE Dev:%d,%d Nr_sectors:%d Rwbs:%s\n",
                 (unsigned int) data->dev >> 20,
                 (unsigned int) (data->dev & ( 1U << 20 ) - 1),
                 data->nr_sector,
                 data->rwbs
        )
);

DEFINE_PRINT_FUNC(block_insert,

        TP_PRINT(" BLOCK_INSERT Dev:%d,%d Nr_sectors:%d Rwbs:%s Comm: %s\n",
                 (unsigned int) data->dev >> 20,
                 (unsigned int) (data->dev & ( 1U << 20 ) - 1),
                 data->nr_sector,
                 data->rwbs,
		 data->comm
        )
);

DEFINE_PRINT_FUNC(net_dev_queue,

	TP_PRINT(" DEV_QUEUE Skb: %p Len:%d\n",
		 data->buff,
		 data->len
	)
);

DEFINE_PRINT_FUNC(net_dev_xmit,

        TP_PRINT(" DEV_XMIT Skb: %p Len:%d Ret:%d\n",
                 data->buff,
                 data->len,
		 data->rc
        )
);

DEFINE_PRINT_FUNC(netif_receive_skb,

	TP_PRINT(" DEV_RECV Skb: %p Len: %d\n",
		 data->buff,
		 data->len
	)
);

DEFINE_PRINT_FUNC(netif_rx,

	TP_PRINT(" DEV_RX Skb: %p Len: %d\n",
                 data->buff,
                 data->len
        )
);

DEFINE_PRINT_FUNC(sys_enter_recvfrom,

	TP_PRINT(" RECVFROM Fd:%d Size:%d\n",
		 data->fd,
		 data->size
	)
);

DEFINE_PRINT_FUNC(sys_enter_sendto,

        TP_PRINT(" SENDTO Fd:%d Len:%d\n",
                 data->fd,
                 data->len
        )
);




func get_output_function(char *event, char **event_title) {
	if(strstr(event,"sched_switch")) {
		*event_title = "Sched Switches";
		return sched_switch_entry;
	}
	else if(strstr(event,"newtask")) {
		*event_title = "New Tasks";
		return newtask_entry;
	}
	else if(strstr(event,"sys_enter_open")) {
		*event_title = "File Opened";
		return enter_open_entry;
	}
	else if(strstr(event,"sys_enter/")) {
		*event_title = "Syscalls";
		return sys_enter_entry;
	}
	else if(strstr(event,"sys_enter_read")) {
		*event_title = "File Reads";
		return enter_read_entry;
	}
	else if(strstr(event,"sys_exit_read")) {
		*event_title = "Exit Reads";
		return exit_read_entry;
	}
	else if(strstr(event,"sys_enter_write")) {
		*event_title = "File Writes";
		return enter_write_entry;
	}
	else if(strstr(event,"sys_enter_lseek")) {
		*event_title = "Lseeks";
		return enter_lseek_entry;
	}
	else if(strstr(event,"sys_enter_mmap")) {
		*event_title = "Mmaps";
		return enter_mmap_entry;
	}
	else if(strstr(event,"sys_exit_brk")) {
		*event_title = "Exit Brk";
		return sys_exit_brk_entry;
	}
	else if(strstr(event,"kmalloc")) {
		*event_title = "Kmalloc";
		return kmalloc_entry;
	}
	else if(strstr(event,"kmem_cache_alloc")) {
		*event_title = "Kmem Cache";
		return kmem_cache_entry;
	}
	else if(strstr(event,"mm_page_fault_handle")) {
		*event_title = "Page Fault";
		return page_fault_entry;
	}
	else if(strstr(event,"mm_page_alloc/")) {
		*event_title = "Page Alloc";
		return page_alloc_entry;
	}
	else if(strstr(event,"mm_page_alloc_zone_locked")) {
		*event_title = "Page Zone";
		return page_alloc_zone_entry;
	}
	else if(strstr(event,"mm_page_free")) {
		*event_title = "Page Free";
		return page_free_entry;
	}
	else if(strstr(event,"mm_page_free_batched")) {
		*event_title = "Page Free Batched";
		return page_free_batched_entry;
	}
	else if(strstr(event,"block_rq_issue")) {
		*event_title = "Block Issue";
		return block_issue_entry;
	}
	else if(strstr(event,"block_rq_insert")) {
		*event_title = "Block Insert";
		return block_insert_entry;
	}
	else if(strstr(event,"page_swap_in")) {
		*event_title = "Swap In";
		return page_swap_in_entry;
	}
	else if(strstr(event,"page_swap_out")) {
		*event_title = "Swap Out";
		return page_swap_out_entry;
	}
	else if(strstr(event,"net_dev_queue")) {
		*event_title = "Skb Queue";
		return net_dev_queue_entry;
	}
	else if(strstr(event,"net_dev_xmit")) {
		*event_title = "Skb Xmit";
		return net_dev_xmit_entry;
	}
	else if(strstr(event,"netif_receive_skb")) {
		*event_title = "Skb Receive";
		return netif_receive_skb_entry;
	}
	else if(strstr(event,"netif_rx")) {
		*event_title = "Skb Rx";
		return netif_rx_entry;
	}
	else if(strstr(event,"sys_enter_recvfrom")) {
		*event_title = "Recvfrom";
		return sys_enter_recvfrom_entry;
	}
	else if(strstr(event,"sys_enter_sendto")) {
		*event_title = "Sendto";
		return sys_enter_sendto_entry;
	}
	else if(strstr(event,"getname")) {
		*event_title = "Getname";
		return getname_entry;
	}
}
