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

/*Image2HeaderVersion: 3.1*/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8710B_SUPPORT == 1)
static boolean check_positive(
	struct PHY_DM_STRUCT *p_dm,
	const u32	condition1,
	const u32	condition2,
	const u32	condition3,
	const u32	condition4
)
{
	u8	_board_type = ((p_dm->board_type & BIT(4)) >> 4) << 0 | /* _GLNA*/
			      ((p_dm->board_type & BIT(3)) >> 3) << 1 | /* _GPA*/
			      ((p_dm->board_type & BIT(7)) >> 7) << 2 | /* _ALNA*/
			      ((p_dm->board_type & BIT(6)) >> 6) << 3 | /* _APA */
			      ((p_dm->board_type & BIT(2)) >> 2) << 4;  /* _BT*/

	u32	cond1 = condition1, cond2 = condition2, cond3 = condition3,
		cond4 = condition4;
	u32	driver1 = p_dm->cut_version << 24 |
			  (p_dm->support_interface & 0xF0) << 16 |
			  p_dm->support_platform << 16 |
			  p_dm->package_type << 12 |
			  (p_dm->support_interface & 0x0F) << 8  |
			  _board_type;

	u32	driver2 = (p_dm->type_glna & 0xFF) <<  0 |
			  (p_dm->type_gpa & 0xFF)  <<  8 |
			  (p_dm->type_alna & 0xFF) << 16 |
			  (p_dm->type_apa & 0xFF)  << 24;

	u32	driver3 = 0;

	u32	driver4 = (p_dm->type_glna & 0xFF00) >>  8 |
			  (p_dm->type_gpa & 0xFF00) |
			  (p_dm->type_alna & 0xFF00) << 8 |
			  (p_dm->type_apa & 0xFF00)  << 16;

	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("===> check_positive (cond1, cond2, cond3, cond4) = (0x%X 0x%X 0x%X 0x%X)\n",
		   cond1, cond2, cond3, cond4));
	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("===> check_positive (driver1, driver2, driver3, driver4) = (0x%X 0x%X 0x%X 0x%X)\n",
		   driver1, driver2, driver3, driver4));

	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("	(Platform, Interface) = (0x%X, 0x%X)\n", p_dm->support_platform,
		   p_dm->support_interface));
	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("	(Board, Package) = (0x%X, 0x%X)\n", p_dm->board_type,
		   p_dm->package_type));


	/*============== value Defined Check ===============*/
	/*QFN type [15:12] and cut version [27:24] need to do value check*/

	if (((cond1 & 0x0000F000) != 0)
	    && ((cond1 & 0x0000F000) != (driver1 & 0x0000F000)))
		return false;
	if (((cond1 & 0x0F000000) != 0)
	    && ((cond1 & 0x0F000000) != (driver1 & 0x0F000000)))
		return false;

	/*=============== Bit Defined Check ================*/
	/* We don't care [31:28] */

	cond1 &= 0x00FF0FFF;
	driver1 &= 0x00FF0FFF;

	if ((cond1 & driver1) == cond1) {
		u32	bit_mask = 0;

		if ((cond1 & 0x0F) == 0) /* board_type is DONTCARE*/
			return true;

		if ((cond1 & BIT(0)) != 0) /*GLNA*/
			bit_mask |= 0x000000FF;
		if ((cond1 & BIT(1)) != 0) /*GPA*/
			bit_mask |= 0x0000FF00;
		if ((cond1 & BIT(2)) != 0) /*ALNA*/
			bit_mask |= 0x00FF0000;
		if ((cond1 & BIT(3)) != 0) /*APA*/
			bit_mask |= 0xFF000000;

		if (((cond2 & bit_mask) == (driver2 & bit_mask))
		    && ((cond4 & bit_mask) == (driver4 &
					       bit_mask)))  /* board_type of each RF path is matched*/
			return true;
		else
			return false;
	} else
		return false;
}
static boolean check_negative(
	struct PHY_DM_STRUCT *p_dm,
	const u32	condition1,
	const u32	condition2
)
{
	return true;
}

