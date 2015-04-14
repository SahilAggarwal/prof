#ifndef _H_PROBE
#define _H_PROBE

#include "cpu_map.h"
#include "thread_map.h"

struct probe {
	struct probe_buff    *buff;
	struct cpu_map       *cpu_map;
	struct thread_map    *thread_map;
};

int probe_start(char *,char **,pid_t);

static int probe_start_exec(char *,char **);

static int probe_start_pid(pid_t);

static struct probe *probe_init(pid_t);

void probe_enable(struct probe *);

#endif
