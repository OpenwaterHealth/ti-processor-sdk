DCC_TOOL_PATH=../../../../../tools/dcc_tools/
OUT_PATH=../../../../include

rm *.bin
rm $OUT_PATH/dcc_viss_ar0233.h
$DCC_TOOL_PATH/dcc_gen_linux AR0233_ipipe_rgb2rgb_1_dcc.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_viss_h3a_aewb_cfg.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_viss_h3a_mux_luts_cfg.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_viss_nsf4.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_viss_blc.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_flxd_cfa.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_rawfe_decompand.xml
cat *.bin > ../../dcc_bins/dcc_viss.bin
$DCC_TOOL_PATH/dcc_bin2c ../../dcc_bins/dcc_viss.bin $OUT_PATH/dcc_viss_ar0233.h dcc_viss_ar0233

rm *.bin
rm $OUT_PATH/dcc_2a_ar0233.h
$DCC_TOOL_PATH/dcc_gen_linux AR0233_awb_alg_ti3_tuning.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_viss_h3a_aewb_cfg.xml
$DCC_TOOL_PATH/dcc_gen_linux AR0233_viss_h3a_mux_luts_cfg.xml
cat *.bin > ../../dcc_bins/dcc_2a.bin
$DCC_TOOL_PATH/dcc_bin2c ../../dcc_bins/dcc_2a.bin $OUT_PATH/dcc_2a_ar0233.h dcc_2a_ar0233

rm *.bin
rm $OUT_PATH/dcc_ldc_ar0233.h
$DCC_TOOL_PATH/dcc_gen_linux AR0233_ldc.xml
cat *.bin > ../../dcc_bins/dcc_ldc.bin
$DCC_TOOL_PATH/dcc_bin2c ../../dcc_bins/dcc_ldc.bin $OUT_PATH/dcc_ldc_ar0233.h dcc_ldc_ar0233

rm *.bin

