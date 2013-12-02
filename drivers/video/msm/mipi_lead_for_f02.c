/* Copyright (c) 2008-2011, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_lead.h"
#include <mach/gpio.h>
#ifdef CONFIG_ZTE_PLATFORM
#include <mach/zte_memlog.h>
#endif

static struct dsi_buf lead_tx_buf;
static struct dsi_buf lead_rx_buf;
extern u32 LcdPanleID;
extern void mipi_zte_set_backlight(struct msm_fb_data_type *mfd);
//[ECID 000000] zhangqi add for CABC begin
#ifdef CONFIG_BACKLIGHT_CABC
void mipi_set_backlight(struct msm_fb_data_type *mfd);
#endif
//[ECID 000000] zhangqi add for CABC end


// #ifdef CONFIG_FB_MSM_GPIO
#define GPIO_LCD_RESET 129
// #else
// #define GPIO_LCD_RESET 84
// #endif

//[ECID:0000] ZTEBSP wangminrong start 20120411 for lcd jianrong 
#define CONFIG_LCD_READ_ID 0 //[ECID:0000] ZTEBSP wangminrong 20120730 for lcd read id 
/*ic define*/
#define HIMAX_8363 		1
#define HIMAX_8369 		2
#define NOVATEK_35510	3
#define RENESAS_R61408	4

#define HIMAX8369_TIANMA_TN_ID		0xB1
#define HIMAX8369_TIANMA_IPS_ID		0xA5
#define HIMAX8369_LEAD_ID				0
#define HIMAX8369_LEAD_HANNSTAR_ID	0x88
#define NT35510_YUSHUN_ID				0
#define NT35510_LEAD_ID				0xA0
#define HIMAX8369_YUSHUN_IVO_ID       0x85

/*about icchip sleep and display on */
static char display_off[2] = {0x28, 0x00};
static char enter_sleep[2] = {0x10, 0x00};
static char exit_sleep[2] = {0x11, 0x00};
static char display_on[2] = {0x29, 0x00};
static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(enter_sleep), enter_sleep}
};


/*about himax8363 chip id */
static char hx8363_setpassword_para[4]={0xB9,0xFF,0x83,0x63};
//static char hx8363_icid_rd_para[2] = {0xB9, 0x00}; 
//static char hx8363_panleid_rd_para[2] = {0xda, 0x00};    
   
/*static struct dsi_cmd_desc hx8363_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8363_icid_rd_para), hx8363_icid_rd_para
};
*/
/*
static struct dsi_cmd_desc hx8363_setpassword_cmd[] = 
{	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8363_setpassword_para),hx8363_setpassword_para},

};


static struct dsi_cmd_desc hx8363_panleid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8363_panleid_rd_para), hx8363_panleid_rd_para
};
*/
/*about himax8369 chip id */
static char hx8369_setpassword_para[4]={0xB9,0xFF,0x83,0x69};
//static char hx8369_icid_rd_para[2] = {0xB9, 0x00}; 
//static char hx8369_panleid_rd_para[2] = {0xda, 0x00};    

/*
static struct dsi_cmd_desc hx8369_icid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8369_icid_rd_para), hx8369_icid_rd_para
};
*/
//static struct dsi_cmd_desc hx8369_setpassword_cmd[] = 
//{	
//	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},

//};
/*
static struct dsi_cmd_desc hx8369_panleid_rd_cmd = 
{
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(hx8369_panleid_rd_para), hx8369_panleid_rd_para
};
*/

/*about Novatek3511 chip id */
//static char nt3511_page_ff[5] = {0xff, 0xaa,0x55,0x25,0x01};
//static char nt3511_page_f8[20] = {0xf8, 0x01,0x12,0x00,0x20,0x33,0x13,0x00,0x40,0x00,0x00,0x23,0x01,0x99,0xc8,0x00,0x00,0x01,0x00,0x00};
//static char nt3511_icid_rd_para[2] = {0xc5, 0x00}; 
//static char nt3511_panleid_rd_para[2] = {0xDA, 0x00};    //added by zte_gequn091966 for lead_nt35510,20111226

//static struct dsi_cmd_desc nt3511_setpassword_cmd[] = {	
//	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nt3511_page_ff),nt3511_page_ff},
//	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nt3511_page_f8),nt3511_page_f8}
//};
/*
static struct dsi_cmd_desc nt3511_icid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt3511_icid_rd_para), nt3511_icid_rd_para};


static struct dsi_cmd_desc nt3511_panleid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt3511_panleid_rd_para), nt3511_panleid_rd_para
};   //added by zte_gequn091966 for lead_nt35510,20111226
*/

/*about RENESAS r61408 chip id */
//static char r61408_setpassword_para[2]={0xb0,0x04};
//static struct dsi_cmd_desc r61408_setpassword_cmd[] = 
//{	
//	{DTYPE_GEN_LWRITE, 1, 0, 0, 1, sizeof(r61408_setpassword_para),r61408_setpassword_para},

//};
/*static char r61408_icid_rd_para[2] = {0xbf, 0x00}; 
static struct dsi_cmd_desc r61408_icid_rd_cmd = 
{
	DTYPE_GEN_READ1, 1, 0, 0, 1, sizeof(r61408_icid_rd_para), r61408_icid_rd_para
};
*/
/**************************************
0. CABC start 
**************************************/
//[ECID 000000] zhangqi add for CABC begin
extern  unsigned int lcd_id_type;

#ifdef CONFIG_BACKLIGHT_CABC
static char hx8369_para_CABC_0xc9[10]={0xc9,0x3e,0x00,0x00,0x01,0x2f,0x00,0x1e,0x1e,0x00};//{0xC9,0x3e,0x30,0x00,0x01,0x2f,0x00,0x1e,0x1e,0x00};
static char hx8369_para_CABC_0x51[2]={0x51,0xff};
static char hx8369_para_CABC_0x53[2]={0x53,0x2c};
static char hx8369_para_CABC_0x55[2]={0x55,0x00};
static struct dsi_cmd_desc hx8369_display_on_CABC_backlight_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_para_CABC_0xc9), hx8369_para_CABC_0xc9},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_para_CABC_0x51), hx8369_para_CABC_0x51},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_para_CABC_0x53), hx8369_para_CABC_0x53},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_para_CABC_0x55), hx8369_para_CABC_0x55},
};

static char hx8369_para_CABC_0x53_off[2]={0x53,0x0c};

static struct dsi_cmd_desc hx8369_display_off_CABC_backlight_cmds[] = {
	{DTYPE_DCS_WRITE1, 1, 0, 0, 100, sizeof(hx8369_para_CABC_0x53_off), hx8369_para_CABC_0x53_off},

};


static char hx8363_para_CABC_0xc9[4]={0xc9,0x0f,0x3e,0x01};
static char hx8363_para_CABC_0x51[2]={0x51,0xff};
static char hx8363_para_CABC_0x53[2]={0x53,0x2c};
static char hx8363_para_CABC_0x55[2]={0x55,0x03};//zhangqi add for move mode 2012.7.26
static struct dsi_cmd_desc hx8363_display_on_CABC_backlight_cmds[] = {
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_para_CABC_0xc9), hx8363_para_CABC_0xc9},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_para_CABC_0x51), hx8363_para_CABC_0x51},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_para_CABC_0x53), hx8363_para_CABC_0x53},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_para_CABC_0x55), hx8363_para_CABC_0x55},
};

//static char hx8363_para_CABC_0x51_off[2]={0x51,0x00};
static char hx8363_para_CABC_0x53_off[2]={0x53,0x0c};
//static char hx8363_para_CABC_0x55_off[2]={0x55,0x00};//zhangqi add for move mode 2012.7.26
static struct dsi_cmd_desc hx8363_display_off_CABC_backlight_cmds[] = {
//	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_para_CABC_0xc9), hx8363_para_CABC_0xc9},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_para_CABC_0x51_off), hx8363_para_CABC_0x51_off},
//	{DTYPE_DCS_WRITE1, 1, 0, 0, 100, sizeof(hx8363_para_CABC_0x55_off), hx8363_para_CABC_0x55_off},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 100, sizeof(hx8363_para_CABC_0x53_off), hx8363_para_CABC_0x53_off},

};

#endif 
//[ECID 000000] zhangqi add for CABC end


/**************************************
1. hx8363 yassy start 
**************************************/
static char hx8363_yassy_para_0xb1[13]={0xB1,0x78,0x34,0x08,0x34,0x02,0x13,
								0x11,0x11,0x2d,0x35,0x3F,0x3F};  
static char hx8363_yassy_para_0xba[14]={0xBA,0x80,0x00,0x10,0x08,0x08,0x10,0x7c,0x6e,
								0x6d,0x0a,0x01,0x84,0x43};   //TWO LANE
static char hx8363_yassy_para_0x3a[2]={0x3a,0x77};
//static char hx8363_para_0x36[2]={0x36,0x0a};
static char hx8363_yassy_para_0xb2[4]={0xb2,0x33,0x33,0x22};
static char hx8363_yassy_para_0xb3[2]={0xb3,0x00};
static char hx8363_yassy_para_0xb4[10]={0xb4,0x08,0x12,0x72,0x12,0x06,0x03,0x54,0x03,0x4e};
static char hx8363_yassy_para_0xb6[2]={0xb6,0x2c};
static char hx8363_yassy_para_0xcc[2]={0xcc,0x09};
static char hx8363_yassy_para_0xe0[31]={0xe0,0x01,0x09,0x17,0x10,0x10,0x3e,0x07,
	0x8d,0x90,0x54,0x16,0xd5,0x55,0x53,0x19,0x01,0x09,0x17,0x10,0x10,0x3e,0x07,
	0x8d,0x90,0x54,0x16,0xd5,0x55,0x53,0x19};	

static struct dsi_cmd_desc hx8363_yassy_display_on_cmds[] = 
{

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_setpassword_para),hx8363_setpassword_para},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb1), hx8363_yassy_para_0xb1},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb2), hx8363_yassy_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xba), hx8363_yassy_para_0xba},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0x3a), hx8363_yassy_para_0x3a},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb3), hx8363_yassy_para_0xb3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb4), hx8363_yassy_para_0xb4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xb6), hx8363_yassy_para_0xb6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xcc), hx8363_yassy_para_0xcc},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 10, sizeof(hx8363_para_0x36), hx8363_para_0x36},		
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yassy_para_0xe0), hx8363_yassy_para_0xe0},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8363_para_0xc1), hx8363_para_0xc1},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(hx8363_para_0xc2), hx8363_para_0xc2},	
	

	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};



/**************************************
2. hx8369 lead start 
**************************************/
static char hx8369_lead_tn_para_0xb0[3]={0xb0,0x01,0x09};
static char hx8369_lead_tn_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x07,0x00,0x0F,0x0F,
	0x21,0x28,0x3F,0x3F,0x07,0x23,0x01,0xE6,0xE6,0xE6,0xE6,0xE6};  
