/*
 * espi-mbus
 *
 * Copyright (C) 2019 packom.net Limited
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version. 
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "esp_sdk_ver.h"
#include "user_config.h"
#include "softuart.h"

#define user_procTaskPrio        0
#define user_procTaskQueueLen    1
os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static void user_procTask(os_event_t *events);

static os_timer_t some_timer;

Softuart uart;
#define REQ_DATA_LEN  5
unsigned char req_data[REQ_DATA_LEN] = {0x10, 0x5b, 0x30, 0x8b, 0x16};
extern int ets_printf(const char *fmt, ...);

void some_timerfunc(void *arg)
{
  int ii;
  unsigned char rcv_data;

  ets_printf("some_timerfunc\n");

  os_printf("Initialize UART\n");
  Softuart_SetPinTx(&uart, 4);
  Softuart_SetPinRx(&uart, 5);
  Softuart_Init(&uart, 2400);

  os_printf("Send MBus request data: ");
  for (ii = 0; ii < REQ_DATA_LEN; ii++)
  {
    os_printf("%2.2x", req_data[ii]);
    Softuart_Putchar(&uart, req_data[ii]);
  }
  os_printf("\n");

  // Give time for response
  os_delay_us(100000);

  os_printf("Received MBus data: ");
  while (Softuart_Available(&uart))
  {
    rcv_data = Softuart_Read(&uart);
    os_printf("%2.2x ", rcv_data);
  }
  os_printf("\n");

  os_printf("Done\n");
}

static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{
  os_delay_us(10);
}

void ICACHE_FLASH_ATTR user_init(void)
{
  // According to docs required as first step, to enable us timer
  // Note that this means that maximum os_timer_arm (not us) value is about
  // 432s (0x689D0 in ms)
  system_timer_reinit();

  // Disarm timer
  os_timer_disarm(&some_timer);

  // Setup timer
  os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

  // Arm the timer
  // &some_timer is the pointer
  // 1000 is the fire time in ms
  // 0 for once and 1 for repeating
  os_timer_arm(&some_timer, 1000, 0);
    
  // Start os task
  system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
}

#if ESP_SDK_VERSION >= 030000
void ICACHE_FLASH_ATTR user_pre_init(void)
{
  bool rc = false;
  static const partition_item_t part_table[] = 
  {
    {SYSTEM_PARTITION_RF_CAL,
     0x3fb000,
     0x1000},
    {SYSTEM_PARTITION_PHY_DATA,
     0x3fc000,
     0x1000},
    {SYSTEM_PARTITION_SYSTEM_PARAMETER,
     0x3fd000,
     0x3000},
  };

  // This isn't an ideal approach but there's not much point moving on unless
  // or until this has succeeded cos otherwise the SDK will just barf and 
  // refuse to call user_init()
  while (!rc)
  {
    rc = system_partition_table_regist(part_table,
				       sizeof(part_table)/sizeof(part_table[0]),
                                       4);
  }

  return;
}
#endif // ESP_SDK_VERSION >= 030000