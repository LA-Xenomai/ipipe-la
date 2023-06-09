/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2021 Loongson Technology Co., Ltd.
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __LT8618_H__
#define __LT8618_H__

/* LT8618 Device Address
 * CI2CA				I2C address
 * (0~400mV)			0x72(default)
 * (400mV~800mV)		0x7a
 * (800mV~1200mV)		0x90
 * (1200mV~1600mV)		0x92
 * (2000mV~2400mV)		0x94
 * (2400mV~2800mV)		0x7e
 * (2800mV~3300mV)		0x76
 *
 * LT8618_CHIP_ADDR=0x72 when CI2CA pins are low(0~400mV)
 * LT8618_CHIP_ADDR=0x76 when CI2CA pins are high(2800mV~3300mV)
 * i2c addr 0x72(0x76) without w/r bit 0x39(0x3B)
 *
 * When LT8618SXB i2c_addr is not 0x72,
 * we need to control nRES Pin to reset LT8618SXB below:
 * Reset nRES_Pin -> delay 100ms -> Set nRES_Pin -> delay 100ms
 */
#define LT8618_CHIP_NAME "LT8618"
#define LT8618_CHIP_ADDR 0x3B
#define LT8618_GPIO_PLACEMENT_OFFSET 16U

#define LT8618_REG_START 0x0000
#define LT8618_REG_END 0x8585
#define LT8618_REG_PAGE 0x100
#define LT8618_REG_PAGE_SELECT 0xFF

#define LT8618_REG_ENABLE 0x80EE
#define LT8618_REG_CHIP_ENABLE_POS 0U
#define LT8618_REG_CHIP_ENABLE_MSK BIT_MASK(LT8618_REG_CHIP_ENABLE_POS)
#define CHIP_ENABLE 1U
#define ENABLE_REG_BANK 0x01

#define DDC_SPEED_10K 0x64
#define DDC_SPEED_50K 0x14
#define DDC_SPEED_100K 0x0A
#define DDC_SPEED_200K 0x05

/* General Registers */
#define LT8618_REG_CHIP_VERSION_BASE 0x8000
#define CHIP_VERSION_LEN 3U
#define LT8618_REG_CHIP_VERSION(x) (LT8618_REG_CHIP_VERSION_BASE + (x))
#define V_YEAR 0U
#define V_MONTH 1U
#define V_REVISION 2U

#define LT8618_REG_INPUT_VIDEO_TYPE 0x800A

#define LT8618_REG_INT_CFG 0x8210
#define INT_CFG_POLARITY_POS 0U
#define INT_CFG_POLARITY_MSK BIT_MASK(INT_CFG_POLARITY_POS)
#define INT_CFG_POLARITY_ACTIVE_HIGH 1U
#define INT_CFG_POLARITY_ACTIVE_LOW 0U

#define LT8618_REG_FREQ_METER2_BASE 0x821D
#define FREQ_METER2_LEN 3U
#define LT8618_REG_FREQ_METER2(x) (LT8618_REG_FREQ_METER2_BASE + (x))
#define FREQ_METER2_STATUS 0U
#define FREQ_METER2_FREQ_CHANGE_FLAG_POS 7U
#define FREQ_METER2_FREQ_CHANGE_FLAG_MSK \
	BIT_MASK(FREQ_METER2_FREQ_CHANGE_FLAG_POS)
#define FREQ_CHANGE 1U
#define FREQ_UNCHANGE 0U
#define FREQ_METER2_FREQ_DETECT_FLAG_POS 6U
#define FREQ_METER2_FREQ_DETECT_FLAG_MSK \
	BIT_MASK(FREQ_METER2_FREQ_DETECT_FLAG_POS)
#define FREQ_DETECT 1U
#define FREQ_UNDETECT 0U
#define FREQ_METER2_FREQ_STABLE_FLAG_POS 5U
#define FREQ_METER2_FREQ_STABLE_FLAG_MSK \
	BIT_MASK(FREQ_METER2_FREQ_STABLE_FLAG_POS)
