#ifndef USR_TEST_H_
#define USR_TEST_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define DEV_NAME "/dev/virtual_spidev"


typedef struct __virtual_spi_ioctl_cmd_s
{
    uint16_t reg;
    uint16_t val;
}virtual_spi_ioctl_cmd_t;


#define DEVICE_MAGIC 'b'
#define BOARD_SET_REG_VALUE            _IOW(DEVICE_MAGIC, 0x01, virtual_spi_ioctl_cmd_t)      /* set reg vaule*/
#define BOARD_GET_REG_VALUE            _IOR(DEVICE_MAGIC, 0x02, virtual_spi_ioctl_cmd_t)      /* get reg vaule*/


#endif
