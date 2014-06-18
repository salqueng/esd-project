#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/ioport.h>
#include <linux/delay.h>

#include <asm/fcntl.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define DRIVER_AUTHOR "TEAM5"
#define DRIVER_DESC "Montecarlo Simulation"

#define DRIVER_MAJOR 230
#define DRIVER_NAME "MONTECARLO"
#define DRIVER_MODULE_VERSION "MONTECARLO SIMULATOR v0.1"

#define K_ERT_ADDRESS 0x8800F000
#define K_ERT_ADDRESS_RANGE 0x2

#define SIGMA_SQRT_T_ADDRESS 0x8800F004
#define SIGMA_SQRT_T_ADDRESS_RANGE 0x2

#define S_E05_SIGMA_T_ADDRESS 0x8800F002
#define S_E05_SIGMA_T_ADDRESS_RANGE 0x2

#define M_COUNT_ADDRESS 0x8800F006
#define M_COUNT_ADDRESS_RANGE 0x4 //4-byte

//Sum, Sum Square
#define SUM_ADDRESS 0x8800F010
#define SUM_ADDRESS_RANGE 0x8

#define SQUARE_SUM_ADDRESS 0x8800F020
#define SQUARE_SUM_ADDRESS_RANGE 0x8

//Mode
#define MODE_ADDRESS 0x8800F020
#define MODE_ADDRESS_RANGE 0x2

#define MODE_0_STOP 0
#define MODE_1_START 1

//Status
#define STATUS_ADDRESS 0x8800F022
#define STATUS_ADDRESS_RANGE 0x2

#define STATUS_0_WORKING 0
#define STATUS_1_FINISHED 1

#define IOCTL_10_STOP 0x10
#define IOCTL_11_START 0x11
#define IOCTL_20_STATUS 0x20

static unsigned short *k_ert;
static unsigned short *sigma_sqrt_t;
static unsigned short *s_e05_sigma_t;
static unsigned int *M_count;

static unsigned long *sum;
static unsigned long *square_sum;

static unsigned short *mode;
static unsigned short *status;

static int montecarlo_usage;

void print_current_status(void) {
    printk(KERN_INFO"K_ERT:                   %hu", *k_ert);
    printk(KERN_INFO"SIGMA_SQRT_T:            %hu", *sigma_sqrt_t);
    printk(KERN_INFO"S_E05_SIGMA_T:           %hu", *s_e05_sigma_t);
    printk(KERN_INFO"M_count (divided by 5):  %u", *M_count);
    printk(KERN_INFO"SUM:                     %lu", *sum);
    printk(KERN_INFO"SQUARE_SUM:              %lu", *square_sum);
    printk(KERN_INFO"MODE:                    %hu", *mode);
    printk(KERN_INFO"STATUS:                  %hu", *status);
}

int montecarlo_open(struct inode *inode, struct file *flip) {
    if (montecarlo_usage !=0) return -EBUSY;

    k_ert = ioremap(K_ERT_ADDRESS, K_ERT_ADDRESS_RANGE);
    sigma_sqrt_t = ioremap(SIGMA_SQRT_T_ADDRESS, SIGMA_SQRT_T_ADDRESS_RANGE);
    s_e05_sigma_t = ioremap(S_E05_SIGMA_T_ADDRESS, S_E05_SIGMA_T_ADDRESS_RANGE);
    M_count = ioremap(M_COUNT_ADDRESS, M_COUNT_ADDRESS_RANGE);

    sum = ioremap(SUM_ADDRESS, SUM_ADDRESS_RANGE);
    square_sum = ioremap(SQUARE_SUM_ADDRESS, SQUARE_SUM_ADDRESS_RANGE);

    mode = ioremap(MODE_ADDRESS, MODE_ADDRESS_RANGE);
    status = ioremap(STATUS_ADDRESS, STATUS_ADDRESS_RANGE);

    if (! check_mem_region((unsigned long)k_ert, K_ERT_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)sigma_sqrt_t, SIGMA_SQRT_T_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)s_e05_sigma_t, S_E05_SIGMA_T_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)M_count, M_COUNT_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)sum, SUM_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)square_sum, SQUARE_SUM_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)mode, MODE_ADDRESS_RANGE) &&
        ! check_mem_region((unsigned long)status, STATUS_ADDRESS_RANGE)) {
        request_mem_region((unsigned long)k_ert, K_ERT_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)sigma_sqrt_t,
                           SIGMA_SQRT_T_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)s_e05_sigma_t,
                           S_E05_SIGMA_T_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)M_count, M_COUNT_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)sum, SUM_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)square_sum, SQUARE_SUM_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)mode, MODE_ADDRESS_RANGE, DRIVER_NAME);
        request_mem_region((unsigned long)status, STATUS_ADDRESS_RANGE, DRIVER_NAME);
    } else {
        printk(KERN_WARNING"Can't get IO Region!");
    }

    montecarlo_usage = 1;
    return 0;
}