static char hx8369_lead_tn_para_0xb2[16]={0xB2,0x00,0x23,0x0A,0x0A,0x70,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //VIDEO MODE
//static char para_0xb2[16]={0xB2,0x00,0x20,0x0A,0x0A,0x70,0x00,0xFF,
//	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //CMD MODE
static char hx8369_lead_tn_para_0xb4[6]={0xB4,0x00,0x0C,0x84,0x0C,0x01}; 
static char hx8369_lead_tn_para_0xb6[3]={0xB6,0x2c,0x2c};
static char hx8369_lead_tn_para_0xd5[27]={0xD5,0x00,0x05,0x03,0x00,0x01,0x09,0x10,
	0x80,0x37,0x37,0x20,0x31,0x46,0x8A,0x57,0x9B,0x20,0x31,0x46,0x8A,
	0x57,0x9B,0x07,0x0F,0x07,0x00}; 
static char hx8369_lead_tn_para_0xe0[35]={0xE0,0x00,0x06,0x06,0x29,0x2d,0x3F,0x13,0x32,
	0x08,0x0c,0x0D,0x11,0x14,0x11,0x14,0x0e,0x15,0x00,0x06,0x06,0x29,0x2d,
	0x3F,0x13,0x32,0x08,0x0c,0x0D,0x11,0x14,0x11,0x14,0x0e,0x15};
static char hx8369_lead_tn_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_lead_tn_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,
	0x6C,0x02,0x11,0x18,0x40};   //TWO LANE
//static char para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,
	//0x6C,0x02,0x10,0x18,0x40};   //ONE LANE

static struct dsi_cmd_desc hx8369_lead_display_on_cmds[] = {
	 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb0), hx8369_lead_tn_para_0xb0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb1), hx8369_lead_tn_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb2), hx8369_lead_tn_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb4), hx8369_lead_tn_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xb6), hx8369_lead_tn_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xd5), hx8369_lead_tn_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xe0), hx8369_lead_tn_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0x3a), hx8369_lead_tn_para_0x3a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_tn_para_0xba), hx8369_lead_tn_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};
/**************************************
3. HANNSTAR hx8369 lead start 
**************************************/


static char hx8369_lead_hannstar_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x07,0x00,0x0E,0x0E,
	0x21,0x29,0x3F,0x3F,0x01,0x63,0x01,0xE6,0xE6,0xE6,0xE6,0xE6};  
static char hx8369_lead_hannstar_para_0xb2[16]={0xB2,0x00,0x23,0x07,0x07,0x70,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //VIDEO MODE
static char hx8369_lead_hannstar_para_0xb4[6]={0xB4,0x02,0x18,0x80,0x10,0x01}; 
static char hx8369_lead_hannstar_para_0xb6[3]={0xB6,0x1F,0x1F};
static char hx8369_lead_hannstar_para_0xcc[2]={0xcc,0x00}; 


static char hx8369_lead_hannstar_para_0xd5[27]={0xD5,0x00,0x07,0x00,0x00,0x01,0x0a,0x10,
	0x60,0x33,0x37,0x23,0x01,0xB9,0x75,0xA8,0x64,0x00,0x00,0x41,0x06,
	0x50,0x07,0x07,0x0F,0x07,0x00}; 
static char hx8369_lead_hannstar_para_0xe0[35]={0xE0,0x00,0x03,0x00,0x09,0x09,0x21,0x1B,0x2D,
	0x06,0x0c,0x10,0x15,0x16,0x14,0x16,0x12,0x18,0x00,0x03,0x00,0x09,0x09,
	0x21,0x1B,0x2D,0x06,0x0c,0x10,0x15,0x16,0x14,0x16,0x12,0x18};
static char hx8369_lead_hannstar_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_lead_hannstar_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x02,0x10,0x30,
	0x6F,0x02,0x11,0x18,0x40};   //TWO LANE


static struct dsi_cmd_desc hx8369_lead_hannstar_display_on_cmds[] = {
	 
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb1), hx8369_lead_hannstar_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb2), hx8369_lead_hannstar_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb4), hx8369_lead_hannstar_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xb6), hx8369_lead_hannstar_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xcc), hx8369_lead_hannstar_para_0xcc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xd5), hx8369_lead_hannstar_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xe0), hx8369_lead_hannstar_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0x3a), hx8369_lead_hannstar_para_0x3a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_lead_hannstar_para_0xba), hx8369_lead_hannstar_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};

/**************************************
4. hx8369 tianma TN start 
**************************************/
static char hx8369_tianma_tn_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x0A,0x00,0x11,0x12,0x21,0x29,0x3F,0x3F,
	0x01,0x1a,0x01,0xE6,0xE6,0xE6,0xE6,0xE6}; 
static char hx8369_tianma_tn_para_0xb2[16]={0xB2,0x00,0x23,0x03,0x03,0x70,0x00,0xFF,0x00,0x00,0x00,0x00,
	0x03,0x03,0x00,0x01};  //VIDEO MODE
//static char hx8369_tianma_tn_para_0xb4[6]={0xB4,0x02,0x18,0x70,0x0f,0x01}; //wangminrong add for lcd white noize 20120510
static char hx8369_tianma_tn_para_0xb4[6]={0xB4,0x02,0x18,0x70,0x13,0x05};
static char hx8369_tianma_tn_para_0xb6[3]={0xB6,0x4a,0x4a};
//static char hx8369_tianma_tn_para_0xd5[27]={0xD5,0x00,0x09,0x03,0x29,0x01,0x0a,0x28,0x60,0x11,0x13,0x00,
//	0x00,0x40,0x26,0x51,0x37,0x00,0x00,0x71,0x35,0x60,0x24,0x07,0x0F,0x04,0x04}; 
static char hx8369_tianma_tn_para_0xd5[27]={0xD5,0x00,0x09,0x03,0x2B,0x01,0x0C,0x28,0x60,0x11,0x13,0x00,
	0x00,0x60,0xC4,0x71,0xC5,0x00,0x00,0x71,0x05,0x60,0x04,0x07,0x0F,0x04,0x04};  //wangminrong add for lcd white noize 20120510
static char hx8369_tianma_tn_para_0xe0[35]={0xE0,0x00,0x02,0x0b,0x0a,0x09,0x18,0x1d,0x2a,0x08,0x11,0x0d,
	0x13,0x15,0x14,0x15,0x0f,0x14,0x00,0x02,0x0b,0x0a,0x09,0x18,0x1d,0x2a,0x08,0x11,0x0d,0x13,0x15,
	0x14,0x15,0x0f,0x14};
static char hx8369_tianma_tn_para_0xcc[2]={0xcc,0x00}; 
static char hx8369_tianma_tn_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_tianma_tn_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6F,0x02,0x11,0x18,0x40};   //TWO LANE
static struct dsi_cmd_desc hx8369_tianma_tn_display_on_cmds[] = 
{

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb1), hx8369_tianma_tn_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb2), hx8369_tianma_tn_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb4), hx8369_tianma_tn_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xb6), hx8369_tianma_tn_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xd5),hx8369_tianma_tn_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xe0), hx8369_tianma_tn_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0x3a), hx8369_tianma_tn_para_0x3a},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xcc), hx8369_tianma_tn_para_0xcc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_tn_para_0xba), hx8369_tianma_tn_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};
/**************************************
5. hx8369 tianma IPS start 
**************************************/
static char hx8369_tianma_ips_para_0xb1[20]={0xB1,0x01,0x00,0x34,0x07,0x00,0x0D,0x0D,0x1A,0x22,0x3F,
	0x3F,0x01,0x23,0x01,0xE6,0xE6,0xE6,0xE6,0xE6}; 
static char hx8369_tianma_ips_para_0xb2[16]={0xB2,0x00,0x23,0x05,0x05,0x70,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x01};  //VIDEO MODE
static char hx8369_tianma_ips_para_0xb4[6]={0xB4,0x00,0x18,0x80,0x06,0x02}; 
static char hx8369_tianma_ips_para_0xb6[3]={0xB6,0x42,0x42};
static char hx8369_tianma_ips_para_0xd5[27]={0xD5,0x00,0x09,0x03,0x29,0x01,0x0A,0x28,0x70,
	0x11,0x13,0x00,0x00,0x40,0x26,0x51,0x37,0x00,0x00,0x71,0x35,0x60,0x24,0x07,0x0F,0x04,0x04}; 
static char hx8369_tianma_ips_para_0xe0[35]={0xE0,0x00,0x0A,0x0F,0x2E,0x33,0x3F,0x1D,0x3E,0x07,0x0D,0x0F,
	0x12,0x15,0x13,0x15,0x10,0x17,0x00,0x0A,0x0F,0x2E,0x33,0x3F,0x1D,0x3E,0x07,0x0D,
	0x0F,0x12,0x15,0x13,0x15,0x10,0x17};
static char hx8369_tianma_ips_para_0x3a[2]={0x3A,0x77}; 
static char hx8369_tianma_ips_para_0xba[14]={0xBA,0x00,0xA0,0xC6,0x00,0x0A,0x00,0x10,0x30,0x6F,0x02,0x11,0x18,0x40};   //TWO LANE

static struct dsi_cmd_desc hx8369_tianma_ips_display_on_cmds[] = 
{

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_setpassword_para),hx8369_setpassword_para},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb1), hx8369_tianma_ips_para_0xb1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb2), hx8369_tianma_ips_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb4), hx8369_tianma_ips_para_0xb4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xb6), hx8369_tianma_ips_para_0xb6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xd5), hx8369_tianma_ips_para_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xe0), hx8369_tianma_ips_para_0xe0},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0x3a), hx8369_tianma_ips_para_0x3a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8369_tianma_ips_para_0xba), hx8369_tianma_ips_para_0xba},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


/**************************************
6. nt35510 lead start 
**************************************/

static char nt35510_lead_cmd_page1_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x01};
static char nt35510_lead_cmd_page1_bc[4] = {0xbc, 0x00,0x78,0x1A};
static char nt35510_lead_cmd_page1_bd[4] = {0xbd, 0x00,0x78,0x1A};
static char nt35510_lead_cmd_page1_be[3] = {0xbe, 0x00,0x4E};
static char nt35510_lead_cmd_page1_b0[4] = {0xb0, 0x00,0x00,0x00};
static char nt35510_lead_cmd_page1_b1[4] = {0xb1, 0x00,0x00,0x00};
//static char lead_cmd_page1_b7[4] = {0xb7, 0x44,0x44,0x44};
//static char lead_cmd_page1_b3[4] = {0xb1, 0x0B,0x0B,0x0B};
static char nt35510_lead_cmd_page1_b9[4] = {0xb9, 0x34,0x34,0x34};
static char nt35510_lead_cmd_page1_ba[4] = {0xba, 0x16,0x16,0x16};

