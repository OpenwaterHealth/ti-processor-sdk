
SRCDIR += src/d3_rvp src/d3_rvp/include
INCDIR += src/d3_rvp src/d3_rvp/include

# Common source files across all platforms and cores
SRCS_COMMON += board_init.c board_lld_init.c board_clock.c board_mmr.c board_pll.c board_serdes_cfg.c
SRCS_COMMON += board_ddr.c board_info.c board_ethernet_config.c board_i2c_io_exp.c board_utils.c board_control.c board_power.c
SRCS_COMMON += board_pinmux.c J721E_pinmux_data.c J721E_pinmux_data_info.c J721E_pinmux_data_gesi.c J721E_pinmux_data_gesi_cpsw9g.c J721E_pinmux_data_d3_rvp_pers_board.c
SRCS_COMMON += J721E_pinmux_data_d3_rvp_bb.c
SRCS_COMMON += board_ddrtempmonitor.c J721E_pinmux_data_d3_bb_cpsw9g.c
PACKAGE_SRCS_COMMON = src/d3_rvp/src_files_d3_rvp.mk
