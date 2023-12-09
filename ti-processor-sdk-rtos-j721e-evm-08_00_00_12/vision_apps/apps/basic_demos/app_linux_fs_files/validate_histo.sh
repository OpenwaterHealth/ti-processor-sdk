#!/bin/sh -x

if [[ ! -f ./validate.out ]]
then
  gcc ./validate_histo.c -o validate.out -lm
fi

./validate.out 4096 ./output/csix_raw_output_ch_0_* ./output/histo_output_ch_0_*
./validate.out 4096 ./output/csix_raw_output_ch_1_* ./output/histo_output_ch_1_*
./validate.out 4096 ./output/csix_raw_output_ch_2_* ./output/histo_output_ch_2_*
./validate.out 4096 ./output/csix_raw_output_ch_3_* ./output/histo_output_ch_3_*
./validate.out 4096 ./output/csix_raw_output_ch_4_* ./output/histo_output_ch_4_*
./validate.out 4096 ./output/csix_raw_output_ch_5_* ./output/histo_output_ch_5_*
./validate.out 4096 ./output/csix_raw_output_ch_6_* ./output/histo_output_ch_6_*
./validate.out 4096 ./output/csix_raw_output_ch_7_* ./output/histo_output_ch_7_*