static char nt35510_lead_cmd_page1_b6[4] = {0xb6, 0x36,0x36,0x36};
static char nt35510_lead_cmd_page1_b7[4] = {0xb7, 0x26,0x26,0x26};
static char nt35510_lead_cmd_page1_b8[4] = {0xb8, 0x26,0x26,0x26};
static char nt35510_lead_cmd_page1_d1[] = {0xD1,0x00,0x00,0x00,0x2D,0x00,0x5C,
0x00,0x80,0x00,0xAB,0x00,0xE4,0x01,0x15,0x01,0x5C,0x01,0x8E,0x01,0xD3,0x02,
0x03,0x02,0x45,0x02,0x77,0x02,0x78,0x02,0xA4,0x02,0xD1,0x02,0xEA,0x03,0x09,
0x03,0x1A,0x03,0x32,0x03,0x40,0x03,0x59,0x03,0x68,0x03,0x7C,0x03,0xB2,0x03,
0xD8};
static char nt35510_lead_cmd_page1_d2[] = {0xD2,0x00,0x00,0x00,0x2D,0x00,0x5C,
0x00,0x80,0x00,0xAB,0x00,0xE4,0x01,0x15,0x01,0x5C,0x01,0x8E,0x01,0xD3,0x02,
0x03,0x02,0x45,0x02,0x77,0x02,0x78,0x02,0xA4,0x02,0xD1,0x02,0xEA,0x03,0x09,
0x03,0x1A,0x03,0x32,0x03,0x40,0x03,0x59,0x03,0x68,0x03,0x7C,0x03,0xB2,0x03,
0xD8};
static char nt35510_lead_cmd_page1_d3[] = {0xD3,0x00,0x00,0x00,0x2D,0x00,0x5C,
0x00,0x80,0x00,0xAB,0x00,0xE4,0x01,0x15,0x01,0x5C,0x01,0x8E,0x01,0xD3,0x02,
0x03,0x02,0x45,0x02,0x77,0x02,0x78,0x02,0xA4,0x02,0xD1,0x02,0xEA,0x03,0x09,
0x03,0x1A,0x03,0x32,0x03,0x40,0x03,0x59,0x03,0x68,0x03,0x7C,0x03,0xB2,0x03,
0xD8};
static char nt35510_lead_cmd_page1_d4[] = {0xD4,0x00,0x00,0x00,0x2D,0x00,0x5C,
0x00,0x80,0x00,0xAB,0x00,0xE4,0x01,0x15,0x01,0x5C,0x01,0x8E,0x01,0xD3,0x02,
0x03,0x02,0x45,0x02,0x77,0x02,0x78,0x02,0xA4,0x02,0xD1,0x02,0xEA,0x03,0x09,
0x03,0x1A,0x03,0x32,0x03,0x40,0x03,0x59,0x03,0x68,0x03,0x7C,0x03,0xB2,0x03,
0xD8};
static char nt35510_lead_cmd_page1_d5[] = {0xD5,0x00,0x00,0x00,0x2D,0x00,0x5C,
0x00,0x80,0x00,0xAB,0x00,0xE4,0x01,0x15,0x01,0x5C,0x01,0x8E,0x01,0xD3,0x02,
0x03,0x02,0x45,0x02,0x77,0x02,0x78,0x02,0xA4,0x02,0xD1,0x02,0xEA,0x03,0x09,
0x03,0x1A,0x03,0x32,0x03,0x40,0x03,0x59,0x03,0x68,0x03,0x7C,0x03,0xB2,0x03,
0xD8};
static char nt35510_lead_cmd_page1_d6[] = {0xD6,0x00,0x00,0x00,0x2D,0x00,0x5C,
0x00,0x80,0x00,0xAB,0x00,0xE4,0x01,0x15,0x01,0x5C,0x01,0x8E,0x01,0xD3,0x02,
0x03,0x02,0x45,0x02,0x77,0x02,0x78,0x02,0xA4,0x02,0xD1,0x02,0xEA,0x03,0x09,
0x03,0x1A,0x03,0x32,0x03,0x40,0x03,0x59,0x03,0x68,0x03,0x7C,0x03,0xB2,0x03,
0xD8};

static char nt35510_lead_cmd_page0_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x00};
static char nt35510_lead_cmd_page0_b1[2] = {0xb1, 0xf8};   //0xcc
static char nt35510_lead_cmd_page0_b4[2] = {0xb4, 0x10};
static char nt35510_lead_cmd_page0_b6[2] = {0xb6, 0x02};
static char nt35510_lead_cmd_page0_b7[3] = {0xb7, 0x71,0x71};
static char nt35510_lead_cmd_page0_b8[5] = {0xb8, 0x01,0x0A,0x0A,0x0A};
static char nt35510_lead_cmd_page0_bc[4] = {0xbc, 0x05,0x05,0x05};
static char nt35510_lead_cmd_page0_bd[6] = {0xbd, 0x01,0x84,0x07,0x31,0x00};
static char nt35510_lead_cmd_page0_be[6] = {0xbe, 0x01,0x84,0x07,0x31,0x00};
static char nt35510_lead_cmd_page0_bf[6] = {0xbf, 0x01,0x84,0x07,0x31,0x00};

static char nt35510_lead_cmd_page0_c7[2] = {0xc7, 0x02};
static char nt35510_lead_cmd_page0_c9[5] = {0xc9, 0x11,0x00,0x00,0x00};
static char nt35510_lead_cmd_page0_3a[2] = {0x3a, 0x77};
static char nt35510_lead_cmd_page0_35[2] = {0x35, 0x00};
static char nt35510_lead_cmd_page0_21[2] = {0x21, 0x00};

static struct dsi_cmd_desc nt35510_lead_display_on_cmds[] = {

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_f0), nt35510_lead_cmd_page1_f0},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_bc), nt35510_lead_cmd_page1_bc},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_bd), nt35510_lead_cmd_page1_bd},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_be), nt35510_lead_cmd_page1_be},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b0), nt35510_lead_cmd_page1_b0},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b1), nt35510_lead_cmd_page1_b1},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b9), nt35510_lead_cmd_page1_b9},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_ba), nt35510_lead_cmd_page1_ba},

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b6), nt35510_lead_cmd_page1_b6},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b7), nt35510_lead_cmd_page1_b7},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_b8), nt35510_lead_cmd_page1_b8},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d1), nt35510_lead_cmd_page1_d1},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d2), nt35510_lead_cmd_page1_d2},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d3), nt35510_lead_cmd_page1_d3},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d4), nt35510_lead_cmd_page1_d4},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d5), nt35510_lead_cmd_page1_d5},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page1_d6), nt35510_lead_cmd_page1_d6},

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_f0), nt35510_lead_cmd_page0_f0},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b1), nt35510_lead_cmd_page0_b1},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b4), nt35510_lead_cmd_page0_b4},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b6), nt35510_lead_cmd_page0_b6},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b7), nt35510_lead_cmd_page0_b7},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_b8), nt35510_lead_cmd_page0_b8},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_bc), nt35510_lead_cmd_page0_bc},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_bd), nt35510_lead_cmd_page0_bd},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_be), nt35510_lead_cmd_page0_be},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_bf), nt35510_lead_cmd_page0_bf},

{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_f0), nt35510_lead_cmd_page0_f0},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_c7), nt35510_lead_cmd_page0_c7},
{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_c9), nt35510_lead_cmd_page0_c9},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_3a), nt35510_lead_cmd_page0_3a},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_35), nt35510_lead_cmd_page0_35},
{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt35510_lead_cmd_page0_21), nt35510_lead_cmd_page0_21},
//{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(lead_cmd_page0_36), lead_cmd_page0_36},
{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


/**************************************
7. nt35510 yushun start 
**************************************/
//static char cmd_page_f3[9] = {0xf3, 0x00,0x32,0x00,0x38,0x31,0x08,0x11,0x00};
static char nt3511_yushun_cmd_page0_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x00};
static char nt3511_yushun_cmd_page0_b0[6] = {0xb0, 0x04,0x0a,0x0e,0x09,0x04};
static char nt3511_yushun_cmd_page0_b1[3] = {0xb1, 0x18,0x04};
static char nt3511_yushun_cmd_page0_36[2] = {0x36, 0x90};
static char nt3511_yushun_cmd_page0_b3[2] = {0xb3, 0x00};
static char nt3511_yushun_cmd_page0_b6[2] = {0xb6, 0x03};
static char nt3511_yushun_cmd_page0_b7[3] = {0xb7, 0x70,0x70};
static char nt3511_yushun_cmd_page0_b8[5] = {0xb8, 0x00,0x06,0x06,0x06};
static char nt3511_yushun_cmd_page0_bc[4] = {0xbc, 0x00,0x00,0x00};
static char nt3511_yushun_cmd_page0_bd[6] = {0xbd, 0x01,0x84,0x06,0x50,0x00};
static char nt3511_yushun_cmd_page0_cc[4] = {0xcc, 0x03,0x01,0x06};

static char nt3511_yushun_cmd_page1_f0[6] = {0xf0, 0x55,0xaa,0x52,0x08,0x01};
static char nt3511_yushun_cmd_page1_b0[4] = {0xb0, 0x05,0x05,0x05};
static char nt3511_yushun_cmd_page1_b1[4] = {0xb1, 0x05,0x05,0x05};
static char nt3511_yushun_cmd_page1_b2[4] = {0xb2, 0x03,0x03,0x03};
static char nt3511_yushun_cmd_page1_b8[4] = {0xb8, 0x25,0x25,0x25};
static char nt3511_yushun_cmd_page1_b3[4] = {0xb3, 0x0b,0x0b,0x0b};
static char nt3511_yushun_cmd_page1_b9[4] = {0xb9, 0x34,0x34,0x34};
static char nt3511_yushun_cmd_page1_bf[2] = {0xbf, 0x01};
static char nt3511_yushun_cmd_page1_b5[4] = {0xb5, 0x08,0x08,0x08};
static char nt3511_yushun_cmd_page1_ba[4] = {0xba, 0x24,0x24,0x24};
static char nt3511_yushun_cmd_page1_b4[4] = {0xb4, 0x2e,0x2e,0x2e};
static char nt3511_yushun_cmd_page1_bc[4] = {0xbc, 0x00,0x68,0x00};
static char nt3511_yushun_cmd_page1_bd[4] = {0xbd, 0x00,0x7c,0x00};
static char nt3511_yushun_cmd_page1_be[3] = {0xbe, 0x00,0x45};
static char nt3511_yushun_cmd_page1_d0[5] = {0xd0, 0x0c,0x15,0x0b,0x0e};

