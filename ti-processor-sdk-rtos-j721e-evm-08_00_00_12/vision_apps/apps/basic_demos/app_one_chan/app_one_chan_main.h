
#ifndef _APP_ONE_CHAN_H_
#define _APP_ONE_CHAN_H_

#include "app_one_chan_common.h"

vx_status app_init(AppObj *obj);
vx_status app_deinit(AppObj *obj);
vx_status app_create_graph(AppObj *obj);
vx_status app_run_graph(AppObj *obj);
vx_status app_delete_graph(AppObj *obj);
void add_graph_parameter_by_node_index(vx_graph graph, vx_node node, vx_uint32 node_parameter_index);

#endif