#define FREQ_STABLE 1U
#define FREQ_UNSTABLE 0U
#define FREQ_METER2_PCLK_POS 0U
#define FREQ_METER2_PCLK_MSK (0x7 << FREQ_METER2_PCLK_POS)
#define FREQ_METER2_PCLK_H 1U
#define FREQ_METER2_PCLK_L 2U

#define LT8618_REG_INPUT_DATA_LANE_SEQ 0x8245
#define LT8618_REG_INPUT_VIDEO_SYNC_GEN 0x8247
#define LT8618_REG_INPUT_SIGNAL_SAMPLE_TYPE 0x824F
#define LT8618_REG_INPUT_SRC_SELECT 0x8250

#define LT8618_REG_LINK_STATUS 0x825E
#define LINK_STATUS_CEC_POS 3U
#define LINK_STATUS_CEC_MSK BIT_MASK(LINK_STATUS_CEC_POS)
#define LINK_STATUS_INPUT_POS 2U
#define LINK_STATUS_INPUT_MSK BIT_MASK(LINK_STATUS_INPUT_POS)
#define LINK_STATUS_OUTPUT_AC_POS 1U
#define LINK_STATUS_OUTPUT_AC_MSK BIT_MASK(LINK_STATUS_OUTPUT_AC_POS)
#define LINK_STATUS_OUTPUT_DC_POS 0U
#define LINK_STATUS_OUTPUT_DC_MSK BIT_MASK(LINK_STATUS_OUTPUT_DC_POS)
#define LINK_STATUS_STABLE 1U
#define LINK_STATUS_UNSTABLE 0U

#define LT8618_REG_INPUT_VIDEO_TIMING_BASE 0x8270
#define INPUT_VIDEO_TIMING_LEN 18U
#define LT8618_REG_INPUT_VIDEO_TIMING_PARAMETER \
	LT8618_REG_INPUT_VIDEO_TIMING_BASE
#define INPUT_VIDEO_TIMING_VS_POL_POS 1U
#define INPUT_VIDEO_TIMING_VS_POL_MSK BIT_MASK(INPUT_VIDEO_TIMING_HS_POL_POS)
#define INPUT_VIDEO_TIMING_VS_POL_P 1U
#define INPUT_VIDEO_TIMING_VS_POL_N 0U
#define INPUT_VIDEO_TIMING_HS_POL_POS 0U
#define INPUT_VIDEO_TIMING_HS_POL_MSK BIT_MASK(INPUT_VIDEO_TIMING_PCLK_POL_POS)
#define INPUT_VIDEO_TIMING_HS_POL_P 1U
#define INPUT_VIDEO_TIMING_HS_POL_N 0U
#define LT8618_REG_INPUT_VIDEO_TIMING_VSW 0x8271
#define LT8618_REG_INPUT_VIDEO_TIMING_HSW_H 0x8272
#define LT8618_REG_INPUT_VIDEO_TIMING_HSW_L 0x8273
#define LT8618_REG_INPUT_VIDEO_TIMING_VBP_L 0x8274
#define LT8618_REG_INPUT_VIDEO_TIMING_VFP_L 0x8275
#define LT8618_REG_INPUT_VIDEO_TIMING_HBP_H 0x8276
#define LT8618_REG_INPUT_VIDEO_TIMING_HBP_L 0x8277
#define LT8618_REG_INPUT_VIDEO_TIMING_HFP_H 0x8278
#define LT8618_REG_INPUT_VIDEO_TIMING_HFP_L 0x8279
#define LT8618_REG_INPUT_VIDEO_TIMING_VTO_H 0x827A
#define LT8618_REG_INPUT_VIDEO_TIMING_VTO_L 0x827B
#define LT8618_REG_INPUT_VIDEO_TIMING_HTO_H 0x827C
#define LT8618_REG_INPUT_VIDEO_TIMING_HTO_L 0x827D
#define LT8618_REG_INPUT_VIDEO_TIMING_VAC_H 0x827E
#define LT8618_REG_INPUT_VIDEO_TIMING_VAC_L 0x827F
#define LT8618_REG_INPUT_VIDEO_TIMING_HAC_H 0x8280
#define LT8618_REG_INPUT_VIDEO_TIMING_HAC_L 0x8281