static char nt3511_yushun_cmd_page1_d1[53] = {0xd1, 0x00,0x37,0x00,0x61,0x00,0x92,0x00,0xB4,0x00,0xCF,0x00
,0xF6,0x01,0x2F,0x01,0x7F,0x01,0x97,0x01,0xC0,0x01,0xE5,0x02,0x25,0x02,0x5E,0x02,0x60,0x02
,0x87,0x02,0xBE,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d2[53] = {0xd2, 0x00,0x37,0x00,0x61,0x00,0x92,0x00,0xB4,0x00,0xCF,0x00
,0xF6,0x01,0x2F,0x01,0x7F,0x01,0x97,0x01,0xC0,0x01,0xE5,0x02,0x25,0x02,0x5E,0x02,0x60,0x02
,0x87,0x02,0xBE,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d3[53] = {0xd3, 0x00,0x37,0x00,0x61,0x00,0x92,0x00,0xB4,0x00,0xCF,0x00
,0xF6,0x01,0x2F,0x01,0x7F,0x01,0x97,0x01,0xC0,0x01,0xE5,0x02,0x25,0x02,0x5E,0x02,0x60,0x02
,0x87,0x02,0xBE,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d4[53] = {0xd4, 0x00,0x37,0x00,0x50,0x00,0x89,0x00,0xA9,0x00,0xC0,0x00
,0xF6,0x01,0x14,0x01,0x48,0x01,0x6B,0x01,0xA7,0x01,0xD3,0x02,0x17,0x02,0x4F,0x02,0x51,0x02
,0x86,0x02,0xBD,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d5[53] = {0xd5, 0x00,0x37,0x00,0x50,0x00,0x89,0x00,0xA9,0x00,0xC0,0x00
,0xF6,0x01,0x14,0x01,0x48,0x01,0x6B,0x01,0xA7,0x01,0xD3,0x02,0x17,0x02,0x4F,0x02,0x51,0x02
,0x86,0x02,0xBD,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};
static char nt3511_yushun_cmd_page1_d6[53] = {0xd6, 0x00,0x37,0x00,0x50,0x00,0x89,0x00,0xA9,0x00,0xC0,0x00
,0xF6,0x01,0x14,0x01,0x48,0x01,0x6B,0x01,0xA7,0x01,0xD3,0x02,0x17,0x02,0x4F,0x02,0x51,0x02
,0x86,0x02,0xBD,0x02,0xE2,0x03,0x0F,0x03,0x30,0x03,0x5C,0x03,0x77,0x03,0x94,0x03,0x9F,0x03
,0xAC,0x03,0xBA,0x03,0xF1};

static struct dsi_cmd_desc nt3511_yushun_display_on_cmds[] = {

       // yushun nt35510
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(cmd_page_ff),cmd_page_ff},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(cmd_page_f3),cmd_page_f3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_f0),nt3511_yushun_cmd_page0_f0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b0),nt3511_yushun_cmd_page0_b0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b1),nt3511_yushun_cmd_page0_b1},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_36),nt3511_yushun_cmd_page0_36},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b3),nt3511_yushun_cmd_page0_b3},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b6),nt3511_yushun_cmd_page0_b6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b7),nt3511_yushun_cmd_page0_b7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_b8),nt3511_yushun_cmd_page0_b8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_bc),nt3511_yushun_cmd_page0_bc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_bd),nt3511_yushun_cmd_page0_bd},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page0_cc),nt3511_yushun_cmd_page0_cc},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_f0),nt3511_yushun_cmd_page1_f0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b0),nt3511_yushun_cmd_page1_b0},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b1),nt3511_yushun_cmd_page1_b1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b2),nt3511_yushun_cmd_page1_b2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b8),nt3511_yushun_cmd_page1_b8},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b3),nt3511_yushun_cmd_page1_b3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b9),nt3511_yushun_cmd_page1_b9},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_bf),nt3511_yushun_cmd_page1_bf},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b5),nt3511_yushun_cmd_page1_b5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_ba),nt3511_yushun_cmd_page1_ba},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_b4),nt3511_yushun_cmd_page1_b4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_bc),nt3511_yushun_cmd_page1_bc},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_bd),nt3511_yushun_cmd_page1_bd},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_be),nt3511_yushun_cmd_page1_be},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d0),nt3511_yushun_cmd_page1_d0},

	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d1), nt3511_yushun_cmd_page1_d1},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d2), nt3511_yushun_cmd_page1_d2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d3), nt3511_yushun_cmd_page1_d3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d4), nt3511_yushun_cmd_page1_d4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d5), nt3511_yushun_cmd_page1_d5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt3511_yushun_cmd_page1_d6), nt3511_yushun_cmd_page1_d6},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


/**************************************
8. 461408 truly start 
**************************************/
static char r61408_truly_lg_para_0xb0[2]={0xB0,0x04}; 
static char r61408_truly_lg_para_0xb3[3]={0xB3,0x10,0x00}; 
static char r61408_truly_lg_para_0xbd[2]={0xbd,0x00}; 
static char r61408_truly_lg_para_0xc0[3]={0xc0,0x00,0x66};
static char r61408_truly_lg_para_0xc1[16]={0xc1,0x23,0x31,0x99,0x26,0x25,0x00,
	0x10,0x28,0x0c,0x0c,0x00,0x00,0x00,0x21,0x01};
static char r61408_truly_lg_para_0xc2[7]={0xc2,0x10,0x06,0x06,0x01,0x03,0x00};
static char r61408_truly_lg_para_0xc8[25]={0xc8,0x00,0x0e,0x17,0x20,0x2e,0x4b,
	0x3b,0x28,0x19,0x11,0x0a,0x02,0x00,0x0e,0x15,0x20,0x2e,0x47,0x3b,0x28,0x19,
	0x11,0x0a,0x02};
static char r61408_truly_lg_para_0xc9[25]={0xc9,0x00,0x0e,0x17,0x20,0x2e,0x4b,
	0x3b,0x28,0x19,0x11,0x0a,0x02,0x00,0x0e,0x15,0x20,0x2e,0x47,0x3b,0x28,0x19,
	0x11,0x0a,0x02};
static char r61408_truly_lg_para_0xca[25]={0xca,0x00,0x0e,0x17,0x20,0x2e,0x4b,
	0x3b,0x28,0x19,0x11,0x0a,0x02,0x00,0x0e,0x15,0x20,0x2e,0x47,0x3b,0x28,0x19,
	0x11,0x0a,0x02};
static char r61408_truly_lg_para_0xd0[17]={0xd0,0x29,0x03,0xce,0xa6,0x0c,0x43,
	0x20,0x10,0x01,0x00,0x01,0x01,0x00,0x03,0x01,0x00};
static char r61408_truly_lg_para_0xd1[8]={0xd1,0x18,0x0c,0x23,0x03,0x75,0x02,0x50};
static char r61408_truly_lg_para_0xd3[2]={0xd3,0x33};
static char r61408_truly_lg_para_0xd5[3]={0xd5,0x2a,0x2a};
static char r61408_truly_lg_para_0xde[3]={0xde,0x01,0x51};
static char r61408_truly_lg_para_0xe6[2]={0xe6,0x51};//vcomdc flick
static char r61408_truly_lg_para_0xfa[2]={0xfa,0x03};
static char r61408_truly_lg_para_0xd6[2]={0xd6,0x28};
static char r61408_truly_lg_para_0x2a[5]={0x2a,0x00,0x00,0x01,0xdf};
static char r61408_truly_lg_para_0x2b[5]={0x2b,0x00,0x00,0x03,0x1f};
static char r61408_truly_lg_para_0x36[2]={0x36,0x00};
static char r61408_truly_lg_para_0x3a[2]={0x3a,0x77};

/**************************************
9. nt315516_ss start 
**************************************/
//--------------------------------------------------------------//
//--------------------- SUCCESS  LCD ---------------------------//
//--------------------------------------------------------------//
/* for yushun panel */
static char nt315516_ss_0xff_yushun[6]={0xFF,0xAA,0x55,0x25,0x01,0x01};
static char nt315516_ss_0xf2_yushun[36]={0xF2,0x00,0x00,0x4A,0x0A,0xA8,0x00,0x00,0x00,
	   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x00,
	   0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x01,0x51,0x00,0x01,0x00,0x01};
static char nt315516_ss_0xf3_yushun[8] = {0xF3, 0x02,0x03,0x07,0x45,0x88,0xd1,0x0d};
static char nt315516_ss_0xf0_yushun[6]= {0xF0,0x55,0xAA,0x52,0x08,0x00};
static char nt315516_ss_0xb1_yushun[4]={0xB1,0xfc,0x00,0x00};  //0xfc stands for video,0xcc stands for cmd
static char nt315516_ss_0xb8_yushun[5]={0xB8,0x01,0x02,0x02,0x02};  
static char nt315516_ss_0xc9_yushun[7]={0xC9,0x63,0x06,0x0d,0x1a,0x17,0x00};
static char nt315516_ss_0xf0_1_yushun[6]={0xF0,0x55,0xAA,0x52,0x08,0x01};
static char nt315516_ss_0xb0_yushun[4]={0xB0,0x05,0x05,0x05}; 
static char nt315516_ss_0xb1_1_yushun[4]={0xB1,0x05,0x05,0x05};
static char nt315516_ss_0xb2_yushun[4]={0xB2,0x01,0x01,0x01};
static char nt315516_ss_0xb3_yushun[4]={0xB3,0x0e,0x0e,0x0e};
static char nt315516_ss_0xb4_yushun[4]={0xB4,0x08,0x08,0x08};

static char nt315516_ss_0xb6_yushun[4]={0xB6,0x44,0x44,0x44};
static char nt315516_ss_0xb7_yushun[4]={0xB7,0x34,0x34,0x34};
static char nt315516_ss_0xb8_1_yushun[4]={0xB8,0x10,0x10,0x10};
static char nt315516_ss_0xb9_yushun[4]={0xB9,0x26,0x26,0x26};
static char nt315516_ss_0xba_yushun[4]={0xBA,0x34,0x34,0x34};   
static char nt315516_ss_0xbc_yushun[4]={0xBC,0x00,0xc8,0x00};
static char nt315516_ss_0xbd_yushun[4]={0xBD,0x00,0xc8,0x00};
static char nt315516_ss_0xbe_yushun[2]={0xBE,0x75};    //ZTEBSP DangXiao 20120604 , for Success LCD filcker
static char nt315516_ss_0xc0_yushun[3]={0xC0,0x04,0x00};
static char nt315516_ss_0xca_yushun[2]={0xCA,0x00};
static char nt315516_ss_0xd0_yushun[5]={0xD0,0x0a,0x10,0x0d,0x0f};

// Positive Red Gamma
static char nt315516_ss_0xd1_yushun[17]={0xD1,0x00,0x70,0x00,0xCE,0x00,0xF7,0x01,0x10,0x01,0x21,0x01,0x44,0x01,0x62,0x01,0x8D};
static char nt315516_ss_0xd2_yushun[17]={0xD2,0x01,0xAF,0x01,0xE4,0x02,0x0C,0x02,0x4D,0x02,0x82,0x02,0x84,0x02,0xB8,0x02,0xF0};
static char nt315516_ss_0xd3_yushun[17]={0xD3,0x03,0x14,0x03,0x42,0x03,0x5E,0x03,0x80,0x03,0x97,0x03,0xB0,0x03,0xC0,0x03,0xDF};
static char nt315516_ss_0xd4_yushun[5]  ={0xD4,0x03,0xFD,0x03,0xFF};
// Positive Green Gamma
static char nt315516_ss_0xd5_yushun[17]={0xD5,0x00,0x70,0x00,0xCE,0x00,0xf7,0x01,0x10,0x01,0x21,0x01,0x44,0x01,0x62,0x01,0x8D};
static char nt315516_ss_0xd6_yushun[17]={0xD6,0x01,0xAF,0x01,0xE4,0x02,0x0c,0x02,0x4d,0x02,0x82,0x02,0x84,0x02,0xB8,0x02,0xF0};
static char nt315516_ss_0xd7_yushun[17]={0xD7,0x03,0x14,0x03,0x42,0x03,0x5e,0x03,0x80,0x03,0x97,0x03,0xB0,0x03,0xC0,0x03,0xDF};
static char nt315516_ss_0xd8_yushun[5]  ={0xD8,0x03,0xFD,0x03,0xFF};
                 
