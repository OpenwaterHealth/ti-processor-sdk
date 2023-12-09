#include "app_one_chan_common.h"

void app_set_cfg_default(AppObj *obj)
{
    obj->width_in = 2336;
    obj->height_in = 1450;
    obj->width_out = 1920;
    obj->height_out = 1080;
    obj->is_interactive = 1;
    obj->table_width = LDC_TABLE_WIDTH;
    obj->table_height = LDC_TABLE_HEIGHT;
    obj->ds_factor = LDC_DS_FACTOR;
    obj->sensor_name = "HIMAX5530";
    obj->cam_dcc_id = 7;
    obj->selectedCam = 0;
}