/******************************************************************************
*                           agc_tab.TXT
******************************************************************************/

u32 array_mp_8710b_agc_tab[] = {
	0xC78, 0xFC000001,
	0xC78, 0xFB010001,
	0xC78, 0xFA020001,
	0xC78, 0xF9030001,
	0xC78, 0xF8040001,
	0xC78, 0xF7050001,
	0xC78, 0xF6060001,
	0xC78, 0xF5070001,
	0xC78, 0xF4080001,
	0xC78, 0xF3090001,
	0xC78, 0xF20A0001,
	0xC78, 0xF10B0001,
	0xC78, 0xF00C0001,
	0xC78, 0xEF0D0001,
	0xC78, 0xEE0E0001,
	0xC78, 0xED0F0001,
	0xC78, 0xEC100001,
	0xC78, 0xEB110001,
	0xC78, 0xEA120001,
	0xC78, 0xE9130001,
	0xC78, 0xE8140001,
	0xC78, 0xE7150001,
	0xC78, 0xE6160001,
	0xC78, 0xE5170001,
	0xC78, 0xE4180001,
	0xC78, 0xE3190001,
	0xC78, 0xE21A0001,
	0xC78, 0xE11B0001,
	0xC78, 0xE01C0001,
	0xC78, 0xC31D0001,
	0xC78, 0xC21E0001,
	0xC78, 0xC11F0001,
	0xC78, 0xC0200001,
	0xC78, 0xA3210001,
	0xC78, 0xA2220001,
	0xC78, 0xA1230001,
	0xC78, 0xA0240001,
	0xC78, 0x86250001,
	0xC78, 0x85260001,
	0xC78, 0x84270001,
	0xC78, 0x83280001,
	0xC78, 0x82290001,
	0xC78, 0x812A0001,
	0xC78, 0x802B0001,
	0xC78, 0x632C0001,
	0xC78, 0x622D0001,
	0xC78, 0x612E0001,
	0xC78, 0x602F0001,
	0xC78, 0x42300001,
	0xC78, 0x41310001,
	0xC78, 0x40320001,
	0xC78, 0x23330001,
	0xC78, 0x22340001,
	0xC78, 0x21350001,
	0xC78, 0x20360001,
	0xC78, 0x02370001,
	0xC78, 0x01380001,
	0xC78, 0x00390001,
	0xC78, 0x003A0001,
	0xC78, 0x003B0001,
	0xC78, 0x003C0001,
	0xC78, 0x003D0001,
	0xC78, 0x003E0001,
	0xC78, 0x003F0001,
	0xC78, 0xF7400001,
	0xC78, 0xF7410001,
	0xC78, 0xF7420001,
	0xC78, 0xF7430001,
	0xC78, 0xF7440001,
	0xC78, 0xF7450001,
	0xC78, 0xF7460001,
	0xC78, 0xF7470001,
	0xC78, 0xF7480001,
	0xC78, 0xF6490001,
	0xC78, 0xF34A0001,
	0xC78, 0xF24B0001,
	0xC78, 0xF14C0001,
	0xC78, 0xF04D0001,
	0xC78, 0xD14E0001,
	0xC78, 0xD04F0001,
	0xC78, 0xB5500001,
	0xC78, 0xB4510001,
	0xC78, 0xB3520001,
	0xC78, 0xB2530001,
	0xC78, 0xB1540001,
	0xC78, 0xB0550001,
	0xC78, 0xAF560001,
	0xC78, 0xAE570001,
	0xC78, 0xAD580001,
	0xC78, 0xAC590001,
	0xC78, 0xAB5A0001,
	0xC78, 0xAA5B0001,
	0xC78, 0xA95C0001,
	0xC78, 0xA85D0001,
	0xC78, 0xA75E0001,
	0xC78, 0xA65F0001,
	0xC78, 0xA5600001,
	0xC78, 0xA4610001,
	0xC78, 0xA3620001,
	0xC78, 0xA2630001,
	0xC78, 0xA1640001,
	0xC78, 0xA0650001,
	0xC78, 0x87660001,
	0xC78, 0x86670001,
	0xC78, 0x85680001,
	0xC78, 0x84690001,
	0xC78, 0x836A0001,
	0xC78, 0x826B0001,
	0xC78, 0x816C0001,
	0xC78, 0x806D0001,
	0xC78, 0x636E0001,
	0xC78, 0x626F0001,
	0xC78, 0x61700001,
	0xC78, 0x60710001,
	0xC78, 0x42720001,
	0xC78, 0x41730001,
	0xC78, 0x40740001,
	0xC78, 0x23750001,
	0xC78, 0x22760001,
	0xC78, 0x21770001,
	0xC78, 0x20780001,
	0xC78, 0x03790001,
	0xC78, 0x027A0001,
	0xC78, 0x017B0001,
	0xC78, 0x007C0001,
	0xC78, 0x007D0001,
	0xC78, 0x007E0001,
	0xC78, 0x007F0001,
	0xC50, 0x69553422,
	0xC50, 0x69553420,

};

