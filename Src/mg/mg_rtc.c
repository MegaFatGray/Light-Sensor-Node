#include "mg_rtc.h"
#include "stm32l0xx_hal.h"
#include "string.h"
#include "global_defs.h"

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
	
	#ifdef DEBUG_RTC_OUTPUTS
	char myDateString[50];
	sprintf(myDateString, "\n\rRTC date set to %d %d %d", day, month, year);
	HAL_UART_Transmit(&huart1, (uint8_t*)myDateString, strlen(myDateString), 500);
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
	
	#ifdef DEBUG_RTC_OUTPUTS
	char myTimeString[50];
	sprintf(myTimeString, "\n\rRTC time set to %d %d %d", hour, minute, second);
	HAL_UART_Transmit(&huart1, (uint8_t*)myTimeString, strlen(myTimeString), 500);
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

void RTC_Get(void)
{
	RTC_DateTypeDef rtcDate;
	RTC_TimeTypeDef rtcTime;
	
	HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
	
	#ifdef DEBUG_RTC_OUTPUTS
	char rtcString[50];
	sprintf(rtcString, "\n\r%d/%d/%d %d:%d:%d", rtcDate.Date, rtcDate.Month, rtcDate.Year, rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds);
	HAL_UART_Transmit(&huart1, (uint8_t*)rtcString, strlen(rtcString), 500);
	#endif
}

