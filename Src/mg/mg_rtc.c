#include "mg_rtc.h"
#include "stm32l0xx_hal.h"
#include "string.h"
#include "global_defs.h"
#include "mg_state_machine.h"

extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart1;

void RTC_GetCompileDate(RTC_DateTypeDef *rtcDate)
{
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char datestr[] = __DATE__;
	char monthstr[5];
	int month, day, year;
	
	sscanf(datestr, "%s %d %d", monthstr, &day, &year);
	month = ((strstr(month_names, monthstr)-month_names)/3) + 1;
	year -= 2000;
	
	rtcDate->Year  = year;
	rtcDate->Month = month;
	rtcDate->Date  = day;
	
	#ifdef DEBUG_RTC
	char debugString[50];
	sprintf(debugString, "\n\rRTC date set to %d %d %d", day, month, year);
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
	#endif
}

void RTC_GetCompileTime(RTC_TimeTypeDef *rtcTime)
{
	char timestr[] = __TIME__;
	int hour, minute, second;
	
	timestr[2] = ' ';
	timestr[5] = ' ';
	
	sscanf(timestr, "%d %d %d", &hour, &minute, &second);
	
	rtcTime->Hours   = hour;
	rtcTime->Minutes = minute;
	rtcTime->Seconds = second;
	
	#ifdef DEBUG_RTC
	char debugString[50];
	sprintf(debugString, "\n\rRTC time set to %d %d %d", hour, minute, second);
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
	#endif
}

void RTC_Set(void)
{
	RTC_DateTypeDef rtcDate;
	RTC_TimeTypeDef rtcTime;
	
	RTC_GetCompileDate(&rtcDate);
	RTC_GetCompileTime(&rtcTime);
	
	HAL_RTC_SetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
}

void RTC_Get(RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
{
	HAL_RTC_GetDate(&hrtc, date, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, time, RTC_FORMAT_BIN);
	
	#ifdef DEBUG_RTC
	char debugString[50];
	sprintf(debugString, "\n\r%d/%d/%d %d:%d:%d", date->Date, date->Month, date->Year, time->Hours, time->Minutes, time->Seconds);
	HAL_UART_Transmit(&huart1, (uint8_t*)debugString, strlen(debugString), 500);
	#endif
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	// Check if alarm A has triggered the interrupt
	if(RTC->ISR & RTC_ISR_ALRAF)
	{
		RTC_DateTypeDef rtcDate;
		RTC_TimeTypeDef rtcTime;
		
		/* Get the current time */
		RTC_Get(&rtcDate, &rtcTime);
		
		/* Work out the next alarm interval */
		const uint8_t alarmInterval[4] = { 15, 30, 45, 0 };
		uint8_t intervalIndex = (rtcTime.Seconds / 15);
		
		/* Disable write protection for RTC registers */
		RTC->WPR = 0xCA;
		RTC->WPR = 0x53;
		
		/* Disable alarm A */
		RTC->CR &= ~RTC_CR_ALRAE;
		
		/* Wait for alarm A write flag to be set */
		while(!(RTC->ISR | RTC_ISR_ALRAWF)) {}
		
		/* Reconfigure alarm A */
		RTC_AlarmTypeDef sAlarm;
		sAlarm.AlarmTime.Hours = 0x0;
		sAlarm.AlarmTime.Minutes = 0x0;
		sAlarm.AlarmTime.Seconds = alarmInterval[intervalIndex];
		sAlarm.AlarmTime.SubSeconds = 0x0;
		sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
		sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS
																|RTC_ALARMMASK_MINUTES;
		sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
		sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
		sAlarm.AlarmDateWeekDay = 0x1;
		sAlarm.Alarm = RTC_ALARM_A;
		if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
		
		/* Re-enable alarm A */
		RTC->CR |= RTC_CR_ALRAE;
		
		/* Re-enable write protection for RTC registers */
		RTC->WPR = 0xFF;
		
		/* Set the RTC interrupt flag */
		rtcInterrupt = 1;
	}
}

