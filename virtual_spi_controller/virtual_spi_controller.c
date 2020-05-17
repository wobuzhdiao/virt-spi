#include "virtual_spi_controller.h"


static struct spi_board_info virtual_spi_board_0 = {
    .modalias = "virtual_spi_dev0",
    .max_speed_hz = 16000000,
    .chip_select = 3,
    .mode = SPI_MODE_0,
};

static void	virtual_spi_master_dev_release(struct device *dev)
{

}

static struct platform_device gvirtual_spi_master_platform_device = {
    .name =   "virtual_spi_master_dev",
    .id = 1,
    .dev =
    {
        .release = virtual_spi_master_dev_release,
    }
};


static int virtual_spi_master_setup(struct spi_device *spi)
{
    int ret = 0;

    if(spi->chip_select >= spi->master->num_chipselect)
    {
        printk("%s:%d invalid chip_select\n", __FUNCTION__, __LINE__);
    }
    return ret;
}

static int virtual_spi_transfer(struct spi_master *master,
        struct spi_message *msg)
{
    virtual_spi_master_info_t *nspi = spi_master_get_devdata(master);
    struct spi_transfer *t;
    virtual_spi_dev_info_t *vir_spi_dev_infop;
    int result = 0;
    u32 tr_size;
    u16 reg_addr = 0;

    list_for_each_entry(t, &msg->transfers, transfer_list) {
        tr_size = t->len;
        if((t->tx_buf && t->rx_buf)||((t->tx_buf == NULL)&&(t->rx_buf == NULL)) ||(t->len < 4))
        {
            result = -EINVAL;
            break;
        }
        reg_addr = 0;
        if(t->tx_buf)
        {
            reg_addr |= ((u8 *)t->tx_buf)[0] << 8;
            reg_addr |= ((u8 *)t->tx_buf)[1];
        }
        else
        {
            reg_addr |= ((u8 *)t->rx_buf)[0] << 8;
            reg_addr |= ((u8 *)t->rx_buf)[1];
        }
        
        list_for_each_entry(vir_spi_dev_infop, &nspi->list, node)
        {
            if(vir_spi_dev_infop->chip_select == msg->spi->chip_select)
            {
                if(reg_addr >= VIRTUAL_SPI_DEV_REGS_NUM)
                {
                    result = -EINVAL;
                    goto done;
                }
                
                if(t->rx_buf)
                {
                    ((u8 *)t->rx_buf)[2] = vir_spi_dev_infop->regs[reg_addr]>>8;
                    ((u8 *)t->rx_buf)[3] = vir_spi_dev_infop->regs[reg_addr]&0x00FF;
                }
                else
                {
                    vir_spi_dev_infop->regs[reg_addr] = ((u8 *)t->tx_buf)[2]<<8|((u8 *)t->tx_buf)[3];
                }
            }
        }
    }

done:
    msg->status = result;
    spi_finalize_current_message(master);
    return result;

}


static int virtual_spi_master_platform_probe(struct platform_device *platform_dev)
{
    struct spi_master *master;
    virtual_spi_master_info_t *nspi;
    struct spi_device * spi_dev;

    master = spi_alloc_master(&(platform_dev->dev),sizeof(virtual_spi_master_info_t));
    if (!master) 
    {
        printk("%s: unable to alloc SPI master\n", __func__);
        return -EINVAL;
    }

    nspi = spi_master_get_devdata(master);
    master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST;
    master->bus_num = -1;

    master->num_chipselect = VRIUTAL_SPI_DEV_NUM;
    master->transfer_one_message = virtual_spi_transfer;
    master->setup = virtual_spi_master_setup;
    INIT_LIST_HEAD(&nspi->list);
    if (spi_register_master(master)) 
    {
        return -EINVAL;
    }

    platform_set_drvdata(platform_dev, master);

    if ((spi_dev = spi_new_device(master, &virtual_spi_board_0)) == NULL)
    {
        printk("%s:%d add spi device failed\n", __FUNCTION__, __LINE__);

        return -EINVAL;
    }
    else
    {
        virtual_spi_dev_info_t *virtual_devp = kzalloc(sizeof(virtual_spi_dev_info_t), GFP_KERNEL);
        if(NULL == virtual_devp)
        {
            spi_unregister_device(spi_dev);
            return -EINVAL;
        }
        
        virtual_devp->chip_select = spi_dev->chip_select;
        virtual_devp->regs[2] = 10;
        list_add_tail(&(virtual_devp->node), &(nspi->list));

    }



    return 0;
} 

static int virtual_spi_master_platform_remove(struct platform_device *platform_dev)
{
    struct spi_master	*master = platform_get_drvdata(platform_dev);

    spi_unregister_master(master);
    printk("%s:%d\n", __FUNCTION__, __LINE__);
    return 0;
}


static struct platform_driver virtual_spi_master_platform_driver = {
    .driver = {
        .name = "virtual_spi_master_dev",
        .owner = THIS_MODULE,
    },
    .probe = virtual_spi_master_platform_probe,
    .remove = virtual_spi_master_platform_remove,
};





static int __init virtual_spi_master_init(void)
{
    int ret = 0;

    ret = platform_device_register(&gvirtual_spi_master_platform_device);

    if(ret == 0)
    {
        ret = platform_driver_register(&virtual_spi_master_platform_driver);
    }

    return ret;
}

static void __exit virtual_spi_master_exit(void)
{
    printk("%s:%d\n", __FUNCTION__, __LINE__);
    platform_device_unregister(&gvirtual_spi_master_platform_device);
    printk("%s:%d\n", __FUNCTION__, __LINE__);

    platform_driver_unregister(&virtual_spi_master_platform_driver);
}



module_init(virtual_spi_master_init);
module_exit(virtual_spi_master_exit);
MODULE_DESCRIPTION("Virtual Spi Controller Device Drivers");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("jerry_chg");
