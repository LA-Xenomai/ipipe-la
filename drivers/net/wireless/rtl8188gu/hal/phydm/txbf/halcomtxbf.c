/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
/* ************************************************************
 * Description:
 *
 * This file is for TXBF mechanism
 *
 * ************************************************************ */
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (BEAMFORMING_SUPPORT == 1)
/*Beamforming halcomtxbf API create by YuChen 2015/05*/

void hal_com_txbf_beamform_init(
	void			*p_dm_void
)
{
}

/*Only used for MU BFer Entry when get GID management frame (self is as MU STA)*/
void hal_com_txbf_config_gtab(
	void			*p_dm_void
)
{
}

void phydm_beamform_set_sounding_enter(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&(p_txbf_info->txbf_enter_work_item)) ==
	    false)
		odm_schedule_work_item(&(p_txbf_info->txbf_enter_work_item));
#endif
}

void phydm_beamform_set_sounding_leave(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&(p_txbf_info->txbf_leave_work_item)) ==
	    false)
		odm_schedule_work_item(&(p_txbf_info->txbf_leave_work_item));
#else
	hal_com_txbf_leave_work_item_callback(p_dm);
#endif
}

void phydm_beamform_set_sounding_rate(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&(p_txbf_info->txbf_rate_work_item)) ==
	    false)
		odm_schedule_work_item(&(p_txbf_info->txbf_rate_work_item));
#else
	hal_com_txbf_rate_work_item_callback(p_dm);
#endif
}

void phydm_beamform_set_sounding_status(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&(p_txbf_info->txbf_status_work_item)) ==
	    false)
		odm_schedule_work_item(&(p_txbf_info->txbf_status_work_item));
#else
	hal_com_txbf_status_work_item_callback(p_dm);
#endif
}

void phydm_beamform_set_sounding_fw_ndpa(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (*p_dm->p_is_fw_dw_rsvd_page_in_progress)
		odm_set_timer(p_dm, &(p_txbf_info->txbf_fw_ndpa_timer), 5);
	else
		odm_schedule_work_item(&(p_txbf_info->txbf_fw_ndpa_work_item));
#else
	hal_com_txbf_fw_ndpa_work_item_callback(p_dm);
#endif
}

void phydm_beamform_set_sounding_clk(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&(p_txbf_info->txbf_clk_work_item)) ==
	    false)
		odm_schedule_work_item(&(p_txbf_info->txbf_clk_work_item));
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	struct _ADAPTER	*padapter = p_dm->adapter;

	rtw_run_in_thread_cmd(padapter, hal_com_txbf_clk_work_item_callback, p_dm);
#else
	hal_com_txbf_clk_work_item_callback(p_dm);
#endif
}

void phydm_beamform_set_reset_tx_path(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&
				       (p_txbf_info->txbf_reset_tx_path_work_item)) == false)
		odm_schedule_work_item(&(p_txbf_info->txbf_reset_tx_path_work_item));
#else
	hal_com_txbf_reset_tx_path_work_item_callback(p_dm);
#endif
}

void phydm_beamform_set_get_tx_rate(
	void			*p_dm_void
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	if (odm_is_work_item_scheduled(&(p_txbf_info->txbf_get_tx_rate_work_item))
	    == false)
		odm_schedule_work_item(&(p_txbf_info->txbf_get_tx_rate_work_item));
#else
	hal_com_txbf_get_tx_rate_work_item_callback(p_dm);
#endif
}

void
hal_com_txbf_leave_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
}


void
hal_com_txbf_fw_ndpa_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
}

void
hal_com_txbf_clk_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PHAL_DATA_TYPE	p_hal_data = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT		*p_dm = &p_hal_data->DM_OutSrc;
#else
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
#endif

	PHYDM_DBG(p_dm, DBG_TXBF, ("[%s] Start!\n", __func__));

	if (p_dm->support_ic_type & ODM_RTL8812)
		hal_txbf_jaguar_clk_8812a(p_dm);
}



void
hal_com_txbf_rate_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
}


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
void hal_com_txbf_fw_ndpa_timer_callback(
	struct timer_list		*p_timer
)
{

	struct _ADAPTER		*adapter = (struct _ADAPTER *)p_timer->Adapter;
	PHAL_DATA_TYPE	p_hal_data = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT		*p_dm = &p_hal_data->DM_OutSrc;

	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;


	PHYDM_DBG(p_dm, DBG_TXBF, ("[%s] Start!\n", __func__));

	if (*p_dm->p_is_fw_dw_rsvd_page_in_progress)
		odm_set_timer(p_dm, &(p_txbf_info->txbf_fw_ndpa_timer), 5);
	else
		odm_schedule_work_item(&(p_txbf_info->txbf_fw_ndpa_work_item));
}
#endif


