#include "virtual_spi_dev.h"

static struct class *spi_ctrl_class_ptr;

static int virtual_dev_open(struct inode *inode, struct file *filp)
{
    struct cdev *cdev = inode->i_cdev;
    virtual_spi_dev_t *devp = container_of(cdev, struct __virtual_spi_dev_s, chr_dev);

    filp->private_data = devp;	

    return 0;
}

static int virtual_dev_close(struct inode *inode, struct file *file)
{
    file->private_data  = NULL;
    return 0;
}

static ssize_t virtual_dev_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    int ret = 0;

    return ret;
}

static ssize_t virtual_dev_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    int ret = 0;

    return ret;
}

static int virtual_dev_set_reg(virtual_spi_dev_t *devp, virtual_spi_ioctl_cmd_t *cmdp)
{
    uint8_t buf[4];

    if(devp == NULL || cmdp == NULL)
        return -EINVAL;

    memset(buf, 0, sizeof(buf));
    buf[0] = cmdp->reg>>8;
    buf[1] = cmdp->reg &0x00FF;
    buf[2] = cmdp->val>>8;
    buf[3] = cmdp->val&0x00FF;

    return spi_write(devp->spi, buf, sizeof(buf));
}

static int virtual_dev_get_reg(virtual_spi_dev_t *devp, virtual_spi_ioctl_cmd_t *cmdp)
{
    int ret = 0;
    uint8_t buf[4];
    
    if(devp == NULL || cmdp == NULL)
        return -EINVAL;

    memset(buf, 0, sizeof(buf));
    buf[0] = cmdp->reg>>8;
    buf[1] = cmdp->reg &0x00FF;
    ret = spi_read(devp->spi, buf, sizeof(buf));
    
    if(ret == 0)
    {
        cmdp->val = buf[2]<<8 | buf[3];
    }

    return ret;
}

static long virtual_dev_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
    int ret = 0;
    virtual_spi_ioctl_cmd_t cmd_info;
    virtual_spi_dev_t *devp = file->private_data;
    
    memset(&cmd_info, 0, sizeof(virtual_spi_ioctl_cmd_t));

    switch(cmd)
    {
        case BOARD_SET_REG_VALUE:
            if(copy_from_user(&cmd_info, (void __user *)args, sizeof(virtual_spi_ioctl_cmd_t)))
                return -EINVAL;
            ret = virtual_dev_set_reg(devp, &cmd_info);
            break;
        case BOARD_GET_REG_VALUE:
            if(copy_from_user(&cmd_info, (void __user *)args, sizeof(virtual_spi_ioctl_cmd_t)))
                return -EINVAL;


            ret = virtual_dev_get_reg(devp, &cmd_info);
            if(ret != 0)
                return ret;

            if(copy_to_user((void __user *) args, &cmd_info, sizeof(virtual_spi_ioctl_cmd_t)))
                return -EFAULT;
            break;
        default:
            return -1;
    }

    return ret;
}



static struct  file_operations virtual_spi_dev_ops =
{
    .owner = THIS_MODULE,
    .open = virtual_dev_open,
    .release = virtual_dev_close,
    .unlocked_ioctl = virtual_dev_ioctl,
    .read = virtual_dev_read,
    .write = virtual_dev_write,
};


static int virtual_spi_dev_probe(struct spi_device *spi)
{
    int ret = 0;
    virtual_spi_dev_t *devp = NULL;
    uint8_t rx_buf[4];
    uint8_t tx_buf[4];
    uint16_t reg_value = 0;

    devp = kzalloc(sizeof(virtual_spi_dev_t),GFP_KERNEL);
    if(devp == NULL)
    {
        printk(KERN_ERR"%s:Init:malloc failed\n", __FUNCTION__);
        return -ENOSPC;
    }
    memcpy(devp->name, DEV_NAME, sizeof(DEV_NAME));

    devp->spi = spi;

    /*register chrdev*/
    ret = alloc_chrdev_region(&(devp->dev_num),0,1,devp->name);
    if(ret != 0)
    {
        printk(KERN_ERR"%s: can not alloc dev_num\n", __FUNCTION__);
        goto out_err_mem;
    }

    cdev_init(&devp->chr_dev, &virtual_spi_dev_ops);
    cdev_add(&devp->chr_dev,devp->dev_num, 1);

    spi_ctrl_class_ptr = class_create(THIS_MODULE, "virtual_dev_class");
    if (IS_ERR(spi_ctrl_class_ptr)) 
    {
        printk(KERN_ERR "%s:class_create() failed for bsr_class\n", __FUNCTION__);
        goto out_err_class;
    }

    devp->base_dev = device_create(spi_ctrl_class_ptr, NULL, devp->dev_num, devp, devp->name);
    if(devp->base_dev == NULL)
    {
        printk(KERN_ERR"%s:device_create failed \n", __FUNCTION__);
        goto out_err_device;
    }
    mutex_init(&devp->mlock);
    spi_set_drvdata(spi, devp);
    
    tx_buf[0] = 0x00;
    tx_buf[1] = 0x02;
    tx_buf[2] = 0x00;
    tx_buf[3] = 0x57;
    rx_buf[0] = 0x00;
    rx_buf[1] = 0x02;
    spi_write(spi, tx_buf, sizeof(tx_buf));
    spi_read(spi, rx_buf, sizeof(rx_buf));
    reg_value = rx_buf[2]<<8 | rx_buf[3];
    printk("%s:%d reg_value=%d\n", __FUNCTION__, __LINE__, reg_value);
    return 0;

out_err_device:
    class_destroy(spi_ctrl_class_ptr);
    spi_ctrl_class_ptr = NULL;
out_err_class:
    unregister_chrdev_region(devp->dev_num,1);
out_err_mem:
    kfree(devp);
    devp = NULL;
    
    return ret;
}
static int virtual_spi_dev_remove(struct spi_device *spi)
{
    int ret = 0;
    virtual_spi_dev_t *devp = spi_get_drvdata(spi);

    printk("%s:module uninstalled\n", __FUNCTION__);
    device_del(devp->base_dev);
    class_destroy(spi_ctrl_class_ptr);
    cdev_del(&devp->chr_dev);
    unregister_chrdev_region(devp->dev_num,1);
    devp->base_dev = NULL;
    kfree(devp);
    devp = NULL;
    spi_ctrl_class_ptr = NULL;


    return ret;
}

static const struct of_device_id virtual_spi_dev_match[] = {
        {
         .compatible = "virtual,virtual_spi_master_dev",
         },
        {},
};

static struct spi_driver virtual_spi_dev_driver = {
        .driver = {
                   .name = "virtual_spi_dev0",
                   .bus = &spi_bus_type,
                   .owner = THIS_MODULE,
                   .of_match_table = virtual_spi_dev_match,
                   },
        .probe = virtual_spi_dev_probe,
        .remove = virtual_spi_dev_remove,

};

static int __init vitual_spi_dev_init(void)
{
    int ret;

    ret = spi_register_driver(&virtual_spi_dev_driver);
    if(ret != 0)
    {
        printk("%s: spi register failed\n", __FUNCTION__);
    }

    return ret;
}

static void __exit vitual_spi_dev_exit(void)
{
	spi_unregister_driver(&virtual_spi_dev_driver);
}

module_init(vitual_spi_dev_init);
module_exit(vitual_spi_dev_exit);

MODULE_DESCRIPTION("Spi Device Driver For Virtual Spi Device");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("jerry_chg");
