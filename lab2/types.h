#ifndef TYPES
#define TYPES

typedef enum{
	STATE_CREATED,
	STATE_READY,
	STATE_RUNNING,
	STATE_BLOCKED,
	STATE_FINISHED
} process_state_t;

typedef enum{
	TRANS_TO_READY,
	TRANS_TO_RUN,
	TRANS_TO_BLOCK,
	TRANS_TO_PREEMPT
} trans_state_t;
#endif