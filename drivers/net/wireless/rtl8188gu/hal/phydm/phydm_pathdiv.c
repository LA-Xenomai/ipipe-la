/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
 * include files
 * ************************************************************ */
#include "mp_precomp.h"
#include "phydm_precomp.h"

#if (defined(CONFIG_PATH_DIVERSITY))
#if RTL8814A_SUPPORT

void phydm_dtp_fix_tx_path(
	void	*p_dm_void,
	u8	path
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_		*p_dm_path_div = &p_dm->dm_path_div;
	u8			i, num_enable_path = 0;

	if (path == p_dm_path_div->pre_tx_path)
		return;
	else
		p_dm_path_div->pre_tx_path = path;

	odm_set_bb_reg(p_dm, 0x93c, BIT(18) | BIT(19), 3);

	for (i = 0; i < 4; i++) {
		if (path & BIT(i))
			num_enable_path++;
	}
	PHYDM_DBG(p_dm, DBG_PATH_DIV, (" number of turn-on path : (( %d ))\n",
				       num_enable_path));

	if (num_enable_path == 1) {
		odm_set_bb_reg(p_dm, 0x93c, 0xf00000, path);

		if (path == BB_PATH_A) { /* 1-1 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A ))\n"));
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
		} else 	if (path == BB_PATH_B) { /* 1-2 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( B ))\n"));
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 0);
		} else 	if (path == BB_PATH_C) { /* 1-3 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( C ))\n"));
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 0);

		} else 	if (path == BB_PATH_D) { /* 1-4 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( D ))\n"));
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 0);
		}

	} else	if (num_enable_path == 2) {
		odm_set_bb_reg(p_dm, 0x93c, 0xf00000, path);
		odm_set_bb_reg(p_dm, 0x940, 0xf0, path);

		if (path == (BB_PATH_AB)) { /* 2-1 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A B ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 1);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(9) | BIT(8), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(11) | BIT(10), 1);
		} else 	if (path == BB_PATH_AC) { /* 2-2 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A C ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 1);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(9) | BIT(8), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(13) | BIT(12), 1);
		} else 	if (path == BB_PATH_AD) { /* 2-3 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A D ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 1);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(9) | BIT(8), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(15) | BIT(14), 1);
		} else 	if (path == BB_PATH_BC) { /* 2-4 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( B C ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 1);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(11) | BIT(10), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(13) | BIT(12), 1);
		} else 	if (path == BB_PATH_BD) { /* 2-5 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( B D ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 1);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(11) | BIT(10), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(15) | BIT(14), 1);
		} else 	if (path == BB_PATH_CD) { /* 2-6 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( C D ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 1);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(13) | BIT(12), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(15) | BIT(14), 1);
		}

	} else	if (num_enable_path == 3) {
		odm_set_bb_reg(p_dm, 0x93c, 0xf00000, path);
		odm_set_bb_reg(p_dm, 0x940, 0xf0, path);
		odm_set_bb_reg(p_dm, 0x940, 0xf0000, path);

		if (path == BB_PATH_ABC) { /* 3-1 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A B C))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 1);
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 2);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(9) | BIT(8), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(11) | BIT(10), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(13) | BIT(12), 2);
			/* set for 3ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(21) | BIT(20), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(23) | BIT(22), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(25) | BIT(24), 2);
		} else 	if (path == BB_PATH_ABD) { /* 3-2 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A B D ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 1);
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 2);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(9) | BIT(8), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(11) | BIT(10), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(15) | BIT(14), 2);
			/* set for 3ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(21) | BIT(20), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(23) | BIT(22), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(27) | BIT(26), 2);

		} else 	if (path == BB_PATH_ACD) { /* 3-3 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( A C D ))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(25) | BIT(24), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 1);
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 2);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(9) | BIT(8), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(13) | BIT(12), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(15) | BIT(14), 2);
			/* set for 3ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(21) | BIT(20), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(25) | BIT(24), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(27) | BIT(26), 2);
		} else 	if (path == BB_PATH_BCD) { /* 3-4 */
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path (( B C D))\n"));
			/* set for 1ss */
			odm_set_bb_reg(p_dm, 0x93c, BIT(27) | BIT(26), 0);
			odm_set_bb_reg(p_dm, 0x93c, BIT(29) | BIT(28), 1);
			odm_set_bb_reg(p_dm, 0x93c, BIT(31) | BIT(30), 2);
			/* set for 2ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(11) | BIT(10), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(13) | BIT(12), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(15) | BIT(14), 2);
			/* set for 3ss */
			odm_set_bb_reg(p_dm, 0x940, BIT(23) | BIT(22), 0);
			odm_set_bb_reg(p_dm, 0x940, BIT(25) | BIT(24), 1);
			odm_set_bb_reg(p_dm, 0x940, BIT(27) | BIT(26), 2);
		}
	} else	if (num_enable_path == 4)
		PHYDM_DBG(p_dm, DBG_PATH_DIV, (" Turn on path ((A  B C D))\n"));

}

