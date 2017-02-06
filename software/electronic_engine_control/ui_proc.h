/*
 * ui_proc.h
 *
 * Status: C
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 */

#ifndef UI_PROC_H_
#define UI_PROC_H_

#include "proj_config.h"
#include "util.h"

#define UI_TASK_PRIORITY      				3

/* Task Delays*/
#define UI_TASK_DELAY_HOURS	  				0
#define UI_TASK_DELAY_MINUTES	  			0
#define UI_TASK_DELAY_SECONDS	  			0
#define UI_TASK_DELAY_MILLISEC	  			300

#define LCD_CURSOR_START_ROW_1_X  			0
#define LCD_CURSOR_START_ROW_1_Y  			0

#define LCD_MSG_Q_SIZE_BYTE					256

void ui_task(void* pdata);

OS_EVENT *get_lcd_msg_q();

#endif /* UI_PROC_H_ */