void odm_read_and_config_mp_8710b_agc_tab(
	struct	PHY_DM_STRUCT *p_dm
)
{
	u32	i = 0;
	u8	c_cond;
	boolean	is_matched = true, is_skipped = false;
	u32	array_len = sizeof(array_mp_8710b_agc_tab) / sizeof(u32);
	u32	*array = array_mp_8710b_agc_tab;

	u32	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("===> odm_read_and_config_mp_8710b_agc_tab\n"));

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 & (BIT(31) | BIT(30))) {/*positive & negative condition*/
			if (v1 & BIT(31)) {/* positive condition*/
				c_cond  = (u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) {/*end*/
					is_matched = true;
					is_skipped = false;
					PHYDM_DBG(p_dm, ODM_COMP_INIT, ("ENDIF\n"));
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					PHYDM_DBG(p_dm, ODM_COMP_INIT, ("ELSE\n"));
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					PHYDM_DBG(p_dm, ODM_COMP_INIT, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (is_skipped == false) {
					if (check_positive(p_dm, pre_v1, pre_v2, v1, v2)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else
					is_matched = false;
			}
		} else {
			if (is_matched)
				odm_config_bb_agc_8710b(p_dm, v1, MASKDWORD, v2);
		}
		i = i + 2;
	}
}

u32 odm_get_version_mp_8710b_agc_tab(void)
{
	return 13;
}

/******************************************************************************
*                           phy_reg.TXT
******************************************************************************/

u32 array_mp_8710b_phy_reg[] = {
	0x800, 0x80045700,
	0x804, 0x00000001,
	0x808, 0x00FC8000,
	0x80C, 0x0000000A,
	0x810, 0x10001331,
	0x814, 0x020C3D10,
	0x818, 0x00200385,
	0x81C, 0x00000000,
	0x820, 0x01000100,
	0x824, 0x00390204,
	0x828, 0x00000000,
	0x82C, 0x00000000,
	0x830, 0x00000000,
	0x834, 0x00000000,
	0x838, 0x00000000,
	0x83C, 0x00000000,
	0x840, 0x00010000,
	0x844, 0x00000000,
	0x848, 0x00000000,
	0x84C, 0x00000000,
	0x850, 0x00030000,
	0x854, 0x00000000,
	0x858, 0x7E1A569A,
	0x85C, 0x569A569A,
	0x860, 0x00000130,
	0x864, 0x20000000,
	0x868, 0x00000000,
	0x86C, 0x27272700,
	0x870, 0x00050000,
	0x874, 0x25005000,
	0x878, 0x00000808,
	0x87C, 0x004F0201,
	0x880, 0xB0000B1E,
	0x884, 0x00000007,
	0x888, 0x00000000,
	0x88C, 0xCCC400C0,
	0x890, 0x00000800,
	0x894, 0xFFFFFFFE,
	0x898, 0x40302010,
	0x89C, 0x00706050,
	0x900, 0x00000000,
	0x904, 0x00000023,
	0x908, 0x00000000,
	0x90C, 0x81121111,
	0x910, 0x00000402,
	0x914, 0x00000201,
	0x920, 0x18C6318C,
	0x924, 0x0000018C,
	0x948, 0x99000000,
	0x94C, 0x00000010,
	0x950, 0x00003000,
	0x954, 0x5A880000,
	0x958, 0x4BC6D87A,
	0x95C, 0x04EB9B79,
	0x96C, 0x00000003,
	0x970, 0x00000000,
	0x974, 0x00000000,
	0x978, 0x00000000,
	0x97C, 0x13000000,
	0x980, 0x00000000,
	0xA00, 0x00D046C8,
	0xA04, 0x80FF800C,
	0xA08, 0x84838300,
	0x80001000,	0x00000000,	0x40000000,	0x00000000,
	0xA0C, 0x2A20100F,
	0x90002000,	0x00000000,	0x40000000,	0x00000000,
	0xA0C, 0x2A20100F,
	0x90003000,	0x00000000,	0x40000000,	0x00000000,
	0xA0C, 0x2A20100F,
	0xA0000000,	0x00000000,
	0xA0C, 0x2E20100F,
	0xB0000000,	0x00000000,
	0xA10, 0x9500BB78,
	0xA14, 0x1114D028,
	0xA18, 0x00881117,
	0xA1C, 0x89140F00,
	0xA20, 0xE82C0001,
	0xA24, 0x64B80C1C,
	0xA28, 0x00008810,
	0xA2C, 0x00D30000,
	0xA70, 0x101FBF00,
	0xA74, 0x00000007,
	0xA78, 0x00000900,
	0xA7C, 0x225B0606,
	0xA80, 0x218075B1,
	0xA84, 0x00200000,
	0xA88, 0x040C0000,
	0xA8C, 0x12345678,
	0xA90, 0xABCDEF00,
	0xA94, 0x001B1B89,
	0xA98, 0x00000000,
	0xA9C, 0x80020000,
	0xAA0, 0x00000000,
	0xAA4, 0x0000000C,
	0xAA8, 0xCA110058,
	0xAAC, 0x01235667,
	0xAB0, 0x00000000,
	0xAB4, 0x20201402,
	0xB2C, 0x00000000,
	0xC00, 0x48071D40,
	0xC04, 0x03A05611,
	0xC08, 0x000000E4,
	0xC0C, 0x6C6C6C6C,
	0xC10, 0x18800000,
	0xC14, 0x40000100,
	0xC18, 0x08800000,
	0xC1C, 0x40000100,
	0xC20, 0x00000000,
	0xC24, 0x00000000,
	0xC28, 0x00000000,
	0xC2C, 0x00000000,
	0xC30, 0x69E9AC4A,
	0xC34, 0x31000040,
	0xC38, 0x21688080,
	0xC3C, 0x0000170C,
	0xC40, 0x1F78403F,
	0xC44, 0x00010036,
	0xC48, 0xEC020107,
	0xC4C, 0x007F037F,
	0xC50, 0x69553420,
	0xC54, 0x43BC0094,
	0xC58, 0x00013169,
	0xC5C, 0x00250492,
	0xC60, 0x00280A00,
	0xC64, 0x7112848B,
	0x80002000,	0x00000000,	0x40000000,	0x00000000,
	0xC68, 0x47C074FF,
	0x90003000,	0x00000000,	0x40000000,	0x00000000,
	0xC68, 0x47C074FF,
	0xA0000000,	0x00000000,
	0xC68, 0x47C074FF,
	0xB0000000,	0x00000000,
	0xC6C, 0x00000036,
	0xC70, 0x2C7F000D,
	0xC74, 0x020600DB,
	0xC78, 0x0000001F,
	0xC7C, 0x00B91612,
	0xC80, 0x390000E4,
	0xC84, 0x11F60000,
	0xC88, 0x1051B75F,
	0xC8C, 0x20200109,
	0xC90, 0x00091521,
	0xC94, 0x00000000,
	0xC98, 0x00121820,
	0xC9C, 0x00007F7F,
	0xCA0, 0x00011000,
	0xCA4, 0x800000A0,
	0xCA8, 0x84E6C606,
	0xCAC, 0x00000060,
	0xCB0, 0x00000000,
	0xCB4, 0x00000000,
	0xCB8, 0x00000000,
	0xCBC, 0x28000000,
	0xCC0, 0x1051B75F,
	0xCC4, 0x00000109,
	0xCC8, 0x000442D6,
	0xCCC, 0x00000000,
	0xCD0, 0x000001C8,
	0xCD4, 0x001C8000,
	0xCD8, 0x00000100,
	0xCDC, 0x40100000,
	0xCE0, 0x00222220,
	0xCE4, 0x10000000,
	0xCE8, 0x37644302,
	0xCEC, 0x2F97D40C,
	0xD00, 0x04030740,
	0xD04, 0x40020401,
	0xD08, 0x0000907F,
	0xD0C, 0x20010201,
	0xD10, 0xA0633333,
	0xD14, 0x3333BC53,
	0xD18, 0x7A8F5B6F,
	0xD2C, 0xCB979975,
	0xD30, 0x00000000,
	0xD34, 0x40608000,
	0xD38, 0x88000000,
	0xD3C, 0xC0127353,
	0xD40, 0x00000000,
	0xD44, 0x00000000,
	0xD48, 0x00000000,
	0xD4C, 0x00000000,
	0xD50, 0x00006528,
	0xD54, 0x00000000,
	0xD58, 0x00000282,
	0xD5C, 0x30032064,
	0xD60, 0x4653DE68,
	0xD64, 0x04518A3C,
	0xD68, 0x00002101,
	0xE00, 0x2D2D2D2D,
	0xE04, 0x2D2D2D2D,
	0xE08, 0x0390272D,
	0xE10, 0x2D2D2D2D,
	0xE14, 0x2D2D2D2D,
	0xE18, 0x2D2D2D2D,
	0xE1C, 0x2D2D2D2D,
	0xE28, 0x00000000,
	0xE30, 0x1000DC1F,
	0xE34, 0x10008C1F,
	0xE38, 0x02140102,
	0xE3C, 0x681604C2,
	0xE40, 0x01007C00,
	0xE44, 0x01004800,
	0xE48, 0xFB000000,
	0xE4C, 0x000028D1,
	0xE50, 0x1000DC1F,
	0xE54, 0x10008C1F,
	0xE58, 0x02140102,
	0xE5C, 0x28160D05,
	0xE60, 0x0000C008,
	0xE68, 0x001B25A4,
	0xE64, 0x281600A0,
	0xE6C, 0x01C00010,
	0xE70, 0x01C00010,
	0xE74, 0x02000010,
	0xE78, 0x02000010,
	0xE7C, 0x02000010,
	0xE80, 0x02000010,
	0xE84, 0x01C00010,
	0xE88, 0x02000010,
	0xE8C, 0x01C00010,
	0xED0, 0x01C00010,
	0xED4, 0x01C00010,
	0xED8, 0x01C00010,
	0xEDC, 0x00000010,
	0xEE0, 0x00000010,
	0xEEC, 0x03C00010,
	0xF14, 0x00000003,
	0xF00, 0x00100300,
	0xF08, 0x0000800B,
	0xF0C, 0x0000F007,
	0xF10, 0x0000A487,
	0xF1C, 0x80000064,
	0xF38, 0x00030155,
	0xF3C, 0x0000003A,
	0xF4C, 0x13000000,
	0xF50, 0x00000000,
	0xF18, 0x00000000,

};

void odm_read_and_config_mp_8710b_phy_reg(
	struct	PHY_DM_STRUCT *p_dm
)
{
	u32	i = 0;
	u8	c_cond;
	boolean	is_matched = true, is_skipped = false;
	u32	array_len = sizeof(array_mp_8710b_phy_reg) / sizeof(u32);
	u32	*array = array_mp_8710b_phy_reg;

	u32	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("===> odm_read_and_config_mp_8710b_phy_reg\n"));

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 & (BIT(31) | BIT(30))) {/*positive & negative condition*/
			if (v1 & BIT(31)) {/* positive condition*/
				c_cond  = (u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) {/*end*/
					is_matched = true;
					is_skipped = false;
					PHYDM_DBG(p_dm, ODM_COMP_INIT, ("ENDIF\n"));
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					PHYDM_DBG(p_dm, ODM_COMP_INIT, ("ELSE\n"));
				} else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					PHYDM_DBG(p_dm, ODM_COMP_INIT, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (is_skipped == false) {
					if (check_positive(p_dm, pre_v1, pre_v2, v1, v2)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else
					is_matched = false;
			}
		} else {
			if (is_matched)
				odm_config_bb_phy_8710b(p_dm, v1, MASKDWORD, v2);
		}
		i = i + 2;
	}
}

u32 odm_get_version_mp_8710b_phy_reg(void)
{
	return 13;
}

/******************************************************************************
*                           phy_reg_pg.TXT
******************************************************************************/

u32 array_mp_8710b_phy_reg_pg[] = {
	0, 0, 0, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 0, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 0, 0x00000e00, 0xffffffff, 0x34363636,
	0, 0, 0, 0x00000e04, 0xffffffff, 0x28303234,
	0, 0, 0, 0x00000e10, 0xffffffff, 0x30343434,
	0, 0, 0, 0x00000e14, 0xffffffff, 0x26262830
};

void odm_read_and_config_mp_8710b_phy_reg_pg(
	struct PHY_DM_STRUCT	*p_dm
)
{
	u32	i = 0;
	u32	array_len = sizeof(array_mp_8710b_phy_reg_pg) / sizeof(u32);
	u32	*array = array_mp_8710b_phy_reg_pg;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	struct _ADAPTER	*adapter = p_dm->adapter;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(adapter);

	PlatformZeroMemory(p_hal_data->BufOfLinesPwrByRate,
			   MAX_LINES_HWCONFIG_TXT * MAX_BYTES_LINE_HWCONFIG_TXT);
	p_hal_data->nLinesReadPwrByRate = array_len / 6;
#endif

	PHYDM_DBG(p_dm, ODM_COMP_INIT,
		  ("===> odm_read_and_config_mp_8710b_phy_reg_pg\n"));

	p_dm->phy_reg_pg_version = 1;
	p_dm->phy_reg_pg_value_type = PHY_REG_PG_EXACT_VALUE;

	for (i = 0; i < array_len; i += 6) {
		u32	v1 = array[i];
		u32	v2 = array[i + 1];
		u32	v3 = array[i + 2];
		u32	v4 = array[i + 3];
		u32	v5 = array[i + 4];
		u32	v6 = array[i + 5];

		odm_config_bb_phy_reg_pg_8710b(p_dm, v1, v2, v3, v4, v5, v6);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		rsprintf((char *)p_hal_data->BufOfLinesPwrByRate[i / 6], 100,
			 "%s, %s, %s, 0x%X, 0x%08X, 0x%08X,",
			 (v1 == 0 ? "2.4G" : "  5G"), (v2 == 0 ? "A" : "B"),
			 (v3 == 0 ? "1Tx" : "2Tx"), v4, v5, v6);
#endif
	}
}



#endif /* end of HWIMG_SUPPORT*/