void phydm_find_default_path(
	void	*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_		*p_dm_path_div = &p_dm->dm_path_div;
	u32	rssi_avg_a = 0, rssi_avg_b = 0, rssi_avg_c = 0, rssi_avg_d = 0,
		rssi_avg_bcd = 0;
	u32	rssi_total_a = 0, rssi_total_b = 0, rssi_total_c = 0, rssi_total_d = 0;

	/* 2 Default path Selection By RSSI */

	rssi_avg_a = (p_dm_path_div->path_a_cnt_all > 0) ?
		     (p_dm_path_div->path_a_sum_all / p_dm_path_div->path_a_cnt_all) : 0 ;
	rssi_avg_b = (p_dm_path_div->path_b_cnt_all > 0) ?
		     (p_dm_path_div->path_b_sum_all / p_dm_path_div->path_b_cnt_all) : 0 ;
	rssi_avg_c = (p_dm_path_div->path_c_cnt_all > 0) ?
		     (p_dm_path_div->path_c_sum_all / p_dm_path_div->path_c_cnt_all) : 0 ;
	rssi_avg_d = (p_dm_path_div->path_d_cnt_all > 0) ?
		     (p_dm_path_div->path_d_sum_all / p_dm_path_div->path_d_cnt_all) : 0 ;


	p_dm_path_div->path_a_sum_all = 0;
	p_dm_path_div->path_a_cnt_all = 0;
	p_dm_path_div->path_b_sum_all = 0;
	p_dm_path_div->path_b_cnt_all = 0;
	p_dm_path_div->path_c_sum_all = 0;
	p_dm_path_div->path_c_cnt_all = 0;
	p_dm_path_div->path_d_sum_all = 0;
	p_dm_path_div->path_d_cnt_all = 0;

	if (p_dm_path_div->use_path_a_as_default_ant == 1) {
		rssi_avg_bcd = (rssi_avg_b + rssi_avg_c + rssi_avg_d) / 3;

		if ((rssi_avg_a + ANT_DECT_RSSI_TH) > rssi_avg_bcd) {
			p_dm_path_div->is_path_a_exist = true;
			p_dm_path_div->default_path = PATH_A;
		} else
			p_dm_path_div->is_path_a_exist = false;
	} else {
		if ((rssi_avg_a >= rssi_avg_b) && (rssi_avg_a >= rssi_avg_c)
		    && (rssi_avg_a >= rssi_avg_d))
			p_dm_path_div->default_path = PATH_A;
		else if ((rssi_avg_b >= rssi_avg_c) && (rssi_avg_b >= rssi_avg_d))
			p_dm_path_div->default_path = PATH_B;
		else if (rssi_avg_c >= rssi_avg_d)
			p_dm_path_div->default_path = PATH_C;
		else
			p_dm_path_div->default_path = PATH_D;
	}


}


