#include "mbus-esp.h"
#include "esp_sdk_ver.h"

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif

//Do nothing function
static void ICACHE_FLASH_ATTR
user_procTask(os_event_t *events)
{
  os_delay_us(10);
}

//Init function
void ICACHE_FLASH_ATTR
user_init()
{
  system_timer_reinit();
  uart_div_modify(0, UART_CLK_FREQ / 115200);
  uart_div_modify(1, UART_CLK_FREQ / 115200);
  os_printf("Starting ...\n");
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

void ICACHE_FLASH_ATTR assert(int expr)
{
}

struct tm *gmtime(const time_t *timep)
{
  return NULL;
}

size_t ICACHE_FLASH_ATTR strftime(char *s, size_t max, const char *format, const struct tm *tm)
{
  return 0;
}

time_t ICACHE_FLASH_ATTR time(time_t *t)
{
  return 0;
}

