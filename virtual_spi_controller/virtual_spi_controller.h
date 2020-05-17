#ifndef VIRTUAL_SPI_CONTROLLER_H_
#define VIRTUAL_SPI_CONTROLLER_H_

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/kdev_t.h>
#include <linux/interrupt.h>
#include <linux/syscalls.h>
#include <linux/mount.h>
#include <linux/device.h>
#include <linux/genhd.h>
#include <linux/namei.h>
#include <linux/shmem_fs.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/vfs.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/dcache.h>
#include <linux/spi/spi.h>

#define VIRTUAL_SPI_DEV_REGS_NUM    16

#define VRIUTAL_SPI_DEV_NUM     4


typedef struct virtual_spi_dev_info_s
{
    int chip_select;
    struct list_head node;
    u16 regs[VIRTUAL_SPI_DEV_REGS_NUM];
}virtual_spi_dev_info_t;

typedef struct virtual_spi_master_info_s
{
    struct list_head list;
}virtual_spi_master_info_t;

#endif