void phydm_candidate_dtp_update(
	void	*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_		*p_dm_path_div = &p_dm->dm_path_div;

	p_dm_path_div->num_candidate = 3;

	if (p_dm_path_div->use_path_a_as_default_ant == 1) {
		if (p_dm_path_div->num_tx_path == 3) {
			if (p_dm_path_div->is_path_a_exist) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_ABC;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_ABD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_ACD;
			} else { /* use path BCD */
				p_dm_path_div->num_candidate = 1;
				phydm_dtp_fix_tx_path(p_dm, BB_PATH_BCD);
				return;
			}
		} else	if (p_dm_path_div->num_tx_path == 2) {
			if (p_dm_path_div->is_path_a_exist) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_AB;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_AC;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_AD;
			} else {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_BC;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_BD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_CD;
			}
		}
	} else {
		/* 2 3 TX mode */
		if (p_dm_path_div->num_tx_path == 3) { /* choose 3 ant form 4 */
			if (p_dm_path_div->default_path == PATH_A) { /* choose 2 ant form 3 */
				p_dm_path_div->ant_candidate_1 =  BB_PATH_ABC;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_ABD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_ACD;
			} else if (p_dm_path_div->default_path == PATH_B) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_ABC;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_ABD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_BCD;
			} else if (p_dm_path_div->default_path == PATH_C) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_ABC;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_ACD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_BCD;
			} else if (p_dm_path_div->default_path == PATH_D) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_ABD;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_ACD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_BCD;
			}
		}

		/* 2 2 TX mode */
		else if (p_dm_path_div->num_tx_path == 2) { /* choose 2 ant form 4 */
			if (p_dm_path_div->default_path == PATH_A) { /* choose 2 ant form 3 */
				p_dm_path_div->ant_candidate_1 =  BB_PATH_AB;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_AC;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_AD;
			} else if (p_dm_path_div->default_path == PATH_B) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_AB;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_BC;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_BD;
			} else if (p_dm_path_div->default_path == PATH_C) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_AC;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_BC;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_CD;
			} else if (p_dm_path_div->default_path == PATH_D) {
				p_dm_path_div->ant_candidate_1 =  BB_PATH_AD;
				p_dm_path_div->ant_candidate_2 =  BB_PATH_BD;
				p_dm_path_div->ant_candidate_3 =  BB_PATH_CD;
			}
		}
	}
}


void phydm_dynamic_tx_path(
	void	*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_		*p_dm_path_div = &p_dm->dm_path_div;

	struct sta_info	*p_entry;
	u32	i;
	u8	num_client = 0;
	u8	h2c_parameter[6] = {0};


	if (!p_dm->is_linked) { /* is_linked==False */
		PHYDM_DBG(p_dm, DBG_PATH_DIV, ("DTP_8814 [No Link!!!]\n"));

		if (p_dm_path_div->is_become_linked == true) {
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" [Be disconnected]----->\n"));
			p_dm_path_div->is_become_linked = p_dm->is_linked;
		}
		return;
	} else {
		if (p_dm_path_div->is_become_linked == false) {
			PHYDM_DBG(p_dm, DBG_PATH_DIV, (" [Be Linked !!!]----->\n"));
			p_dm_path_div->is_become_linked = p_dm->is_linked;
		}
	}

	/* 2 [period CTRL] */
	if (p_dm_path_div->dtp_period >= 2)
		p_dm_path_div->dtp_period = 0;
	else {
		/* PHYDM_DBG(p_dm,DBG_PATH_DIV, ("Phydm_Dynamic_Tx_Path_8814A()  Stay = (( %d ))\n",p_dm_path_div->dtp_period)); */
		p_dm_path_div->dtp_period++;
		return;
	}


	/* 2 [Fix path] */
	if (p_dm->path_select != PHYDM_AUTO_PATH)
		return;

	/* 2 [Check Bfer] */
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if (BEAMFORMING_SUPPORT == 1)
	{
		enum beamforming_cap		beamform_cap = (p_dm->beamforming_info.beamform_cap);

		if (beamform_cap & BEAMFORMER_CAP) { /* BFmer On  &&   Div On->Div Off */
			if (p_dm_path_div->fix_path_bfer == 0) {
				PHYDM_DBG(p_dm, DBG_PATH_DIV, ("[ PathDiv : OFF ]   BFmer ==1\n"));
				p_dm_path_div->fix_path_bfer = 1 ;
			}
			return;
		} else { /* BFmer Off   &&   Div Off->Div On */
			if (p_dm_path_div->fix_path_bfer == 1) {
				PHYDM_DBG(p_dm, DBG_PATH_DIV, ("[ PathDiv : ON ]   BFmer ==0\n"));
				p_dm_path_div->fix_path_bfer = 0;
			}
		}
	}
