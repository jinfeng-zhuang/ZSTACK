#include <assert.h>
#include <stdio.h>

#include "zstack/time.h"
#include "zstack/flow.h"
#include "zstack/types.h"

//#define ENABLE_TIMEOUT

static struct flow_node* find_node(struct flow_node* flow, U8 total, U8 id)
{
    U8 i;

    for (i = 0; i < total; i++)
    {
        if (id == flow[i].node_id)
        {
            return &flow[i];
        }
    }

    return NULL;
}

// Reference and compare
// Keep it simple to add new code, understand, and debug, it's most important!!!

// Period running

/*
 * Return the result of the node execution status:
 *      OK
 *      FAIL
 *      KEEP
 */
U8 flow_run(struct flow *_flow)
{
	/* Pass arguments */
	struct flow_node *flow = _flow->node;
	U8 total = _flow->count;
	U8 *current = &_flow->current;
	flow_callback cb = _flow->cb;
	void *arg = _flow->arg;

	/* Internal required */
    U64 time_end;
    U8 node_id_current = 0;
    struct flow_node* node = NULL; // Initial as NULL
    U8 result = FLOW_DONE;
    
    node_id_current = *current;

    node = find_node(flow, total, node_id_current);

    if (NULL == node)
    {
        printf("[ERROR] Node %d not in flow!!!\n", node_id_current);
        assert(0);
    }

    if (0 == node->flag_busy)
    {
        // log to file
        printf("FLOW ENTER: %s: %d\n", _flow->name, node_id_current);

        node->flag_busy = 1;
        node->time_start = timestamp_us();
    }
    
    result = cb(node_id_current, 0, arg);
    
    time_end = timestamp_us();

    switch (result)
    {
    case FLOW_RUNNING:
        break;
    case FLOW_DONE:
        cb(node_id_current, 1, arg);
        node_id_current = node->node_id_ok;
        node->flag_busy = 0;
        break;
    case FLOW_FAIL:
        cb(node_id_current, 1, arg);
        node_id_current = node->node_id_fail;
        node->flag_busy = 0;
        break;
    default:
        assert(0);
        break;
    }
    
    // Check timeout
    if (time_end - (node->time_start) > (node->timeout))
    {
#ifdef ENABLE_TIMEOUT
        cb(node_id_current, 1, arg);
        node_id_current = node->node_id_fail;
        node->flag_busy = 0;

        result = FLOW_FAIL;
#endif
    }

    //============================= HOOKS =============================

    // Take note of time take
    if (0 == node->flag_busy)
    {
        // Return the current node
        *current = node_id_current;

        // Statistic the time take
        node->time_end = time_end;
        node->time_used = node->time_end - node->time_start;
        
        if (node->time_used_max < node->time_used)
        {
            node->time_used_max = node->time_used;
        }

        // log to file
        printf("FLOW FINISH: %s: %d, RESULT = %c, TAKE %f s\n", _flow->name, node_id_current, result, (F64)node->time_used / 1000000);
    }

    return result;
}
