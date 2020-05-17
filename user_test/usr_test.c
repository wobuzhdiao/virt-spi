#include "usr_test.h"




void printf_useage(void)
{
    int i = 0 ;

    printf("Useage: ./usr_test -w/r  reg_addr -c value\n");

    printf("-w write value to reg_addr[need append -c set_regname]\n");
    printf("-r read value from reg_addr\n");
    return;
}
typedef enum
{
    OP_NONE,
    OP_READ,
    OP_WRITE
}opration_mode_e;

int main(int argc ,char *argv[])
{
    int fd;
    int ret = 0;
    int reg_addr = 0;
    int reg_val = -1;
    char ch;
    int action = 0;

    virtual_spi_ioctl_cmd_t cmd_info;


    if (argc == 1)
    {
        printf_useage();
        exit(1);
    }

    while(((ch = getopt(argc, argv, "hr:w:c:")) != -1)&&(optind <= argc))
    {
        switch(ch)
        {
            case 'w':
                action = OP_WRITE;
                reg_addr = atoi(optarg);
                break;

            case 'r':
                action = OP_READ;
                reg_addr = atoi(optarg);
                
                break;

            case 'c':
                reg_val = atoi(optarg);
                break;
                break;
            case 'h':
                printf_useage();
                exit(1);
            default:
                printf_useage();
                exit(1);
        }
    }
    
    if(action == OP_WRITE)
    {
        if(reg_val == -1)
        {
            printf("reg addr is invalid\n");
            return -1;
        }
    }

    memset(&cmd_info, 0, sizeof(virtual_spi_ioctl_cmd_t));

    fd = open(DEV_NAME, O_RDWR);
    if(fd < 0)
    {
        printf("%s:open failed\n", __FUNCTION__);
        return -1;
    }

    if (action == OP_NONE)
    {
        printf("no action, opt error!!!\n");
        close(fd);
        return 0;
    }

    if (action == OP_WRITE)
    {
        cmd_info.reg = reg_addr;
        cmd_info.val = reg_val;
        ret = ioctl(fd, BOARD_SET_REG_VALUE, &cmd_info);
        if(ret < 0)
        {
            printf("%s:set reg[%d] value failed\n", __FUNCTION__, cmd_info.reg);
        }
        else
        {
            printf("%s:set reg[%d] value success\n", __FUNCTION__, cmd_info.reg);
        }
    }
    else if (action == OP_READ)
    {
        cmd_info.reg = reg_addr;
        cmd_info.val = 0;
        
        ret = ioctl(fd, BOARD_GET_REG_VALUE, &cmd_info);
        if(ret < 0)
        {
            printf("%s:get reg[%d] value failed\n", __FUNCTION__, cmd_info.reg);
        }
        else
        {
            printf("%s:reg[%d]=0x%x \n", __FUNCTION__, cmd_info.reg, cmd_info.val);
        }
    }

    close(fd);


    return 0;
}


