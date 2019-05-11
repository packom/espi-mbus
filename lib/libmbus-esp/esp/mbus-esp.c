#include "mbus-esp.h"
#include "softuart.h"

char *
ICACHE_FLASH_ATTR
os_strdup(const char *s)
{
  char *buf;
  int len;

  len = os_strlen(s) + 1;
  buf = os_malloc(len);
  if (buf != NULL)
  {
    os_memcpy(buf, s, os_strlen(s)+1);
  }

  return buf;
}

Softuart uart;

int
ICACHE_FLASH_ATTR
os_open(const char *pathname, int flags)
{
  //os_printf("os_open\n");
  Softuart_SetPinTx(&uart, 4);
  Softuart_SetPinRx(&uart, 5);
  Softuart_Init(&uart, 2400);
  return 1;
}

ssize_t
ICACHE_FLASH_ATTR
os_read(int fd, void *buf, size_t count)
{
  int ii = 0;
  unsigned char *char_buf = buf;

  //os_printf("Receive serial data %d: ", count);
  while (Softuart_Available(&uart) && (ii < count))
  {
    char_buf[ii] = Softuart_Read(&uart);
    //os_printf("%2.2x ", char_buf[ii]);
    ii++;
  }
  //os_printf("\n");

  return ii;
}

ssize_t
ICACHE_FLASH_ATTR
os_write(int fd, const void *buf, size_t count)
{
  int ii;
  unsigned char *char_buf = (unsigned char *)buf;

  //os_printf("Send serial data: ");
  for (ii = 0; ii < count; ii++)
  {
    //os_printf("%2.2x", char_buf[ii]);
    Softuart_Putchar(&uart, char_buf[ii]);
  }
  //os_printf("\n");

  return ii;
}

int
ICACHE_FLASH_ATTR
os_close(int fd)
{
  return 0;
}

int
ICACHE_FLASH_ATTR
tcdrain(int fd)
{
  return 0;
}

int
ICACHE_FLASH_ATTR
cfsetispeed(struct termios *termios_p, speed_t speed)
{
  return 0;
}

int
ICACHE_FLASH_ATTR
cfsetospeed(struct termios *termios_p, speed_t speed)
{
  return 0;
}

int
ICACHE_FLASH_ATTR
tcsetattr(int fd, int optional_actions,
          const struct termios *termios_p)
{
  return 0;
}

int
ICACHE_FLASH_ATTR
isatty(int fd)
{
  return 1;
}

time_t
ICACHE_FLASH_ATTR
time(time_t *t)
{
  if (t != NULL)
  {
    *t = 0;
  }
  return 0;
}

struct tm *
ICACHE_FLASH_ATTR
gmtime(const time_t *timep)
{
  struct tm *gmtime;
  gmtime = os_malloc(sizeof(struct tm));
  if (gmtime != NULL)
  {
    os_memset(gmtime, 0, sizeof(struct tm));
  }
  return gmtime;
}

size_t
ICACHE_FLASH_ATTR
strftime(char *s, size_t max, const char *format,
         const struct tm *tm)
{
  s[0] = 0;
  return 0;
}