void
hal_com_txbf_status_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
}

void
hal_com_txbf_reset_tx_path_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
}

void
hal_com_txbf_get_tx_rate_work_item_callback(
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER		*adapter
#else
	void			*p_dm_void
#endif
)
{
}


boolean hal_com_txbf_set(
	void			*p_dm_void,
	u8			set_type,
	void			*p_in_buf
)
{
	struct PHY_DM_STRUCT	*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	u8			*p_u1_tmp = (u8 *)p_in_buf;
	struct _HAL_TXBF_INFO	*p_txbf_info = &p_dm->beamforming_info.txbf_info;

	PHYDM_DBG(p_dm, DBG_TXBF, ("[%s] set_type = 0x%X\n", __func__, set_type));

	switch (set_type) {
	case TXBF_SET_SOUNDING_ENTER:
		p_txbf_info->txbf_idx = *p_u1_tmp;
		phydm_beamform_set_sounding_enter(p_dm);
		break;

	case TXBF_SET_SOUNDING_LEAVE:
		p_txbf_info->txbf_idx = *p_u1_tmp;
		phydm_beamform_set_sounding_leave(p_dm);
		break;

	case TXBF_SET_SOUNDING_RATE:
		p_txbf_info->BW = p_u1_tmp[0];
		p_txbf_info->rate = p_u1_tmp[1];
		phydm_beamform_set_sounding_rate(p_dm);
		break;

	case TXBF_SET_SOUNDING_STATUS:
		p_txbf_info->txbf_idx = *p_u1_tmp;
		phydm_beamform_set_sounding_status(p_dm);
		break;

	case TXBF_SET_SOUNDING_FW_NDPA:
		p_txbf_info->ndpa_idx = *p_u1_tmp;
		phydm_beamform_set_sounding_fw_ndpa(p_dm);
		break;

	case TXBF_SET_SOUNDING_CLK:
		phydm_beamform_set_sounding_clk(p_dm);
		break;

	case TXBF_SET_TX_PATH_RESET:
		p_txbf_info->txbf_idx = *p_u1_tmp;
		phydm_beamform_set_reset_tx_path(p_dm);
		break;

	case TXBF_SET_GET_TX_RATE:
		phydm_beamform_set_get_tx_rate(p_dm);
		break;

	}

	return true;
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
boolean hal_com_txbf_get(
	struct _ADAPTER		*adapter,
	u8			get_type,
	void			*p_out_buf
)
{
	PHAL_DATA_TYPE		p_hal_data = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT			*p_dm = &p_hal_data->DM_OutSrc;
	boolean			*p_boolean = (boolean *)p_out_buf;

	PHYDM_DBG(p_dm, DBG_TXBF, ("[%s] Start!\n", __func__));

	if (get_type == TXBF_GET_EXPLICIT_BEAMFORMEE) {
		if (IS_HARDWARE_TYPE_OLDER_THAN_8812A(adapter))
			*p_boolean = false;
		else if (
			IS_HARDWARE_TYPE_JAGUAR(adapter)
			|| IS_HARDWARE_TYPE_JAGUAR_AND_JAGUAR2(adapter))
			*p_boolean = true;
		else
			*p_boolean = false;
	} else if (get_type == TXBF_GET_EXPLICIT_BEAMFORMER) {
		if (IS_HARDWARE_TYPE_OLDER_THAN_8812A(adapter))
			*p_boolean = false;
		else	if (
			IS_HARDWARE_TYPE_JAGUAR(adapter)
			|| IS_HARDWARE_TYPE_JAGUAR_AND_JAGUAR2(adapter)) {
			if (p_hal_data->RF_Type == RF_2T2R || p_hal_data->RF_Type == RF_3T3R)
				*p_boolean = true;
			else
				*p_boolean = false;
		} else
			*p_boolean = false;
	} else if (get_type == TXBF_GET_MU_MIMO_STA) {
		*p_boolean = false;


	} else if (get_type == TXBF_GET_MU_MIMO_AP) {
		*p_boolean = false;
	}

	return true;
}
#endif


#endif
