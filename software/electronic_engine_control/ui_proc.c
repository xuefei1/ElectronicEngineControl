/*
 * ui_proc.c
 *
 * Status: N
 *
 *  Created on: Feb 4, 2017
 *      Author: xuefei1
 *
 *      This process is mostly used for debugging
 *      for now, it only interfaces with the LCD
 *      so we could also call it LCD process
 */

#include "ui_proc.h"

/* Used to store debug msg */
OS_EVENT *lcd_msg_q;

char lcd_msg_buf[LCD_MSG_Q_SIZE_BYTE];

void ui_task(void* pdata) {
	INT8U err;
//	alt_up_character_lcd_dev *lcd;
//	lcd = alt_up_character_lcd_open_dev(CHARACTER_LCD_0_NAME);
//	alt_up_character_lcd_init(lcd);
	lcd_msg_q = OSQCreate((void*) lcd_msg_buf,
			LCD_MSG_Q_SIZE_BYTE / sizeof(char));
	while (1) {
		char* msg = (char *) OSQPend(lcd_msg_q, Q_TIMEOUT_WAIT_FOREVER, &err);
		if (err)
			disp_err(err, "Error pending on q");
//		alt_up_character_lcd_init(lcd);
//		alt_up_character_lcd_set_cursor_pos(lcd, LCD_CURSOR_START_ROW_1_X,
//				LCD_CURSOR_START_ROW_1_Y);
//		alt_up_character_lcd_string(lcd, msg);
		err = OSTimeDlyHMSM(UI_TASK_DELAY_HOURS, UI_TASK_DELAY_MINUTES,
				UI_TASK_DELAY_SECONDS, UI_TASK_DELAY_MILLISEC);
		if (err)
			disp_err(err, "Error delaying");
	}
}

/* Getter for LCD msg Q */
OS_EVENT *get_lcd_msg_q() {
	return lcd_msg_q;
}
