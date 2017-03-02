#include <stdio.h>
#include "includes.h"
#include "system.h"
#include "altera_up_avalon_character_lcd.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_timer.h"
#include "sys/alt_timestamp.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
#define   Q_SIZE_BYTES         2048
OS_STK    lcd_stk[TASK_STACKSIZE];

OS_EVENT  *msg_q;

char      buf[Q_SIZE_BYTES];

/*            Constants          */

#define OS_TIME_LCD_DELAY_HOURS       0
#define OS_TIME_LCD_DELAY_MINUTES     0
#define OS_TIME_LCD_DELAY_SECONDS     0
#define OS_TIME_LCD_DELAY_MILLISEC    100

#define LCD_CURSOR_START_ROW_1_X  0
#define LCD_CURSOR_START_ROW_1_Y  0

#define OS_Q_PEND_WAIT_FOREVER    0

#define GREEN_LED_ALL_ON          0xFF
#define GREEN_LED_ALL_OFF         0x0

/* Definition of Task Priorities */

#define LCD_TASK_PRIORITY         1

/*
 * Prints error to console
 */
void disp_err(INT8U err, char* msg){
	printf("%s:%d\n", msg, err);
}

static void isr_gate_timer (void* context)
{

	IOWR_ALTERA_AVALON_TIMER_STATUS(GATE_TIMER_BASE,0);
	INT8U* curr_state = (INT8U*)GREEN_LEDS_BASE;
	if(*curr_state == GREEN_LED_ALL_ON){
		*curr_state = GREEN_LED_ALL_OFF;
	}else{
		*curr_state = GREEN_LED_ALL_ON;
	}
	INT32U* count = (INT32U*)FREQUENCY_COUNTER_0_BASE;
	OSQPost(msg_q, (void*) count);

}

/* Prints "Hello World" when the switch changes state */
void lcd_task(void* pdata)
{
  static INT32U last_count = 0;
  INT8U err;
  alt_up_character_lcd_dev *lcd;
  lcd = alt_up_character_lcd_open_dev(CHARACTER_LCD_0_NAME);
  alt_up_character_lcd_init(lcd);
  char str[15];
  while (1)
  {
	INT32U count = *(INT32U *)OSQPend(msg_q, OS_Q_PEND_WAIT_FOREVER, &err);
	if(err) disp_err(err, "Error pending on q");
	sprintf(str, "%Lu", count - last_count);
	last_count = count;
	alt_up_character_lcd_init(lcd);
	alt_up_character_lcd_set_cursor_pos(lcd, LCD_CURSOR_START_ROW_1_X, LCD_CURSOR_START_ROW_1_Y);
	alt_up_character_lcd_string(lcd, str);
    err = OSTimeDlyHMSM(OS_TIME_LCD_DELAY_HOURS, OS_TIME_LCD_DELAY_MINUTES, OS_TIME_LCD_DELAY_SECONDS, OS_TIME_LCD_DELAY_MILLISEC);
    if(err) disp_err(err, "Error delaying");
  }
}

/* The main function creates three tasks and starts multi-tasking */
int main(void)
{
  msg_q = OSQCreate((void*)buf, Q_SIZE_BYTES / sizeof(char));
  if(msg_q == NULL) {
	  printf("failed to init q\n");
	  return 1;
  }

  alt_ic_isr_register(GATE_TIMER_IRQ_INTERRUPT_CONTROLLER_ID, GATE_TIMER_IRQ,
		  &isr_gate_timer, NULL, NULL);

  INT8U err = OSTaskCreateExt(lcd_task,
                  NULL,
                  (void *)&lcd_stk[TASK_STACKSIZE-1],
                  LCD_TASK_PRIORITY,
                  LCD_TASK_PRIORITY,
                  lcd_stk,
                  TASK_STACKSIZE,
                  NULL,
                  OS_TASK_OPT_NONE);

  if(err)
	  disp_err(err, "Error starting task");

  *(INT8U*)GREEN_LEDS_BASE = GREEN_LED_ALL_ON;

  IOWR_ALTERA_AVALON_TIMER_CONTROL(GATE_TIMER_BASE, ALTERA_AVALON_TIMER_CONTROL_CONT_MSK | ALTERA_AVALON_TIMER_CONTROL_START_MSK | ALTERA_AVALON_TIMER_CONTROL_ITO_MSK);
  printf("Starting...\n");
  OSStart();
  return 0;
}

