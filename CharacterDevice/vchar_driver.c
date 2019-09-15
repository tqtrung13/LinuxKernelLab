#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>

#include "vchar_driver.h"

#define DRIVER_AUTHOR "Tran Quoc Trung"
#define DRIVER_DESC "A sample for device character"
#define DRIVER_VERSION "0.5"

typedef struct vchar_dev {
    unsigned char * control_regs;
    unsigned char * status_regs;
    unsigned char * data_regs;
} vchar_dev_t;

struct _vchar_drv {
    dev_t dev_num;
    struct class *dev_class;
    struct device *dev;
    vchar_dev_t * vchar_hw;
} vchar_drv;

int vchar_hw_init(vchar_dev_t *hw)
{
    char *buf;
    buf = kzalloc(NUM_DEV_REGS * REG_SIZE, GFP_KERNEL);
    if ( !buf ) {
        return -ENOMEM;
    }

    hw->control_regs = buf;
    hw->status_regs = hw->control_regs + NUM_CTRL_REGS;
    hw->data_regs = hw->status_regs + NUM_STS_REGS;

    hw->control_regs[CONTROL_ACCESS_REG] = 0x03;
    hw->control_regs[DEVICE_STATUS_REG] = 0x03;

    return 0;
}

void vchar_hw_exit(vchar_dev_t *hw)
{
    kfree(hw->control_regs);
}


static int __init vchar_driver_init(void)
{
    int ret = 0;
    vchar_drv.dev_num = 0;

    ret = alloc_chrdev_region(&vchar_drv.dev_num, 0, 1, "vchar_device");
    if (ret < 0) {
        printk("failed to register device number dynamically\n");
        goto failed_register_device;
    }
    printk("allocate device number (%d,%d)\n", MAJOR(vchar_drv.dev_num), MINOR(vchar_drv.dev_num));

    /*create device file*/
    vchar_drv.dev_class = class_create(THIS_MODULE, "class_vchar_dev");
    if (vchar_drv.dev_class == NULL) {
        printk("failed to create a device class \n");
        goto failed_create_class;
    }
    vchar_drv.dev = device_create(vchar_drv.dev_class, NULL, vchar_drv.dev_num, NULL, "vchar_dev");
    if (IS_ERR(vchar_drv.dev)) {
        printk("failed to create a device \n");
        goto failed_create_dev;
    }

    vchar_drv.vchar_hw = kzalloc(sizeof(vchar_dev_t), GFP_KERNEL);
    if (!vchar_drv.vchar_hw == NULL) {
        printk("failed to allocate virtual device character\n");
        ret = -ENOMEM;
        goto failed_allocate_structure;
    }

    ret = vchar_hw_init(vchar.vchar_hw);
    if (ret < 0) {
        printk("failed to Initialize a virtual character");
        goto failed_init_hw;
    }
    printk("Initialize character driver successfully");
    return 0;

failed_init_hw:
    kfree(vchar_drv.vchar_hw);
failed_allocate_structure:
    device_destroy(vchar.dev_class, vchar_drv.dev_num);
failed_create_class:
    class_destroy(vchar_drv.dev_class);
failed_create_dev:
    device_destroy(vchar_drv.dev_class, char_dev.dev_num);
failed_register_device:
    return ret;
}

static void __exit vchar_driver_exit(void)
{
    vchar_hw_exit(vchar_dev.vchar_hw);
    kfree(vchar_dev.vchar_hw);
    device_destroy(vchar_drv.dev_class, char_dev.dev_num);
    class_destroy(vchar_drv.dev_class);
    unregister_chrdev_region(vchar_drv.dev_num);
    printk("Exit character device");
}

module_init(vchar_driver_init);
module_exit(vchar_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
MODULE_SUPPORT_DEVICE("testdevice");
