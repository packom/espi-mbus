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
#include "mbus.h"

// Globals
static os_timer_t mbus_response_timer;
mbus_handle *handle = NULL;

// Function protoypes
static int init_mbus_slaves(mbus_handle *handle);
void handle_mbus_response(void *arg);
void send_mbus_query(void);

void ICACHE_FLASH_ATTR user_init(void)
{
  // According to docs required as first step, to enable us timer
  // Note that this means that maximum os_timer_arm (not us) value is about
  // 432s (0x689D0 in ms)
  system_timer_reinit();

  uart_div_modify(0, UART_CLK_FREQ / 74880);
  uart_div_modify(1, UART_CLK_FREQ / 74880);

  system_init_done_cb((init_done_cb_t)send_mbus_query);
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

void
ICACHE_FLASH_ATTR
send_mbus_query(void)
{
  char *device = "/dev/espi";
  int address = 48;
  long baudrate = 2400;

  os_printf("MBUS: Initialize M-Bus library\n");
  if ((handle = mbus_context_serial(device)) == NULL)
  {
    os_printf("MBUS: Could not initialize M-Bus library: %s\n",  mbus_error_str());
    return;
  }

  os_printf("MBUS: Set up serial port\n");
  if (mbus_connect(handle) == -1)
  {
    os_printf("MBUS: Failed to setup serial port\n");
    mbus_context_free(handle);
    return;
  }

  os_printf("MBUS: Set baudrate\n");
  if (mbus_serial_set_baudrate(handle, baudrate) == -1)
  {
    os_printf("MBUS: Failed to set baud rate\n");
    mbus_disconnect(handle);
    mbus_context_free(handle);
    return;
  }

  os_printf("MBUS: Init slaves\n");
  if (init_mbus_slaves(handle) == 0)
  {
    os_printf("MBUS: Failed to init slaves\n");
    mbus_disconnect(handle);
    mbus_context_free(handle);
    return;
  }

  os_printf("MBUS: Send request\n");
  if (mbus_send_request_frame(handle, address) == -1)
  {
    os_printf("MBUS: Failed to send M-Bus request\n");
    mbus_disconnect(handle);
    mbus_context_free(handle);
    return;
  }

  // Handle the response - need to leave it at least 400-500ms
  os_timer_disarm(&mbus_response_timer);
  os_timer_setfn(&mbus_response_timer, (os_timer_func_t *)handle_mbus_response, NULL);
  os_timer_arm(&mbus_response_timer, 1000, 0);

  os_printf("MBUS: Sent request\n");

  return;
}

void
ICACHE_FLASH_ATTR
handle_mbus_response(void *arg)
{
  mbus_frame reply;
  mbus_frame_data reply_data;
  char *xml_result;

  os_printf("MBUS: Receive response\n");
  if (mbus_recv_frame(handle, &reply) != MBUS_RECV_RESULT_OK)
  {
    os_printf("MBUS: Failed to receive M-Bus response\n");
    return;
  }

  os_printf("MBUS: Parse frame\n");
  if (mbus_frame_data_parse(&reply, &reply_data) == -1)
  {
    os_printf("MBUS: M-bus data parse error: %s\n", mbus_error_str());
    mbus_disconnect(handle);
    mbus_context_free(handle);
    return;
  }

  os_printf("MBUS: Generate XML\n");
  if ((xml_result = mbus_frame_data_xml(&reply_data)) == NULL)
  {
    os_printf("MBUS: Failed to generate XML: %s\n", mbus_error_str());
    mbus_disconnect(handle);
    mbus_context_free(handle);
    return;
  }

  os_printf("%s\n", xml_result);

  return;
}

static int
ICACHE_FLASH_ATTR
init_mbus_slaves(mbus_handle *handle)
{
    if (mbus_send_ping_frame(handle, MBUS_ADDRESS_NETWORK_LAYER, 1) == -1)
    {
        return 0;
    }

    if (mbus_send_ping_frame(handle, MBUS_ADDRESS_NETWORK_LAYER, 1) == -1)
    {
        return 0;
    }

    return 1;
}