// Positive Blue Gamma
static char nt315516_ss_0xd9_yushun[17]={0xD9,0x00,0x70,0x00,0xCE,0x00,0xF7,0x01,0x10,0x01,0x21,0x01,0x44,0x01,0x62,0x01,0x8D};
static char nt315516_ss_0xdd_yushun[17]={0xDD,0x01,0xAF,0x01,0xE4,0x02,0x0C,0x02,0x4D,0x02,0x82,0x02,0x84,0x02,0xB8,0x02,0xF0};
static char nt315516_ss_0xde_yushun[17]={0xDE,0x03,0x14,0x03,0x42,0x03,0x5E,0x03,0x80,0x03,0x97,0x03,0xB0,0x03,0xC0,0x03,0xDF};
static char nt315516_ss_0xdf_yushun[5]  ={0xDF,0x03,0xFD,0x03,0xFF};      
                 
// Negative Red Gamma
static char nt315516_ss_0xe0_yushun[17]={0xE0,0x00,0x70,0x00,0xCE,0x00,0xF7,0x01,0x10,0x01,0x21,0x01,0x44,0x01,0x62,0x01,0x8D};
static char nt315516_ss_0xe1_yushun[17]={0xE1,0x01,0xAF,0x01,0xE4,0x02,0x0C,0x02,0x4D,0x02,0x82,0x02,0x84,0x02,0xB8,0x02,0xF0};
static char nt315516_ss_0xe2_yushun[17]={0xE2,0x03,0x14,0x03,0x42,0x03,0x5E,0x03,0x80,0x03,0x97,0x03,0xB0,0x03,0xC0,0x03,0xDF};
static char nt315516_ss_0xe3_yushun[5]  ={0xE3,0x03,0xFD,0x03,0xFF};           
                 
// Negative Green Gamma
static char nt315516_ss_0xe4_yushun[17]={0xE4,0x00,0x70,0x00,0xCE,0x00,0xF7,0x01,0x10,0x01,0x21,0x01,0x44,0x01,0x62,0x01,0x8D};
static char nt315516_ss_0xe5_yushun[17]={0xE5,0x01,0xAF,0x01,0xE4,0x02,0x0C,0x02,0x4D,0x02,0x82,0x02,0x84,0x02,0xB8,0x02,0xF0};
static char nt315516_ss_0xe6_yushun[17]={0xE6,0x03,0x14,0x03,0x42,0x03,0x5E,0x03,0x80,0x03,0x97,0x03,0xB0,0x03,0xC0,0x03,0xDF};
static char nt315516_ss_0xe7_yushun[5]  ={0xE7,0x03,0xFD,0x03,0xFF};
                 
// Negative Blue Gamma
static char nt315516_ss_0xe8_yushun[17]={0xE8,0x00,0x70,0x00,0xCE,0x00,0xF7,0x01,0x10,0x01,0x21,0x01,0x44,0x01,0x62,0x01,0x8D};
static char nt315516_ss_0xe9_yushun[17]={0xE9,0x01,0xAF,0x01,0xE4,0x02,0x0C,0x02,0x4D,0x02,0x82,0x02,0x84,0x02,0xB8,0x02,0xF0};
static char nt315516_ss_0xea_yushun[17]={0xEA,0x03,0x14,0x03,0x42,0x03,0x5E,0x03,0x80,0x03,0x97,0x03,0xB0,0x03,0xC0,0x03,0xDF};
static char nt315516_ss_0xeb_yushun[5]  ={0xEB,0x03,0xFD,0x03,0xFF};  

static char nt315516_ss_0x2c_yushun[2] = {0x2c, 0x00};
static char nt315516_ss_0x13_yushun[2] = {0x13, 0x00};

/*  for yushun panel */
static struct dsi_cmd_desc nt315516_ss_display_on_cmds_yushun[] = 
{
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xff_yushun), nt315516_ss_0xff_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xf2_yushun), nt315516_ss_0xf2_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xf3_yushun), nt315516_ss_0xf3_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xf0_yushun), nt315516_ss_0xf0_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb1_yushun), nt315516_ss_0xb1_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb8_yushun), nt315516_ss_0xb8_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xc9_yushun), nt315516_ss_0xc9_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xf0_1_yushun), nt315516_ss_0xf0_1_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb0_yushun), nt315516_ss_0xb0_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb1_1_yushun), nt315516_ss_0xb1_1_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb2_yushun), nt315516_ss_0xb2_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb3_yushun), nt315516_ss_0xb3_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb4_yushun), nt315516_ss_0xb4_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb6_yushun), nt315516_ss_0xb6_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb7_yushun), nt315516_ss_0xb7_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb8_1_yushun), nt315516_ss_0xb8_1_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb9_yushun), nt315516_ss_0xb9_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xba_yushun), nt315516_ss_0xba_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xbc_yushun), nt315516_ss_0xbc_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xbd_yushun), nt315516_ss_0xbd_yushun},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt315516_ss_0xbe_yushun), nt315516_ss_0xbe_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xc0_yushun), nt315516_ss_0xc0_yushun},	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt315516_ss_0xca_yushun), nt315516_ss_0xca_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd0_yushun), nt315516_ss_0xd0_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd1_yushun), nt315516_ss_0xd1_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd2_yushun), nt315516_ss_0xd2_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd3_yushun), nt315516_ss_0xd3_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd4_yushun), nt315516_ss_0xd4_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd5_yushun), nt315516_ss_0xd5_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd6_yushun), nt315516_ss_0xd6_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd7_yushun), nt315516_ss_0xd7_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd8_yushun), nt315516_ss_0xd8_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd9_yushun), nt315516_ss_0xd9_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xdd_yushun), nt315516_ss_0xdd_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xde_yushun), nt315516_ss_0xde_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xdf_yushun), nt315516_ss_0xdf_yushun},		
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe0_yushun), nt315516_ss_0xe0_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe1_yushun), nt315516_ss_0xe1_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe2_yushun), nt315516_ss_0xe2_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe3_yushun), nt315516_ss_0xe3_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe4_yushun), nt315516_ss_0xe4_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe5_yushun), nt315516_ss_0xe5_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe6_yushun), nt315516_ss_0xe6_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe7_yushun), nt315516_ss_0xe7_yushun},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe8_yushun), nt315516_ss_0xe8_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe9_yushun), nt315516_ss_0xe9_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xea_yushun), nt315516_ss_0xea_yushun},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xeb_yushun), nt315516_ss_0xeb_yushun},	

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0x2c_yushun), nt315516_ss_0x2c_yushun},
	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0x13_yushun), nt315516_ss_0x13_yushun},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20, sizeof(display_on), display_on}

};
/* !!! for lead panel */
static char nt315516_ss_0xf0[6]= {0xF0,0x55,0xAA,0x52,0x08,0x00};
static char nt315516_ss_0xb1[2]={0xB1,0xfc};  //0xfc stands for video,0xcc stands for cmd
static char nt315516_ss_0xb8[5]={0xB8,0x01,0x02,0x02,0x02};  
static char nt315516_ss_0xbc[4]={0xBC,0x05,0x05,0x05};
static char nt315516_ss_0xf0_1[6]={0xF0,0x55,0xAA,0x52,0x08,0x01};
static char nt315516_ss_0xb0[4]={0xB0,0x05,0x05,0x05}; 
static char nt315516_ss_0xb1_1[4]={0xB1,0x05,0x05,0x05};

static char nt315516_ss_0xb6[4]={0xB6,0x44,0x44,0x44};
static char nt315516_ss_0xb7[4]={0xB7,0x34,0x34,0x34};

static char nt315516_ss_0xba[4]={0xBA,0x24,0x24,0x24};   
static char nt315516_ss_0xbc_1[4]={0xBC,0x00,0x88,0x00};
static char nt315516_ss_0xbd[4]={0xBD,0x00,0x88,0x00};
static char nt315516_ss_0xbe[2]={0xBE,0x4f};    //ZTEBSP DangXiao 20120604 , for Success LCD  filcker 0x75


// Positive Red Gamma
static char nt315516_ss_0xd1[17]={0xD1,0x00,0x29,0x00,0x33,0x00,0x36,0x00,0x37,0x00,0x40,0x00,0x6d,0x00,0x88,0x00,0xB0};
static char nt315516_ss_0xd2[17]={0xD2,0x00,0xE0,0x01,0x13,0x01,0x3B,0x01,0x78,0x01,0xaa,0x01,0xAB,0x01,0xD7,0x02,0x02};
static char nt315516_ss_0xd3[17]={0xD3,0x02,0x19,0x02,0x38,0x02,0x4f,0x02,0x79,0x02,0xa0,0x02,0xea,0x03,0x21,0x03,0xD0};
static char nt315516_ss_0xd4[5]  ={0xD4,0x03,0xF4,0x03,0xF4};
// Positive Green Gamma
static char nt315516_ss_0xd5[17]={0xD5,0x00,0x29,0x00,0x33,0x00,0x36,0x00,0x37,0x00,0x40,0x00,0x6d,0x00,0x88,0x00,0xb0};
static char nt315516_ss_0xd6[17]={0xD6,0x00,0xE0,0x01,0x13,0x01,0x3B,0x01,0x78,0x01,0xaa,0x01,0xAB,0x01,0xD7,0x02,0x02};
static char nt315516_ss_0xd7[17]={0xD7,0x02,0x19,0x02,0x38,0x02,0x4f,0x02,0x79,0x02,0xa0,0x02,0xea,0x03,0x21,0x03,0xD0};
static char nt315516_ss_0xd8[5]  ={0xD8,0x03,0xF4,0x03,0xF4};
                 
// Positive Blue Gamma
static char nt315516_ss_0xd9[17]={0xD9,0x00,0x29,0x00,0x33,0x00,0x36,0x00,0x37,0x00,0x40,0x00,0x6d,0x00,0x88,0x00,0xb0};
static char nt315516_ss_0xdd[17]={0xDD,0x00,0xE0,0x01,0x13,0x01,0x3B,0x01,0x78,0x01,0xaa,0x01,0xAB,0x01,0xD7,0x02,0x02};
static char nt315516_ss_0xde[17]={0xDE,0x02,0x19,0x02,0x38,0x02,0x4f,0x02,0x79,0x02,0xa0,0x02,0xea,0x03,0x21,0x03,0xD0};
static char nt315516_ss_0xdf[5]  ={0xDF,0x03,0xF4,0x03,0xF4};      
                 