#endif
#endif

	if (p_dm_path_div->use_path_a_as_default_ant == 1) {
		phydm_find_default_path(p_dm);
		phydm_candidate_dtp_update(p_dm);
	} else {
		if (p_dm_path_div->phydm_dtp_state == PHYDM_DTP_INIT) {
			phydm_find_default_path(p_dm);
			phydm_candidate_dtp_update(p_dm);
			p_dm_path_div->phydm_dtp_state = PHYDM_DTP_RUNNING_1;
		}

		else	if (p_dm_path_div->phydm_dtp_state == PHYDM_DTP_RUNNING_1) {
			p_dm_path_div->dtp_check_patha_counter++;

			if (p_dm_path_div->dtp_check_patha_counter >= NUM_RESET_DTP_PERIOD) {
				p_dm_path_div->dtp_check_patha_counter = 0;
				p_dm_path_div->phydm_dtp_state = PHYDM_DTP_INIT;
			}
			/* 2 Search space update */
			else {
				/* 1.  find the worst candidate */


				/* 2. repalce the worst candidate */
			}
		}
	}

	/* 2 Dynamic path Selection H2C */

	if (p_dm_path_div->num_candidate == 1)
		return;
	else {
		h2c_parameter[0] =  p_dm_path_div->num_candidate;
		h2c_parameter[1] =  p_dm_path_div->num_tx_path;
		h2c_parameter[2] =  p_dm_path_div->ant_candidate_1;
		h2c_parameter[3] =  p_dm_path_div->ant_candidate_2;
		h2c_parameter[4] =  p_dm_path_div->ant_candidate_3;

		odm_fill_h2c_cmd(p_dm, PHYDM_H2C_DYNAMIC_TX_PATH, 6, h2c_parameter);
	}

}



void phydm_dynamic_tx_path_init(
	void	*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_		*p_dm_path_div  = &(p_dm->dm_path_div);
	struct _ADAPTER		*p_adapter = p_dm->adapter;
	u8			search_space_2[NUM_CHOOSE2_FROM4] = {BB_PATH_AB, BB_PATH_AC, BB_PATH_AD, BB_PATH_BC, BB_PATH_BD, BB_PATH_CD };
	u8			search_space_3[NUM_CHOOSE3_FROM4] = {BB_PATH_BCD, BB_PATH_ACD,  BB_PATH_ABD, BB_PATH_ABC};

#if ((DM_ODM_SUPPORT_TYPE == ODM_WIN) && USB_SWITCH_SUPPORT)
	p_dm_path_div->is_u3_mode = (*p_dm->hub_usb_mode == 2) ? 1 : 0;
	PHYDM_DBG(p_dm, DBG_PATH_DIV, ("[WIN USB] is_u3_mode = (( %d ))\n",
				       p_dm_path_div->is_u3_mode));
#else
	p_dm_path_div->is_u3_mode = 1;
#endif
	PHYDM_DBG(p_dm, DBG_PATH_DIV, ("Dynamic TX path Init 8814\n"));

	memcpy(&(p_dm_path_div->search_space_2[0]), &(search_space_2[0]),
	       NUM_CHOOSE2_FROM4);
	memcpy(&(p_dm_path_div->search_space_3[0]), &(search_space_3[0]),
	       NUM_CHOOSE3_FROM4);

	p_dm_path_div->use_path_a_as_default_ant = 1;
	p_dm_path_div->phydm_dtp_state = PHYDM_DTP_INIT;
	p_dm->path_select = PHYDM_AUTO_PATH;
	p_dm_path_div->phydm_path_div_type = PHYDM_4R_PATH_DIV;


	if (p_dm_path_div->is_u3_mode) {
		p_dm_path_div->num_tx_path = 3;
		phydm_dtp_fix_tx_path(p_dm, BB_PATH_BCD);/* 3TX  Set Init TX path*/

	} else {
		p_dm_path_div->num_tx_path = 2;
		phydm_dtp_fix_tx_path(p_dm, BB_PATH_BC);/* 2TX // Set Init TX path*/
	}

}


