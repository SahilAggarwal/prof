#ifndef _H_EVENTS
#define _H_EVENTS

#include <asm/types.h>

#define VM_FAULT_MAJOR  0x0004

#define __field(type, item)		 type item;
#define __array(type, item, len)	 type item[len];

#define TP_STRUCT(args...) 		args
#define TP_PRINT(args...)		args

#define TP_STRUCT_COMMON					\
	__field( unsigned short ,  common_type 	        )	\
        __field( unsigned char  ,  common_flags         )	\
        __field( unsigned char  ,  common_preempt_count )	\
        __field( int            ,  common_pid		)	\

#define DEFINE_EVENT(name,tstruct)	 			\
	struct name {						\
		tstruct						\
	};							\
								\
	void name##_entry(void **,char *);			

#define DEFINE_PRINT_FUNC(name,p_args)				\
	void name##_entry(void **arg, char *str) {		\
		struct name *data = (struct name *)*arg; 	\
                sprintf(str,p_args);                            \
                *arg += sizeof(struct name);                    \
	}

#define get_str(field) (char *)get_dynamic_array(field)
#define get_dynamic_array(field)      \
                ((void *)data + (data->arg & 0xffff))
	
typedef void (*func)(void **,char *);

func get_output_function(char *, char **);

DEFINE_EVENT(sample_tid,
	
	TP_STRUCT( 
		__field( int , tid )
		__field( int , pid )	
	)

);

DEFINE_EVENT(sample_time,

	TP_STRUCT(
		__field( long long, time )
	)
);

DEFINE_EVENT(sample_cpu,
	
	TP_STRUCT(
		__field( int  , cpu )
		__field( int  , res )
	)
);

DEFINE_EVENT(sched_switch,

	TP_STRUCT(
		TP_STRUCT_COMMON
		
		__array( char		, prev_comm ,	16	)
		__field( int		, prev_pid 		)
		__field( int            , prev_prio 		)
		__field( long long      , prev_state		)
		__array( char           , next_comm ,   16      )
                __field( int            , next_pid              )
                __field( int            , next_prio             )
	)

);

DEFINE_EVENT(newtask,
	
	TP_STRUCT(
		TP_STRUCT_COMMON
		
		__field( int            , pid	              )
		__array( char           , comm 	     ,  16    )
		__field( unsigned long  , flags	     	      )
		__field( short          , oom_score           )
	)

);

DEFINE_EVENT(enter_open,
	
	 TP_STRUCT(
                TP_STRUCT_COMMON
	
		__field( int             , nr		      )
		__array( char            , filename   , 8     )
		__field( long long       , flags	      )
		__field( long long       , mode		      )	
	)
);

DEFINE_EVENT(getname,

	TP_STRUCT(
		TP_STRUCT_COMMON
	
		__field( long long		, ret_ip		)
		__field( __u32			, arg 			)
	)
);

DEFINE_EVENT(sys_enter,
	
         TP_STRUCT(
                TP_STRUCT_COMMON

		__field( long long       , id               )
		__array( long long       , args       , 6   )
	)
	
);

DEFINE_EVENT(enter_read,
	
	TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int             , nr                 )
                __field( long long       , fd                 )
		__array( char            , buf        , 8     )
		__field( long long       , count              )
	)
);

DEFINE_EVENT(exit_read,
	
	TP_STRUCT(
                TP_STRUCT_COMMON
		
		__field( int             , nr                 )
		__field( long long	 , ret		      )
	)

);

DEFINE_EVENT(enter_write,

        TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int             , nr                 )
                __field( long long       , fd                 )
                __array( char            , buf        , 8     )
                __field( long long       , count              )
        )

);

DEFINE_EVENT(enter_lseek,

	TP_STRUCT(
                TP_STRUCT_COMMON

		__field( int             , nr                 )
                __field( long long       , fd                 )
		__field( long long       , offset             )
		__field( long long       , origin             )
	)

);

DEFINE_EVENT(enter_mmap,
	
	TP_STRUCT(
                TP_STRUCT_COMMON

		__field( int             , nr                 )
		__field( long long       , addr		      )
		__field( long long       , len		      )
		__field( long long       , prot               )
	        __field( long long       , flags	      )
		__field( long long       , fd		      )
		__field( long long       , offset	      )
	)

);

DEFINE_EVENT(sys_exit_brk,

	TP_STRUCT(
		TP_STRUCT_COMMON

		__field( int		, nr		      )
		__field( long long      , ret		      )
	)
);