// Negative Red Gamma
static char nt315516_ss_0xe0[17]={0xE0,0x00,0x29,0x00,0x33,0x00,0x36,0x00,0x37,0x00,0x40,0x00,0x6D,0x00,0x88,0x00,0xB0};
static char nt315516_ss_0xe1[17]={0xE1,0x00,0xE0,0x01,0x13,0x01,0x3B,0x01,0x78,0x01,0xAA,0x01,0xAB,0x01,0xD7,0x02,0x02};
static char nt315516_ss_0xe2[17]={0xE2,0x02,0x19,0x02,0x38,0x02,0x4F,0x02,0x79,0x02,0xA0,0x02,0xEA,0x03,0x21,0x03,0xD0};
static char nt315516_ss_0xe3[5]  ={0xE3,0x03,0xF4,0x03,0xF4};           
                 
// Negative Green Gamma
static char nt315516_ss_0xe4[17]={0xE4,0x00,0x29,0x00,0x33,0x00,0x36,0x00,0x37,0x00,0x40,0x00,0x6D,0x00,0x88,0x00,0xB0};
static char nt315516_ss_0xe5[17]={0xE5,0x00,0xE0,0x01,0x13,0x01,0x3B,0x01,0x78,0x01,0xAA,0x01,0xAB,0x01,0xD7,0x02,0x02};
static char nt315516_ss_0xe6[17]={0xE6,0x02,0x19,0x02,0x38,0x02,0x4F,0x02,0x79,0x02,0xA0,0x02,0xEA,0x03,0x21,0x03,0xD0};
static char nt315516_ss_0xe7[5]  ={0xE7,0x03,0xF4,0x03,0xF4};
                 
// Negative Blue Gamma
static char nt315516_ss_0xe8[17]={0xE8,0x00,0x29,0x00,0x33,0x00,0x36,0x00,0x37,0x00,0x40,0x00,0x6D,0x00,0x88,0x00,0xB0};
static char nt315516_ss_0xe9[17]={0xE9,0x00,0xE0,0x01,0x13,0x01,0x3B,0x01,0x78,0x01,0xAA,0x01,0xAB,0x01,0xD7,0x02,0x02};
static char nt315516_ss_0xea[17]={0xEA,0x02,0x19,0x02,0x38,0x02,0x4F,0x02,0x79,0x02,0xA0,0x02,0xEA,0x03,0x21,0x03,0xD0};
static char nt315516_ss_0xeb[5]  ={0xEB,0x03,0xF4,0x03,0xF4};  

static char nt315516_ss_0x35[2] = {0x35, 0x00};

/*  for lead panel */
static struct dsi_cmd_desc nt315516_ss_display_on_cmds[] = 
{	
    /* Select CMD2, page 0 */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xf0), nt315516_ss_0xf0},
	/* Display control */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb1), nt315516_ss_0xb1},
	/* Source EQ */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb8), nt315516_ss_0xb8},
	/* Z Inversion */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xbc), nt315516_ss_0xbc},
	/* Select CMD2, page 1 */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xf0_1), nt315516_ss_0xf0_1},
	/* AVDD 6.0v */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb0), nt315516_ss_0xb0},
	/* AVEE -6.0v */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb1_1), nt315516_ss_0xb1_1},
	/* AVDD 2.5x VPNL */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb6), nt315516_ss_0xb6},
	/* AVEE -2.5x VPNL */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xb7), nt315516_ss_0xb7},
	/* VGLX: AVEE - AVDD + VCL */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xba), nt315516_ss_0xba},
	/* VGMP: 4.7V,VGSP=0V */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xbc_1), nt315516_ss_0xbc_1},
	/* VGMN: -4.7V,VGSN=-0V */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xbd), nt315516_ss_0xbd},
	/* VCOM */
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(nt315516_ss_0xbe), nt315516_ss_0xbe},
	/* Gamma Code - Positive Red Gamma */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd1), nt315516_ss_0xd1},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd2), nt315516_ss_0xd2},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd3), nt315516_ss_0xd3},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd4), nt315516_ss_0xd4},
	/* Positive Green Gamma */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd5), nt315516_ss_0xd5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd6), nt315516_ss_0xd6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd7), nt315516_ss_0xd7},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd8), nt315516_ss_0xd8},
	/* Positive Blue Gamma */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xd9), nt315516_ss_0xd9},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xdd), nt315516_ss_0xdd},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xde), nt315516_ss_0xde},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xdf), nt315516_ss_0xdf},
	/* Negative Red Gamma */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe0), nt315516_ss_0xe0},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe1), nt315516_ss_0xe1},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe2), nt315516_ss_0xe2},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe3), nt315516_ss_0xe3},
	/* Negative Green Gamma */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe4), nt315516_ss_0xe4},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe5), nt315516_ss_0xe5},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe6), nt315516_ss_0xe6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe7), nt315516_ss_0xe7},
	/* Negative Blue Gamma */
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe8), nt315516_ss_0xe8},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xe9), nt315516_ss_0xe9},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xea), nt315516_ss_0xea},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0xeb), nt315516_ss_0xeb},	

	{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0x35), nt315516_ss_0x35},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	//{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0x2c), nt315516_ss_0x2c},
	//{DTYPE_DCS_WRITE, 1, 0, 0, 0, sizeof(nt315516_ss_0x13), nt315516_ss_0x13},
	{DTYPE_DCS_WRITE, 1, 0, 0, 20, sizeof(display_on), display_on}

}; 



static struct dsi_cmd_desc r61408_truly_lg_display_on_cmds[] = 
{
	
	
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xb0), r61408_truly_lg_para_0xb0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xb3), r61408_truly_lg_para_0xb3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xbd), r61408_truly_lg_para_0xbd},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc0), r61408_truly_lg_para_0xc0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc1), r61408_truly_lg_para_0xc1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc2), r61408_truly_lg_para_0xc2},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc8), r61408_truly_lg_para_0xc8},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xc9), r61408_truly_lg_para_0xc9},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xca), r61408_truly_lg_para_0xca},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd0), r61408_truly_lg_para_0xd0},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd1), r61408_truly_lg_para_0xd1},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd3), r61408_truly_lg_para_0xd3},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd5), r61408_truly_lg_para_0xd5},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xde), r61408_truly_lg_para_0xde},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xe6), r61408_truly_lg_para_0xe6},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xfa), r61408_truly_lg_para_0xfa},
	{DTYPE_GEN_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0xd6), r61408_truly_lg_para_0xd6},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x2a), r61408_truly_lg_para_0x2a},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x2b), r61408_truly_lg_para_0x2b},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x36), r61408_truly_lg_para_0x36},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(r61408_truly_lg_para_0x3a), r61408_truly_lg_para_0x3a},

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};
/**************************************
9. hx8363 YUSHUN IVO start 
**************************************/
#if 0
static char hx8363_yushun_para_0xb1[13]={0xB1,0x78,0x34,0x08,0x34,0x02,0x13,0x11,0x11,0x1C,0x24,0x3F,0x3F};  
static char hx8363_yushun_para_0xba[14]={0xBA,0x80,0x00,0x10,0x08,0x08,0x10,0x7c,0x6e,
								0x6d,0x0a,0x01,0x84,0x43};   //TWO LANE
static char hx8363_yushun_para_0x3a[2]={0x3a,0x77};
//static char hx8363_para_0x36[2]={0x36,0x0a};
static char hx8363_yushun_para_0xb2[4]={0xb2,0x33,0x33,0x22};
static char hx8363_yushun_para_0xb3[2]={0xb3,0x00};
static char hx8363_yushun_para_0xb4[10]={0xb4,0x08,0x12,0x72,0x12,0x06,0x03,0x54,0x03,0x4e};
static char hx8363_yushun_para_0xb6[2]={0xb6,0x2c};
static char hx8363_yushun_para_0xcc[2]={0xcc,0x09};
static char hx8363_yushun_para_0xe0[31]={0x00,0x1E,0x63,0x15,0x13,0x30,0x0C,0xCF,0x0F,0xD5,
	0x17,0xD5,0x96,0xD1,0x17,0x00,0x1E,0x63,0x15,0x13,0x30,0x0C,0xCF,0x0F,0xD5,0x17,0xD5,0x96,0xD1,0x17};	
#endif
static char hx8363_yushun_para_0xb1[13]={0xB1,0x78,0x34,0x07,0x33,0x02,0x13,0x0F,0x00,0x1C,0x24,0x3F,0x3F};  
static char hx8363_yushun_para_0xba[14]={0xBA,0x80,0x00,0x10,0x08,0x08,0x10,0x7C,0x6E,0x6D,0x0A,0x01,0x84,0x43};   //TWO LANE
static char hx8363_yushun_para_0x3a[2]={0x3A,0x70};
//static char hx8363_para_0x36[2]={0x36,0x0a};
static char hx8363_yushun_para_0xb2[4]={0xB2,0x33,0x33,0x22};
static char hx8363_yushun_para_0xb4[10]={0xB4,0x04,0x12,0x72,0x12,0x06,0x03,0x54,0x03,0x4E};
static char hx8363_yushun_para_0xb6[2]={0xB6,0x1C};
static char hx8363_yushun_para_0xcc[2]={0xCC,0x09};
static char hx8363_yushun_para_0xe0[31]={0xE0,0x00,0x1f,0x61,0x0A,0x0A,0x3C,0x0d,0x91,0x50,0x15,0x17,0xD5,0x16,0x13,0x1A,0x00,0x1f,0x61,0x0A,0x0A,0x3C,0x0d,0x91,0x50,0x15,0x17,0xD5,0x16,0x13,0x1A};	
static struct dsi_cmd_desc hx8363_yushun_display_on_cmds[] = 
{

	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_setpassword_para),hx8363_setpassword_para},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb1), hx8363_yushun_para_0xb1},	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb2), hx8363_yushun_para_0xb2},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xba), hx8363_yushun_para_0xba},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0x3a), hx8363_yushun_para_0x3a},
//	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb3), hx8363_yushun_para_0xb3},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb4), hx8363_yushun_para_0xb4},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xb6), hx8363_yushun_para_0xb6},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xcc), hx8363_yushun_para_0xcc},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 10, sizeof(hx8363_para_0x36), hx8363_para_0x36},		
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(hx8363_yushun_para_0xe0), hx8363_yushun_para_0xe0},
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8363_para_0xc1), hx8363_para_0xc1},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(hx8363_para_0xc2), hx8363_para_0xc2},	

	{DTYPE_DCS_WRITE, 1, 0, 0, 10, sizeof(display_on), display_on}

};


