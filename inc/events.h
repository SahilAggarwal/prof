#ifndef _H_EVENTS
#define _H_EVENTS

#define __field(type, item)		type item;
#define __array(type, item, len)	type item[len];

#define TP_STRUCT(args...) 		args
#define TP_PRINT(args...)		args

#define TP_STRUCT_COMMON					\
	__field( unsigned short ,  common_type 	        )	\
        __field( unsigned char  ,  common_flags         )	\
        __field( unsigned char  ,  common_preempt_count )	\
        __field( int            ,  common_pid		)	\

#define DEFINE_EVENT(name,tstruct,p_args)	 		\
	struct name {						\
		tstruct						\
	};							\
								\
	void name##_entry(void **arg,char *str) {		\
		struct name *data = (struct name *)*arg;	\
		sprintf(str,p_args);				\
		*arg += sizeof(struct name);			\
	}
	


DEFINE_EVENT(sample_tid,
	
	TP_STRUCT( 
		__field( __u32, tid )
		__field( __u32, pid )	
	),	

	TP_PRINT("TID:%d PID:%d ",
		 data->tid,
	         data->pid
	)
);

DEFINE_EVENT(sample_time,

	TP_STRUCT(
		__field( __u64, time )
	),

	TP_PRINT(" TIME:%ld",
		 data->time
		)
);

DEFINE_EVENT(sample_cpu,
	
	TP_STRUCT(
		__field( __u32, cpu )
		__field( __u32, res )
	),
	
	TP_PRINT(" CPU:%d",
		 data->cpu
		)
);

DEFINE_EVENT(sample_raw_sched_switch,

	TP_STRUCT(
		TP_STRUCT_COMMON
		
		__array( char		, prev_comm ,	16	)
		__field( int		, prev_pid 		)
		__field( int            , prev_prio 		)
		__field( long long      , prev_state		)
		__array( char           , next_comm ,   16      )
                __field( int            , next_pid              )
                __field( int            , next_prio             )
	),

	TP_PRINT(" SWITCH PrevComm:%s PrevPid:%d NextComm:%s NextPid:%d\n",
		 data->prev_comm,
		 data->prev_pid,
		 data->next_comm,
		 data->next_pid
		)
);

DEFINE_EVENT(sample_raw_newtask,
	
	TP_STRUCT(
		TP_STRUCT_COMMON
		
		__field( int            , pid	              )
		__array( char           , comm 	     ,  16    )
		__field( unsigned long  , flags	     	      )
		__field( short          , oom_score           )
	),

	TP_PRINT(" NEWTASK Pid:%d Comm: %s\n",
		 data->pid,
		 data->comm
	)
);

DEFINE_EVENT(sample_raw_enter_open,
	
	 TP_STRUCT(
                TP_STRUCT_COMMON
	
		__field( int             , nr		      )
		__array( char            , filename   , 8     )
		__field( long long       , flags	      )
		__field( long long       , mode		      )	
	
	),

	TP_PRINT(" ENTER_OPEN Mode:%d\n",
		 data->mode
        )
);

DEFINE_EVENT(sample_raw_sys_enter,
	
         TP_STRUCT(
                TP_STRUCT_COMMON

		__field( long long       , id               )
		__array( long long       , args       , 6   )
	),
	
	TP_PRINT(" SYS_ENTER Id:%d\n",
		 data->id
        )
);

DEFINE_EVENT(sample_raw_enter_read,
	
	TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int             , nr                 )
                __field( long long       , fd                 )
		__array( char            , buf        , 8     )
		__field( long long       , count              )
	),
	
	TP_PRINT(" ENTER_READ Fd:%d Count:%d\n",
		 data->fd,
		 data->count
        )
);

DEFINE_EVENT(sample_raw_exit_read,
	
	TP_STRUCT(
                TP_STRUCT_COMMON
		
		__field( int             , nr                 )
		__field( long long	 , ret		      )
	),

	TP_PRINT(" EXIT_READ Ret:%d\n",
		 data->ret
        )
);

DEFINE_EVENT(sample_raw_enter_write,

        TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int             , nr                 )
                __field( long long       , fd                 )
                __array( char            , buf        , 8     )
                __field( long long       , count              )
        ),

        TP_PRINT(" ENTER_WRITE Fd:%d Count:%d\n",
		 data->fd,
		 data->count
	)
);

DEFINE_EVENT(sample_raw_enter_lseek,

	TP_STRUCT(
                TP_STRUCT_COMMON

		__field( int             , nr                 )
                __field( long long       , fd                 )
		__field( long long       , offset             )
		__field( long long       , origin             )
	),

	TP_PRINT(" LSEEK Fd:%d Offset:%d Origin:%d\n",
		 data->fd,
		 data->offset,
		 data->origin 
        )
);

DEFINE_EVENT(sample_raw_enter_mmap,
	
	TP_STRUCT(
                TP_STRUCT_COMMON

		__field( int             , nr                 )
		__field( long long       , addr		      )
		__field( long long       , len		      )
		__field( long long       , prot               )
	        __field( long long       , flags	      )
		__field( long long       , fd		      )
		__field( long long       , offset	      )
	),

	TP_PRINT(" MMAP Len:%d Fd:%d Offset:%d\n",
		 data->len,
		 data->fd,
		 data->offset
        )
);

DEFINE_EVENT(sample_raw_page_fault,

	TP_STRUCT(
                TP_STRUCT_COMMON
			
		__field( int             , flag                 )
	),
	
	TP_PRINT(" PAGE_FAULT Flag:%d\n",
		 data->flag
        )
);

DEFINE_EVENT(sample_raw_page_alloc,
	
	TP_STRUCT(
                TP_STRUCT_COMMON
		
		__field( long long       , page                )
		__field( int             , order               )
		__field( int             , gfp_flag            )
		__field( int             , mig_type            )
	),
	
	TP_PRINT(" PAGE_ALLOC\n"
        )
);

DEFINE_EVENT(sample_raw_block_issue,

	TP_STRUCT(
                TP_STRUCT_COMMON

		__field( int             , dev                 )
		__field( long long       , sector              )
                __field( int             , nr_sector           )
                __field( int             , bytes               )
		__array( char            , rwbs         , 8    )
		__array( char            , comm         , 16   )
      		__array( char		 , cmd		, 4    )
	),

	TP_PRINT(" BLOCK_ISSUE Dev:%d,%d Nr_sectors:%d Rwbs:%s\n",
		 (unsigned int) data->dev >> 20,
                 (unsigned int) (data->dev & ( 1U << 20 ) - 1),
		 data->nr_sector,
		 data->rwbs
        )
);

DEFINE_EVENT(sample_raw_block_insert,

        TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int             , dev                 )
                __field( long long       , sector              )
                __field( int             , nr_sector           )
                __field( int             , bytes               )
                __array( char            , rwbs         , 8    )
                __array( char            , comm         , 16   )
                __array( char            , cmd          , 4    )
        ),

        TP_PRINT(" BLOCK_INSERT Dev:%d,%d Nr_sectors:%d Rwbs:%s\n",
                 (unsigned int) data->dev >> 20,
		 (unsigned int) (data->dev & ( 1U << 20 ) - 1),
                 data->nr_sector,
                 data->rwbs
        )
);


#endif