int montecarlo_release(struct inode *inode, struct file *flip) {
    iounmap(k_ert);
    iounmap(sigma_sqrt_t);
    iounmap(s_e05_sigma_t);
    iounmap(M_count);
    iounmap(sum);
    iounmap(square_sum);
    iounmap(mode);
    iounmap(status);

    release_mem_region((unsigned long)k_ert, K_ERT_ADDRESS_RANGE);
    release_mem_region((unsigned long)sigma_sqrt_t, SIGMA_SQRT_T_ADDRESS_RANGE);
    release_mem_region((unsigned long)s_e05_sigma_t, S_E05_SIGMA_T_ADDRESS_RANGE);
    release_mem_region((unsigned long)M_count, M_COUNT_ADDRESS_RANGE);
    release_mem_region((unsigned long)sum, SUM_ADDRESS_RANGE);
    release_mem_region((unsigned long)square_sum, SQUARE_SUM_ADDRESS_RANGE);
    release_mem_region((unsigned long)mode, MODE_ADDRESS_RANGE);
    release_mem_region((unsigned long)status, STATUS_ADDRESS_RANGE);

    montecarlo_usage = 0;
    return 0;
}

ssize_t montecarlo_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) {
    int ret;
    char result[256];

    sprintf(result, "%lu|%lu", *sum, *square_sum);
    length = strlen(result);

    ret = copy_to_user(gdata, result, strlen(result));
    if (ret < 0) {
        return -1;
    }

    return length;
}

ssize_t montecarlo_write(struct file *inode,
                         const char *gdata, size_t length, loff_t *off_what) {
    unsigned int ret;
    unsigned int M_original;
    char result[256];

    ret = copy_from_user(result, gdata, 4);
    if (ret < 0) {
        return -1;
    }
    sscanf(result, "%hu|%hu|%hu|%d", k_ert, sigma_sqrt_t, s_e05_sigma_t, &M_original);

    //divided by 5
    *M_count = M_original / 5;

    return length;
}

static int montecarlo_ioctl(struct inode *inode,
                            struct file *flip, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
    case IOCTL_10_STOP:
        *mode = MODE_0_STOP;
        break;
    case IOCTL_11_START:
        *mode = MODE_1_START;
        break;
    case IOCTL_20_STATUS:
        print_current_status();
        break;
    default:
        return -EINVAL;
    }
    print_current_status();
    return 0;
}

struct file_operations montecarlo_fops = {
    .owner   = THIS_MODULE,
    .open    = montecarlo_open,
    .read    = montecarlo_read,
    .write   = montecarlo_write,
    .release = montecarlo_release,
    .ioctl   = montecarlo_ioctl
};

int montecarlo_init(void) {
    int result;

    result = register_chrdev(DRIVER_MAJOR, DRIVER_NAME,  &montecarlo_fops);
    if (result < 0) {
        printk(KERN_WARNING"Can't get any major\n");
        return result;
    }

    printk(KERN_INFO"Init Module, Montecarlo Simulation Number: %d\n", DRIVER_MAJOR);
    return 0;
}

int montecarlo_exit(void) {
    unregister_chrdev(DRIVER_MAJOR, DRIVER_NAME);

    printk("driver: %s DRIVER EXIT\n", DRIVER_NAME);

    return 0;
}

module_init(montecarlo_init);

module_exit(montecarlo_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