void phydm_process_rssi_for_path_div(
	void			*p_dm_void,
	void			*p_phy_info_void,
	void			*p_pkt_info_void
)
{
	struct PHY_DM_STRUCT			*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct phydm_phyinfo_struct		*p_phy_info = (struct phydm_phyinfo_struct *)
			p_phy_info_void;
	struct phydm_perpkt_info_struct	*p_pktinfo = (struct
			phydm_perpkt_info_struct *)p_pkt_info_void;
	struct _ODM_PATH_DIVERSITY_			*p_dm_path_div  = &(p_dm->dm_path_div);

	if (p_pktinfo->is_packet_to_self || p_pktinfo->is_packet_match_bssid) {
		if (p_pktinfo->data_rate > ODM_RATE11M) {
			if (p_dm_path_div->phydm_path_div_type == PHYDM_4R_PATH_DIV) {
#if RTL8814A_SUPPORT
				if (p_dm->support_ic_type & ODM_RTL8814A) {
					p_dm_path_div->path_a_sum_all += p_phy_info->rx_mimo_signal_strength[0];
					p_dm_path_div->path_a_cnt_all++;

					p_dm_path_div->path_b_sum_all += p_phy_info->rx_mimo_signal_strength[1];
					p_dm_path_div->path_b_cnt_all++;

					p_dm_path_div->path_c_sum_all += p_phy_info->rx_mimo_signal_strength[2];
					p_dm_path_div->path_c_cnt_all++;

					p_dm_path_div->path_d_sum_all += p_phy_info->rx_mimo_signal_strength[3];
					p_dm_path_div->path_d_cnt_all++;
				}
#endif
			} else {
				p_dm_path_div->path_a_sum[p_pktinfo->station_id] +=
					p_phy_info->rx_mimo_signal_strength[0];
				p_dm_path_div->path_a_cnt[p_pktinfo->station_id]++;

				p_dm_path_div->path_b_sum[p_pktinfo->station_id] +=
					p_phy_info->rx_mimo_signal_strength[1];
				p_dm_path_div->path_b_cnt[p_pktinfo->station_id]++;
			}
		}
	}


}

#endif /* #if RTL8814A_SUPPORT */

void odm_pathdiv_debug(
	void		*p_dm_void,
	u32		*const dm_value,
	u32		*_used,
	char		*output,
	u32		*_out_len
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_			*p_dm_path_div  = &(p_dm->dm_path_div);
	u32 used = *_used;
	u32 out_len = *_out_len;

	p_dm->path_select = (dm_value[0] & 0xf);
	PHYDM_SNPRINTF((output + used, out_len - used,
			"Path_select = (( 0x%x ))\n", p_dm->path_select));

	/* 2 [Fix path] */
	if (p_dm->path_select != PHYDM_AUTO_PATH) {
		PHYDM_SNPRINTF((output + used, out_len - used,
				"Trun on path  [%s%s%s%s]\n",
				((p_dm->path_select) & 0x1) ? "A" : "",
				((p_dm->path_select) & 0x2) ? "B" : "",
				((p_dm->path_select) & 0x4) ? "C" : "",
				((p_dm->path_select) & 0x8) ? "D" : ""));

		phydm_dtp_fix_tx_path(p_dm, p_dm->path_select);
	} else
		PHYDM_SNPRINTF((output + used, out_len - used, "%s\n", "Auto path"));

	*_used = used;
	*_out_len = out_len;
}