static void lcd_panle_reset(void)
{
	gpio_direction_output(GPIO_LCD_RESET,1);
	//msleep(10);
	usleep(10000);
	gpio_direction_output(GPIO_LCD_RESET,0);
	//msleep(10);
	usleep(10000);
	gpio_direction_output(GPIO_LCD_RESET,1);
	//msleep(10);
	usleep(120000);
}

struct mipi_manufacture_ic {
	struct dsi_cmd_desc *readid_tx;
	int readid_len_tx;
	struct dsi_cmd_desc *readid_rx;
	int readid_len_rx;
	int mode;
};
/*
#ifdef CONFIG_LCD_READ_ID
static int mipi_get_manufacture_icid(struct msm_fb_data_type *mfd)
{
	uint32 icid = 0;
	int i ;
	

	 struct mipi_manufacture_ic mipi_manufacture_icid[4] = {
		{hx8363_setpassword_cmd,ARRAY_SIZE(hx8363_setpassword_cmd),&hx8363_icid_rd_cmd,3,1},
		{nt3511_setpassword_cmd,ARRAY_SIZE(nt3511_setpassword_cmd),&nt3511_icid_rd_cmd,3,0},
		{hx8369_setpassword_cmd,ARRAY_SIZE(hx8369_setpassword_cmd),&hx8369_icid_rd_cmd,3,1},
		{r61408_setpassword_cmd,ARRAY_SIZE(r61408_setpassword_cmd),&r61408_icid_rd_cmd,4,1},

	 };

	for(i = 0; i < ARRAY_SIZE(mipi_manufacture_icid) ; i++)
	{	lcd_panle_reset();	
		mipi_dsi_buf_init(&lead_tx_buf);
		mipi_dsi_buf_init(&lead_rx_buf);
		mipi_set_tx_power_mode(1);	
		
		mipi_dsi_cmds_tx(&lead_tx_buf, mipi_manufacture_icid[i].readid_tx,mipi_manufacture_icid[i].readid_len_tx);
		mipi_dsi_cmd_bta_sw_trigger(); 
		
		if(!mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);	
		
		mipi_dsi_cmds_rx(mfd,&lead_tx_buf, &lead_rx_buf, mipi_manufacture_icid[i].readid_rx,mipi_manufacture_icid[i].readid_len_rx);

		if(mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);
		
		icid = *(uint32 *)(lead_rx_buf.data);
		
		printk("debug read icid is %x\n",icid & 0xffffff);

		switch(icid & 0xffffff){
			case 0x1055:
						return NOVATEK_35510;
			case 0x6383ff:
						return HIMAX_8363;
						
			case 0x6983ff:
						return HIMAX_8369;
			case 0x142201:
						return RENESAS_R61408;
			default:
						break;			
		}

	}
	return 0;
}

static uint32 mipi_get_commic_panleid(struct msm_fb_data_type *mfd,struct dsi_cmd_desc *para,uint32 len,int mode)
{
	uint32 panelid = 0;
	mipi_dsi_buf_init(&lead_tx_buf);
	mipi_dsi_buf_init(&lead_rx_buf);
	mipi_dsi_cmd_bta_sw_trigger(); 
	if(mode)
		mipi_set_tx_power_mode(1);
	else 
		mipi_set_tx_power_mode(0);
	mipi_dsi_cmds_rx(mfd,&lead_tx_buf, &lead_rx_buf, para,len);
	if(mode)
		mipi_set_tx_power_mode(0);
	panelid = *(uint32 *)(lead_rx_buf.data);
	printk("debug read panelid is %x\n",panelid & 0xffffffff);
	return panelid;
}
static uint32 mipi_get_himax8363_panleid(struct msm_fb_data_type *mfd)
{
	uint32 panleid;
	
	panleid =  mipi_get_commic_panleid(mfd,&hx8363_panleid_rd_cmd,1,1);
	switch((panleid >> 8) & 0xff){

		case HIMAX8369_YUSHUN_IVO_ID:
				return (u32)LCD_PANEL_4P0_HX8363_IVO_YUSHUN;
		default:
				return (u32)LCD_PANEL_4P0_HX8363_CMI_YASSY;
	}
}
static uint32 mipi_get_himax8369_panleid(struct msm_fb_data_type *mfd)
{
	uint32 panleid;
	
	panleid =  mipi_get_commic_panleid(mfd,&hx8369_panleid_rd_cmd,1,1);
	switch((panleid>>8) & 0xff){
		case HIMAX8369_TIANMA_TN_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_TN;
		case HIMAX8369_TIANMA_IPS_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_IPS;
		case HIMAX8369_LEAD_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_LEAD;
		case HIMAX8369_LEAD_HANNSTAR_ID:
				return (u32)LCD_PANEL_4P0_HIMAX8369_LEAD_HANNSTAR;
		default:
				return (u32)LCD_PANEL_NOPANEL;
	}
}




static uint32 mipi_get_nt35510_panleid(struct msm_fb_data_type *mfd)
{
	uint32 panleid =  mipi_get_commic_panleid(mfd,&nt3511_panleid_rd_cmd,1,0);
	switch(panleid & 0xff){
		case NT35510_YUSHUN_ID:
				return  (u32)LCD_PANEL_4P0_NT35510_HYDIS_YUSHUN	;
		case NT35510_LEAD_ID:
				return (u32)LCD_PANEL_4P0_NT35510_LEAD;
		default:
				return (u32)LCD_PANEL_NOPANEL;
	}
}

static uint32 mipi_get_icpanleid(struct msm_fb_data_type *mfd )
{
	int icid = 0;

	lcd_panle_reset();
	icid = mipi_get_manufacture_icid(mfd);
	printk("wangmirnong icid is 0x%x-------\r\n",icid);

	switch(icid){
		case HIMAX_8363:		
				LcdPanleID = mipi_get_himax8363_panleid(mfd);
					break;
		case HIMAX_8369:
					LcdPanleID = mipi_get_himax8369_panleid(mfd);
					break;
		case NOVATEK_35510:
					LcdPanleID = mipi_get_nt35510_panleid(mfd);
					break;
		case RENESAS_R61408:
					LcdPanleID = LCD_PANEL_4P0_R61408_TRULY_LG;
			break;
		default:
					LcdPanleID = (u32)LCD_PANEL_NOPANEL;
					printk("warnning cann't indentify the chip\n");
					break;
	}
		
	return LcdPanleID;
}
#endif
*/
//[ECID:0000] ZTEBSP wangminrong start 20120730 read lcd id from bootloader
static u32 __init get_lcdpanleid_from_bootloader(void)
{
	    switch(lcd_id_type)
		{	
			case 10:
				return (u32)LCD_PANEL_4P3_NT35516_HYDIS_YUSHUN;
			case 11:
				return (u32)LCD_PANEL_4P3_NT35516_HYDIS_LEAD;				
			default:
				break;
		}
		
		return (u32)LCD_PANEL_NOPANEL;
}

//[ECID:0000] ZTEBSP wangminrong end 20120730 read lcd id from bootloader

static int mipi_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;
	mipi_set_tx_power_mode(1);

	mipi_dsi_cmds_tx(&lead_tx_buf, display_off_cmds,
			ARRAY_SIZE(display_off_cmds));
	gpio_direction_output(GPIO_LCD_RESET,0);
	//[ECID:0000] ZTEBSP wangminrong end 20120525 for lcd off reset start 
	msleep(5);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
//	gpio_direction_output(121,0);
	
	//[ECID:0000] ZTEBSP wangminrong end 20120525 for lcd off reset end
	return 0;
}

/*
static char nt35516_page_f0[6] = {0xF0,0x55,0xAA,0x52,0x08,0x01};
static char nt35516_icid_rd_para[2] = {0xc5, 0x00}; 
static char nt35516_panleid_rd_para[2] = {0xDA, 0x00};    //added by zte_gequn091966 for lead_nt35510,20111226

static struct dsi_cmd_desc nt35516_setpassword_cmd[] = {	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(nt35516_page_f0),nt35516_page_f0},
};

static struct dsi_cmd_desc nt35516_icid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt35516_icid_rd_para), nt35516_icid_rd_para};


static struct dsi_cmd_desc nt35516_panleid_rd_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 0, sizeof(nt35516_panleid_rd_para), nt35516_panleid_rd_para
};


static uint32 mipi_get_commic_panleid(struct msm_fb_data_type *mfd,struct dsi_cmd_desc *para,uint32 len,int mode)
{
	uint32 panelid = 0;
	mipi_dsi_buf_init(&lead_tx_buf);
	mipi_dsi_buf_init(&lead_rx_buf);
	mipi_dsi_cmd_bta_sw_trigger(); 
	if(mode)
		mipi_set_tx_power_mode(1);
	else 
		mipi_set_tx_power_mode(0);
	mipi_dsi_cmds_rx(mfd,&lead_tx_buf, &lead_rx_buf, para,len);
	if(mode)
		mipi_set_tx_power_mode(0);
	panelid = *(uint32 *)(lead_rx_buf.data);
	printk("debug read panelid is %x\n",panelid & 0xffffffff);
	return panelid;
}

static uint32 mipi_get_nt35516_panleid(struct msm_fb_data_type *mfd)
{
	uint32 panleid =  mipi_get_commic_panleid(mfd,&nt35516_panleid_rd_cmd,1,0);

    printk("wangjianping mipi_get_nt35516_panleid is 0x%x-------\r\n",panleid);
	return (u32)LCD_PANEL_NOPANEL;

}

static int mipi_get_manufacture_icid(struct msm_fb_data_type *mfd)
{
	uint32 icid = 0;
	int i ;
	

	 struct mipi_manufacture_ic mipi_manufacture_icid[] = {
		{nt35516_setpassword_cmd,ARRAY_SIZE(nt35516_setpassword_cmd),&nt35516_icid_rd_cmd,4,0},
	 };

	for(i = 0; i < ARRAY_SIZE(mipi_manufacture_icid) ; i++)
	{	lcd_panle_reset();	
		mipi_dsi_buf_init(&lead_tx_buf);
		mipi_dsi_buf_init(&lead_rx_buf);
		mipi_set_tx_power_mode(1);	
		
		mipi_dsi_cmds_tx(&lead_tx_buf, mipi_manufacture_icid[i].readid_tx,mipi_manufacture_icid[i].readid_len_tx);
		mipi_dsi_cmd_bta_sw_trigger(); 
		
		if(!mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);	
		
		mipi_dsi_cmds_rx(mfd,&lead_tx_buf, &lead_rx_buf, mipi_manufacture_icid[i].readid_rx,mipi_manufacture_icid[i].readid_len_rx);

		if(mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);
		
		icid = *(uint32 *)(lead_rx_buf.data);
		
		printk("debug read icid is %x\n",icid & 0xffffff);

		switch(icid & 0xffffff){
			case 0x1055:
						return NOVATEK_35510;
			case 0x6383ff:
						return HIMAX_8363;
						
			case 0x6983ff:
						return HIMAX_8369;
			case 0x142201:
						return RENESAS_R61408;
			default:
						break;			
		}

	}
	return 0;
}

static uint32 mipi_get_icpanleid(struct msm_fb_data_type *mfd )
{
	int icid = 0;

    printk("wangjianping start mipi_get_icpanleid\r\n");

	lcd_panle_reset();
	icid = mipi_get_manufacture_icid(mfd);
	printk("wangjianping icid is 0x%x-------\r\n",icid);


    LcdPanleID = mipi_get_nt35516_panleid(mfd);

    printk("wangjianping LcdPanleID is 0x%x-------\r\n",icid);
		
	return LcdPanleID;
}
*/

