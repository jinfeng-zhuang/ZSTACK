// flow.h
#ifndef FLOW_H
#define FLOW_H

#include "zstack/types.h"

#define FLOW_NODE_NAME_LEN (16)

// ·À´ô
enum FLOW_RETVAL {
    FLOW_FAIL = 'F',
    FLOW_RUNNING = 'K',
    FLOW_DONE = 'O',
};

// return value: enum FLOW_RETVAL
// U8: make the result value size more clear
typedef U8 (*flow_callback)(U8 step, U8 exit, void *arg);

struct flow_node {
    // Current node id for verify, to avoid node_id not match
    U8 node_id;
    
    // This node's ID is the C array index, to make it simple
    // To keep it simple, this node has 2 branchs
    U8 node_id_ok;
    U8 node_id_fail;
    
    //flow_callback cb; // remove it to reduce the implementation's complex
    //void *param;      // remove it to reduce the implementation's complex
    
    U32 timeout; // us
    
    // Below don't need to initial with user
    U8 flag_busy;       // Busy Flag as 'IP', 0 = complete, 1 = process
    
    // Below is hook for debug
    U64 time_start; // us, 0 - ready, !0 - started, check result or timeout
    U64 time_end;
    U64 time_used; // with ms or s
    U64 time_used_max;
    
    // More hooks for debug
};

struct flow {
    S8* name;
	struct flow_node *node;
	U8 count;
	U8 current;
	flow_callback cb;
	void *arg;
};

U8 flow_run(struct flow *flow);

#endif