DEFINE_EVENT(kmalloc,

	TP_STRUCT(
		TP_STRUCT_COMMON

		__field( long long 	, call_site	      )
		__array( const char	, ptr		, 8   )
		__field( long long	, bytes_req           )
		__field( long long	, bytes_alloc	      )
		__field( int		, gfp_flags	      )
	)
	
);

DEFINE_EVENT(kmem_cache,

        TP_STRUCT(
                TP_STRUCT_COMMON

                __field( long long      , call_site           )
                __array( const char     , ptr           , 8   )
                __field( long long      , bytes_req           )
                __field( long long      , bytes_alloc         )
                __field( int            , gfp_flags           )
        )
);


DEFINE_EVENT(page_fault,

	TP_STRUCT(
                TP_STRUCT_COMMON
	
                __field( long long      , vma                  )
                __field( long long      , addr                 )
                __field( int            , flags                )
                __field( int            , ret                  )
	)

);

DEFINE_EVENT(page_alloc,
	
	TP_STRUCT(
                TP_STRUCT_COMMON
		
		__field( long long       , page                )
		__field( int             , order               )
		__field( int             , gfp_flag            )
		__field( int             , mig_type            )
	)
);

DEFINE_EVENT(page_alloc_zone,
	
	TP_STRUCT(
		TP_STRUCT_COMMON

		__array( char		, page		,   8  )
		__field( int		, order 	       )
		__field( int		, migrate_type	       )
	)

);

DEFINE_EVENT(page_free,
	
	TP_STRUCT(
		TP_STRUCT_COMMON
		
		__array( char           , page          ,   8  )
                __field( int            , order                )
	)
	
);

DEFINE_EVENT(page_free_batched,
	
	TP_STRUCT(
		TP_STRUCT_COMMON

		__array( char           , page          ,   8  )
                __field( int            , order                )
	)

);

DEFINE_EVENT(page_swap_out,
		
	TP_STRUCT(
		TP_STRUCT_COMMON

		__field( unsigned long	, pfn		       )
		__field( int		, ret		       )
	)
);

DEFINE_EVENT(page_swap_in,

        TP_STRUCT(
                TP_STRUCT_COMMON

		__field( long long	, vma		       )
		__field( long long	, addr		       )
		__field( int		, flags		       )
                __field( int            , ret                  )
        )
);

DEFINE_EVENT(block_issue,

	TP_STRUCT(
                TP_STRUCT_COMMON

		__field( int             , dev                 )
		__field( long long       , sector              )
                __field( int             , nr_sector           )
                __field( int             , bytes               )
		__array( char            , rwbs         , 8    )
		__array( char            , comm         , 16   )
      		__array( char		 , cmd		, 4    )
	)

);

DEFINE_EVENT(block_insert,

        TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int             , dev                 )
                __field( long long       , sector              )
                __field( int             , nr_sector           )
                __field( int             , bytes               )
                __array( char            , rwbs         , 8    )
                __array( char            , comm         , 16   )
                __array( char            , cmd          , 4    )
        )

);

DEFINE_EVENT(net_dev_queue,

	TP_STRUCT(
		TP_STRUCT_COMMON
	
		__field( long long	  , buff	       )
		__field( int		  , len		       )
		__array( char		  , name	, 4    )
	)
);

DEFINE_EVENT(net_dev_xmit,

	TP_STRUCT(
		TP_STRUCT_COMMON

		__field( long long        , buff               )
                __field( int              , len                )
		__field( int              , rc                 )
                __array( char             , name        , 4    )
	)
);

DEFINE_EVENT(netif_receive_skb,
	
	TP_STRUCT(
                TP_STRUCT_COMMON

                __field( long long        , buff               )
                __field( int              , len                )
                __array( char             , name        , 4    )
        )
);

DEFINE_EVENT(netif_rx,
	
	TP_STRUCT(
                TP_STRUCT_COMMON

                __field( long long        , buff               )
                __field( int              , len                )
                __array( char             , name        , 4    )
        )
);

DEFINE_EVENT(sys_enter_recvfrom,

	TP_STRUCT(
		TP_STRUCT_COMMON
		
		__field( int		  , nr		       )
		__field( long long	  , fd		       )
		__array( char		  , ubuf	, 8    )
		__field( long long	  , size	       )
		__field( long long 	  , flags	       )
		__array( char		  , addr	, 8    )
		__array( char		  , addr_len	, 8    )
	)
);

DEFINE_EVENT(sys_enter_sendto,

        TP_STRUCT(
                TP_STRUCT_COMMON

                __field( int              , nr                 )
                __field( long long        , fd                 )
                __array( char             , ubuf        , 8    )
                __field( long long        , len                )
                __field( long long        , flags              )
                __array( char             , addr        , 8    )
                __array( char             , addr_len    , 8    )
        )
);

#endif
