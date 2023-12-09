#!/bin/bash

# Allow cores to boot
sleep 6

# Initialize cores
source /opt/vision_apps/vision_apps_init.sh

# Allow cores to configure with rpmsg
sleep 2

# Run the openwater application headless
/opt/vision_apps/vx_app_openwater.out --cfg /opt/vision_apps/app_openwater_headless.cfg