#define LT8618_REG_DDC_BASE 0x8502
#define LT8618_REG_DDC_COUNT 5U
#define LT8618_REG_DDC_CMD 0x8507
#define LT8618_REG_DDC_CMD_POS 0U
#define LT8618_REG_DDC_CMD_MSK (0x7 << LT8618_REG_DDC_CMD_POS)
#define DDC_CMD_ABORT 0U
#define DDC_CMD_READ_BURST 1U
#define DDC_CMD_WRITE_BURST 2U
#define DDC_CMD_READ_SEEK 3U
#define DDC_CMD_READ_EDDC 4U
#define DDC_CMD_RESET 6U

#define LT8618_REG_DDC_STATUS 0x8540
#define LT8618_REG_DDC_BUS_LOST_POS 6U
#define LT8618_REG_DDC_BUS_LOST_MSK BIT_MASK(LT8618_REG_DDC_BUS_LOST_POS)
#define DDC_BUS_LOST 1U
#define LT8618_REG_DDC_BUS_BUSY_POS 5U
#define LT8618_REG_DDC_BUS_BUSY_MSK BIT_MASK(LT8618_REG_DDC_BUS_BUSY_POS)
#define DDC_BUS_BUSY 1U
#define LT8618_REG_DDC_BUS_NACK_POS 4U
#define LT8618_REG_DDC_BUS_NACK_MSK BIT_MASK(LT8618_REG_DDC_BUS_NACK_POS)
#define DDC_BUS_NACK 1U
#define LT8618_REG_DDC_BUS_DOING_POS 2U
#define LT8618_REG_DDC_BUS_DOING_MSK BIT_MASK(LT8618_REG_DDC_BUS_DOING_POS)
#define DDC_BUS_DOING 1U
#define LT8618_REG_DDC_BUS_DONE_POS 1U
#define LT8618_REG_DDC_BUS_DONE_MSK BIT_MASK(LT8618_REG_DDC_BUS_DONE_POS)
#define DDC_BUS_DONE 1U

#define LT8618_REG_FIFO_STATUS 0x8582
#define LT8618_REG_FIFO_DATA_COUNT_POS 2U
#define LT8618_REG_FIFO_DATA_COUNT_MSK (0x3F << LT8618_REG_FIFO_DATA_COUNT_POS)
#define LT8618_REG_FIFO_FULL_POS 1U
#define LT8618_REG_FIFO_FULL_MSK BIT_MASK(LT8618_REG_FIFO_FULL_POS)
#define FIFO_FULL 1U
#define LT8618_REG_FIFO_EMPTY_POS 0U
#define LT8618_REG_FIFO_EMPTY_MSK BIT_MASK(LT8618_REG_FIFO_EMPTY_POS)
#define FIFO_EMPTY 1U

#define LT8618_REG_FIFO_CONTENT 0x8583
#define LT8618_FIFO_MAX_LENGTH 32U

enum lt8618_chip_version {
	LT8618_VER_Unknown = 0,
	LT8618_VER_U1,
	LT8618_VER_U2,
	LT8618_VER_U3,
};

enum lt8618_pll_level {
	LT8618_PLL_LEVEL_LOW = 0,
	LT8618_PLL_LEVEL_MIDDLE,
	LT8618_PLL_LEVEL_HIGH,
};

enum lt8618_ddc_cmd {
	CMD_ABORT = 0x0,
	CMD_READ_BURST = 0x1,
	CMD_WRITE_BURST = 0x2,
	CMD_READ_SEEK = 0x3,
	CMD_READ_EDDC = 0x4,
	CMD_RESET = 0x6,
};

struct lt8618_device {
	enum lt8618_chip_version ver;
	enum lt8618_pll_level pll_level;
};

#endif
