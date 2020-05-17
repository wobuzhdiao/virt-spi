#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/spi/spi.h>
#include <linux/wait.h>
#include <linux/param.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/kdev_t.h>

#include <linux/cdev.h>


#define DEV_NAME "virtual_spidev"
#define QCTEK_SPI_TRANSFER_BUF_LEN 4



typedef struct __virtual_spi_dev_s
{
	char name[32];
	struct cdev chr_dev;
	struct spi_device *spi;
	dev_t dev_num;	
    struct mutex mlock;
	struct device *base_dev;
    struct class *fpga_chr_classp;
	uint8_t spi_read_buf[QCTEK_SPI_TRANSFER_BUF_LEN];
	uint8_t spi_write_buf[QCTEK_SPI_TRANSFER_BUF_LEN];
}virtual_spi_dev_t;



typedef struct __virtual_spi_ioctl_cmd_s
{
    u16 reg;
    u16 val;
}virtual_spi_ioctl_cmd_t;


#define DEVICE_MAGIC 'b'
#define BOARD_SET_REG_VALUE            _IOW(DEVICE_MAGIC, 0x01, virtual_spi_ioctl_cmd_t)      /* set reg vaule*/
#define BOARD_GET_REG_VALUE            _IOR(DEVICE_MAGIC, 0x02, virtual_spi_ioctl_cmd_t)      /* get reg vaule*/
