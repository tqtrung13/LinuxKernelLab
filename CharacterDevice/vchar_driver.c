#include <linux/module.h>
#include <linux/fs.h>

#define DRIVER_AUTHOR "Tran Quoc Trung"
#define DRIVER_DESC "A sample for device character"
#define DRIVER_VERSION "0.1"

struct _vchar_drv {
    dev_t dev_num;
} vchar_drv;

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
    printk("Initialize character driver successfully");
    return 0;
failed_register_device:
    return ret;
}

static void __exit vchar_driver_exit(void)
{
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
