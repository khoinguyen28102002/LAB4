/*
 * scheduler.c
 *
 *  Created on: Nov 26, 2022
 *      Author: USER
 */
#include "scheduler.h"

void SCH_Init(){
	unsigned char i;
	for(i = 0; i < SCH_MAX_TASK; i++){
		SCH_Remove_Task(i);
	}
}
uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	if(!DELAY && !PERIOD)
		return 0;
	DELAY = DELAY/TIME_CYCLE;
	PERIOD = PERIOD/TIME_CYCLE;
	unsigned char i = 0;
	while((SCH_tasks[i].pTask) && (i < SCH_MAX_TASK) && (DELAY > SCH_tasks[i].Delay)){
		DELAY -= SCH_tasks[i].Delay;
		i++;
	}
	if(i == SCH_MAX_TASK){
		return 0;
	}
	else{
		unsigned char j;
		for(j = SCH_MAX_TASK - 1; j > i; j--){
			SCH_tasks[j] = SCH_tasks[j-1];
		}
		SCH_tasks[j].pTask = pFunction;
		SCH_tasks[j].Delay = DELAY;
		SCH_tasks[j].RunMe = 0;
		SCH_tasks[j].Period = PERIOD;
		SCH_tasks[j].TaskID = i;
		SCH_tasks[j+1].Delay -= DELAY;
	}
	return SCH_tasks[i].TaskID;
}
void SCH_Update(void){
	if(SCH_tasks[0].pTask){
		if(SCH_tasks[0].Delay <= 0){
			SCH_tasks[0].RunMe += 1;
		}
		else{
			SCH_tasks[0].Delay--;
		}
	}
}
void SCH_Dispatch_Task(void){
	unsigned int i;
	sTask temp;
	for(i = 0; i < SCH_MAX_TASK; i++){
		if(SCH_tasks[i].RunMe > 0){
			(*SCH_tasks[i].pTask)();
			SCH_tasks[i].RunMe--;
			temp = SCH_tasks[i];
			SCH_Remove_Task(i);
			if(temp.Period != 0){
				uint32_t Period = temp.Period*TIME_CYCLE;
				SCH_Add_Task(temp.pTask, Period, Period);
			}
		}
	}
}
uint8_t SCH_Remove_Task(uint32_t TaskID){
	if(SCH_tasks[TaskID].pTask == 0){
		return 0;
	}
	for(unsigned char i = 0; i < SCH_MAX_TASK - 1; i++){
		SCH_tasks[i] = SCH_tasks[i+1];
	}
	SCH_tasks[SCH_MAX_TASK-1].pTask = 0x0000;
	SCH_tasks[SCH_MAX_TASK-1].Delay = 0;
	SCH_tasks[SCH_MAX_TASK-1].RunMe = 0;
	SCH_tasks[SCH_MAX_TASK-1].Period = 0;
	SCH_tasks[SCH_MAX_TASK-1].TaskID = 0;
	return 1;
}
//void SCH_Go_To_Sleep(void);
