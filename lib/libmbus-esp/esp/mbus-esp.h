#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "user_config.h"
#include "mem.h"

#define snprintf os_snprintf
#define malloc os_malloc
#define free os_free
#define printf os_printf
#define realloc os_realloc
#define fprintf(X, ...) os_printf(__VA_ARGS__)
//#define stderr 0
#define pow(A, B) 0
#define strdup os_strdup
#define strerror(X) ""
#define open  os_open
#define read  os_read
#define write os_write
#define close os_close
#define isatty  os_isatty

char *os_strdup(const char *s);

typedef uint32_t time_t;

struct tm
{
  uint8_t tm_sec;
  uint8_t tm_min;
  uint8_t tm_hour;
  uint8_t tm_mday;
  uint8_t tm_mon;
  uint8_t tm_wday;
  uint16_t tm_year;
  uint16_t tm_yday;
  uint8_t tm_isdst;
  uint8_t pad;
};

struct tm *gmtime(const time_t *timep);
size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
time_t time(time_t *t);

typedef uint32_t itcflag_t;
typedef uint32_t tcflag_t;
typedef uint32_t cc_t;
#define NCCS 4
struct termios
{
  itcflag_t c_iflag;      /* input modes */
  tcflag_t c_oflag;      /* output modes */
  tcflag_t c_cflag;      /* control modes */
  tcflag_t c_lflag;      /* local modes */
  cc_t     c_cc[NCCS];   /* special characters */
};

//double pow(double x, double y);

typedef uint32 speed_t;
typedef uint32 ssize_t;

#define O_RDWR   1
#define O_NOCTTY 2
#define CS8  1
#define CREAD 2
#define CLOCAL 4
#define PARENB 8
#define VMIN 0
#define VTIME 1
#define TCSANOW 1
#define SSIZE_MAX 16
#define B300   300
#define B600   600
#define B1200  1200
#define B2400  2400
#define B4800  4800
#define B9600  9600
#define B19200 19200
#define B38400 38400

#define assert(test)  if (!(test)) { os_printf("!!! ASSERT FAILED !!!"); }

int os_open(const char *pathname, int flags);
ssize_t os_read(int fd, void *buf, size_t count);
ssize_t os_write(int fd, const void *buf, size_t count);
int os_close(int fd);

int tcdrain(int fd);
int cfsetispeed(struct termios *termios_p, speed_t speed);
int cfsetospeed(struct termios *termios_p, speed_t speed);
int tcsetattr(int fd, int optional_actions,
              const struct termios *termios_p);
int isatty(int fd);              

time_t time(time_t *t);
struct tm *gmtime(const time_t *timep);
size_t strftime(char *s, size_t max, const char *format,
                const struct tm *tm);
