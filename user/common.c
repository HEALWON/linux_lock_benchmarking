#include "common.h"

void spin(long duration)
{
  struct timeval start, uptime, now;
  struct timeval add = {0, duration};

  gettimeofday(&start, NULL);
  timeradd(&start, &add, &uptime);
  
  do {
    gettimeofday(&now, NULL);
    if (timercmp(&now, &uptime, >)) break;
  } while (1);
}