#endif /*  #if(defined(CONFIG_PATH_DIVERSITY)) */

void phydm_c2h_dtp_handler(
	void	*p_dm_void,
	u8   *cmd_buf,
	u8	cmd_len
)
{
#if (defined(CONFIG_PATH_DIVERSITY))
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct _ODM_PATH_DIVERSITY_		*p_dm_path_div  = &(p_dm->dm_path_div);

	u8  macid = cmd_buf[0];
	u8  target = cmd_buf[1];
	u8  nsc_1 = cmd_buf[2];
	u8  nsc_2 = cmd_buf[3];
	u8  nsc_3 = cmd_buf[4];

	PHYDM_DBG(p_dm, DBG_PATH_DIV, ("Target_candidate = (( %d ))\n", target));
	/*
	if( (nsc_1 >= nsc_2) &&  (nsc_1 >= nsc_3))
	{
		phydm_dtp_fix_tx_path(p_dm, p_dm_path_div->ant_candidate_1);
	}
	else	if( nsc_2 >= nsc_3)
	{
		phydm_dtp_fix_tx_path(p_dm, p_dm_path_div->ant_candidate_2);
	}
	else
	{
		phydm_dtp_fix_tx_path(p_dm, p_dm_path_div->ant_candidate_3);
	}
	*/
#endif
}

void odm_path_diversity(
	void	*p_dm_void
)
{
#if (defined(CONFIG_PATH_DIVERSITY))
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	if (!(p_dm->support_ability & ODM_BB_PATH_DIV)) {
		PHYDM_DBG(p_dm, DBG_PATH_DIV, ("Return: Not Support PathDiv\n"));
		return;
	}

#if RTL8812A_SUPPORT

	if (p_dm->support_ic_type & ODM_RTL8812)
		odm_path_diversity_8812a(p_dm);
	else
#endif

#if RTL8814A_SUPPORT
		if (p_dm->support_ic_type & ODM_RTL8814A)
			phydm_dynamic_tx_path(p_dm);
		else
#endif
		{}
#endif
}

void phydm_path_diversity_init(
	void	*p_dm_void
)
{
#if (defined(CONFIG_PATH_DIVERSITY))
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	/*p_dm->support_ability |= ODM_BB_PATH_DIV;*/

	if (*(p_dm->p_mp_mode) == true)
		return;

	if (!(p_dm->support_ability & ODM_BB_PATH_DIV)) {
		PHYDM_DBG(p_dm, DBG_PATH_DIV, ("Return: Not Support PathDiv\n"));
		return;
	}

#if RTL8812A_SUPPORT
	if (p_dm->support_ic_type & ODM_RTL8812)
		odm_path_diversity_init_8812a(p_dm);
	else
#endif

#if RTL8814A_SUPPORT
		if (p_dm->support_ic_type & ODM_RTL8814A)
			phydm_dynamic_tx_path_init(p_dm);
		else
#endif
		{}
#endif
}



#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
/*
 * 2011/12/02 MH Copy from MP oursrc for temporarily test.
 *   */

void odm_path_div_chk_ant_switch_callback(
	struct timer_list		*p_timer
)
{
}

void odm_path_div_chk_ant_switch_workitem_callback(
	void            *p_context
)
{
}

void odm_cck_tx_path_diversity_callback(
	struct timer_list		*p_timer
)
{
}

void odm_cck_tx_path_diversity_work_item_callback(
	void            *p_context
)
{
}
u8 odm_sw_ant_div_select_scan_chnl(
	struct _ADAPTER	*adapter
)
{
	return	0;
}
void odm_sw_ant_div_construct_scan_chnl(
	struct _ADAPTER	*adapter,
	u8		scan_chnl
)
{
}

#endif	/*  #if (DM_ODM_SUPPORT_TYPE == ODM_WIN) */