static int first_time_panel_on = 1;
//[ECID:0000] ZTEBSP wangminrong start  20120618 for logo first is return 
static int first_init = 1;
static int mipi_lcd_on(struct platform_device *pdev)
{
	
	struct msm_fb_data_type *mfd = platform_get_drvdata(pdev);
		//[ECID:000000] ZTEBSP wangminrong  for qualcomm continus patch 20120822 start
	 #ifdef CONFIG_CONTINOUS_LOGO
	 if(!mfd->cont_splash_done)
	 {
	 
	 	mfd->cont_splash_done = 1;
		first_init = 0;
		first_time_panel_on = 0;
		return 0;
	 }
	 #endif
	 
	//[ECID:000000] ZTEBSP wangminrong  for qualcomm continus patch 20120822 end
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;
	//printk("zhangqi add %s  first_time_panel_on=%d/n",__func__,first_time_panel_on);
	if(first_time_panel_on){
		first_time_panel_on = 0;
		if(first_init == 1)
		{
			first_init = 0;
			return 0;

		}
		
	//	if(LcdPanleID != (u32)LCD_PANEL_NOPANEL)
		//	return 0;
		
	}
    
//	else
		//[ECID:0000] ZTEBSP wangminrong start 20120730 read lcd id from bootloader
//		{	
		//	LcdPanleID = mipi_get_icpanleid(mfd);
	//	}
		
//[ECID:0000] ZTEBSP wangminrong start  20120618 for logo first is return
	//printk(" wangjianping mipi start read id\n");
    //LcdPanleID = mipi_get_icpanleid(mfd);

	
	lcd_panle_reset();
	printk("mipi init start\n");
	mipi_set_tx_power_mode(1);
 
	switch(LcdPanleID){
		case (u32)LCD_PANEL_4P0_HX8363_CMI_YASSY:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8363_yassy_display_on_cmds,ARRAY_SIZE(hx8363_yassy_display_on_cmds));
				printk("HIMAX8363_YASS init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_LEAD:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_lead_display_on_cmds,ARRAY_SIZE(hx8369_lead_display_on_cmds));
				printk("HIMAX8369_LEAD init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_LEAD_HANNSTAR:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_lead_hannstar_display_on_cmds,ARRAY_SIZE(hx8369_lead_hannstar_display_on_cmds));
				printk("HIMAX8369_LEAD_HANNSTAR init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_TN:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_tianma_tn_display_on_cmds,ARRAY_SIZE(hx8369_tianma_tn_display_on_cmds));
				printk("HIMAX8369_TIANMA_TN init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_HIMAX8369_TIANMA_IPS:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_tianma_ips_display_on_cmds,ARRAY_SIZE(hx8369_tianma_ips_display_on_cmds));
				printk("HIMAX8369_TIANMA_IPS init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_NT35510_LEAD:
				mipi_dsi_cmds_tx(&lead_tx_buf, nt35510_lead_display_on_cmds,ARRAY_SIZE(nt35510_lead_display_on_cmds));
				printk("NT35510_LEAD init ok !!\n");
				break;
				
		case (u32)LCD_PANEL_4P0_NT35510_HYDIS_YUSHUN:
				mipi_dsi_cmds_tx(&lead_tx_buf, nt3511_yushun_display_on_cmds,ARRAY_SIZE(nt3511_yushun_display_on_cmds));
				printk("NT35510_HYDIS_YUSHUN init ok !!\n");
				break;
		case (u32)LCD_PANEL_4P0_R61408_TRULY_LG:
				mipi_dsi_cmds_tx(&lead_tx_buf, r61408_truly_lg_display_on_cmds,ARRAY_SIZE(r61408_truly_lg_display_on_cmds));
				printk("R61408 TRULY LG  init ok !!\n");
			break;
		case (u32)LCD_PANEL_4P0_HX8363_IVO_YUSHUN:
				mipi_dsi_cmds_tx(&lead_tx_buf, hx8363_yushun_display_on_cmds,ARRAY_SIZE(hx8363_yushun_display_on_cmds));
				printk("HIMAX8363_YUSHUN init ok !!\n");
				break;
		case (u32)LCD_PANEL_4P3_NT35516_HYDIS_YUSHUN:
				mipi_dsi_cmds_tx(&lead_tx_buf, nt315516_ss_display_on_cmds_yushun,ARRAY_SIZE(nt315516_ss_display_on_cmds_yushun));
				printk("NT35516_HYDIS_YUSHUN  init ok !!\n");
			break;					
		case (u32)LCD_PANEL_4P3_NT35516_HYDIS_LEAD:
				mipi_dsi_cmds_tx(&lead_tx_buf, nt315516_ss_display_on_cmds,ARRAY_SIZE(nt315516_ss_display_on_cmds));
				printk("NT35516_HYDIS_LEAD  init ok !!\n");
			break;					
		default:				
				printk("can't get icpanelid value\n");				
				break;
				
	}	
	mipi_set_tx_power_mode(0);
	return 0;
}



static struct msm_fb_panel_data lead_panel_data = {
	.on		= mipi_lcd_on,
	.off		= mipi_lcd_off,
//[ECID 000000] zhangqi add for CABC begin
#ifdef CONFIG_BACKLIGHT_CABC
	.set_backlight = mipi_set_backlight,
#else
	.set_backlight = mipi_zte_set_backlight,
#endif
};

//[ECID 000000]zhangqi add for CABC begin
#ifdef CONFIG_BACKLIGHT_CABC
void mipi_set_backlight(struct msm_fb_data_type *mfd)
{
         /*value range is 1--32*/
	 int current_lel = mfd->bl_level;
	 //unsigned long flags;
	 
	 //[ECID:0000]ZTE_BSP maxiaoping 20120803 disable print logs,start.
	 pr_debug("zhangqi add for CABC level=%d lcd_type=%d in %s func \n ",current_lel,lcd_id_type,__func__);
	 //[ECID:0000]ZTE_BSP maxiaoping 20120803 disable print logs,end.
	 
	 if (current_lel >32)
	 	{
	 			printk("Backlight level >32 ? return error. CABC level=%d in %s func \n ",current_lel,__func__);
	 			return;
	 	}
	 	
	 mipi_zte_set_backlight(mfd);


	 if (lcd_id_type==4 || lcd_id_type==5 || lcd_id_type==6 || lcd_id_type ==7 )
	 {
	   //[ECID:0000]ZTE_BSP maxiaoping 20120803 disable print logs,start.
	   pr_debug("zhangqi add for CABC it is a hx8369 IC \n");
	   //[ECID:0000]ZTE_BSP maxiaoping 20120803 disable print logs,end.
	   if(current_lel==0)
	   {
	    mipi_set_tx_power_mode(0);
		  mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_display_off_CABC_backlight_cmds,ARRAY_SIZE( hx8369_display_off_CABC_backlight_cmds));
			mipi_set_tx_power_mode(1);
			msleep(500);

	   }
	   else
	   {
	    hx8369_para_CABC_0x51[1]=(8*current_lel-1);     
	    mipi_set_tx_power_mode(0);
		  mipi_dsi_cmds_tx(&lead_tx_buf, hx8369_display_on_CABC_backlight_cmds,ARRAY_SIZE( hx8369_display_on_CABC_backlight_cmds));
 			mipi_set_tx_power_mode(1);
	   }

	 }
	 else if(lcd_id_type==3 || lcd_id_type==9)
	 	{	
	 		//[ECID:0000]ZTE_BSP maxiaoping 20120803 disable print logs,start.
	 		pr_debug("zhangqi add for CABC it is a hx8363 IC \n");
			//[ECID:0000]ZTE_BSP maxiaoping 20120803 disable print logs,end.
	 		if(current_lel==0)
	   {
	    //  hx8363_para_CABC_0x51[1]=0x00;
			mipi_set_tx_power_mode(0);
			mipi_dsi_cmds_tx(&lead_tx_buf, hx8363_display_off_CABC_backlight_cmds,ARRAY_SIZE( hx8363_display_off_CABC_backlight_cmds));
			mipi_set_tx_power_mode(1);
			msleep(500);
	   }
	 		else
	   {
	      hx8363_para_CABC_0x51[1]=(8*current_lel-1);//;      
			mipi_set_tx_power_mode(0);
			mipi_dsi_cmds_tx(&lead_tx_buf, hx8363_display_on_CABC_backlight_cmds,ARRAY_SIZE( hx8363_display_on_CABC_backlight_cmds));
			mipi_set_tx_power_mode(1);
	   }
	 	}
	 else
	 	 printk("zhangqi add for CABC it is ?? IC \n");
	   return;


	 
}
#endif
//[ECID 000000]zhangqi add for CABC end
static int ch_used[3];

int mipi_lead_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	pdev = platform_device_alloc("mipi_lead", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	lead_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &lead_panel_data,
		sizeof(lead_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}


static int __devinit mipi_lead_lcd_probe(struct platform_device *pdev)
{	
//	struct msm_panel_info   *pinfo =&( ((struct msm_fb_panel_data  *)(pdev->dev.platform_data))->panel_info);
	
	if (pdev->id == 0) return 0;
	
	mipi_dsi_buf_alloc(&lead_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&lead_rx_buf, DSI_BUF_SIZE);
//[ECID:0000] ZTEBSP wangminrong start 20120730 read lcd id from bootloader
	if((LcdPanleID = get_lcdpanleid_from_bootloader() )==(u32)LCD_PANEL_NOPANEL)
		pr_debug("cann't get get_lcdpanelid from bootloader\n");
//[ECID:0000] ZTEBSP wangminrong start 20120730 read lcd id from bootloader	

    printk("wjp mipi_lead_lcd_probe LcdPanleID:%d \n",LcdPanleID);

/*	if (LcdPanleID ==LCD_PANEL_4P0_R61408_TRULY_LG)//this panel is different from others
	{
		pinfo->lcdc.h_back_porch = 80;
		pinfo->lcdc.h_front_porch = 180;	
		pinfo->lcdc.v_back_porch = 12;	
	}
*/

	msm_fb_add_device(pdev);

	return 0;
}

static struct platform_driver this_driver = {
	.probe  = mipi_lead_lcd_probe,
	.driver = {
		.name   = "mipi_lead",
	},
};

static int __init mipi_lcd_init(void)
{
	return platform_driver_register(&this_driver);
}

module_init(mipi_lcd_init);
//[ECID:0000] ZTEBSP wangminrong end 20120411 for lcd jianrong 

