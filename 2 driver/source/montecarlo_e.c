#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
//#include <linux/types.h>
//#include <linux/fcntl.h>
#include <linux/ioport.h>
#include <linux/delay.h>

//#include <asm/fcntl.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define DRIVER_AUTHOR "TEAM5"
#define DRIVER_DESC "Montecarlo Simulation"

#define MONTECARLO_MAJOR 230
#define MONTECARLO_NAME "MONTECARLO"
#define MONTECARLO_MODULE_VERSION "MONTECARLO SIMULATOR v0.1"

#define MONTECARLO_ADDRESS 0x88000100
#define MONTECARLO_RANGE 0x2000


#define MODE_0_STOP 0
#define MODE_1_START 1

#define STATUS_0_WORKING 0
#define STATUS_1_FINISHED 1

#define IOCTL_10_STOP 0x10
#define IOCTL_11_START 0x11

static void* montecarlo_region;

static unsigned short *k_ert;
static unsigned short *sigma_sqrt_t;
static unsigned short *s_e05_sigma_t;
static unsigned int *M_count;


static unsigned long *sum;
static unsigned long *square_sum;

static unsigned short *mode;
static unsigned short *status;

static unsigned int *current_count;

static int montecarlo_usage;

void printk_current_status(void) {
    printk(KERN_INFO"K_ERT:                   %hu\n", *k_ert);
    printk(KERN_INFO"SIGMA_SQRT_T:            %hu\n", *sigma_sqrt_t);
    printk(KERN_INFO"S_E05_SIGMA_T:           %hu\n", *s_e05_sigma_t);
    printk(KERN_INFO"M_count (divided by 5):  %u\n", *M_count);
    printk(KERN_INFO"SUM:                     %lu\n", *sum);
    printk(KERN_INFO"SQUARE_SUM:              %lu\n", *square_sum);
    printk(KERN_INFO"MODE:                    %hu\n", *mode);
    printk(KERN_INFO"STATUS:                  %hu\n", *status);
	printk(KERN_INFO"CURRENT_COUNT:           %u\n", *current_count);
}

int montecarlo_open(struct inode *inode, struct file *flip) {
    if (montecarlo_usage !=0) return -EBUSY;

	montecarlo_region = ioremap(MONTECARLO_ADDRESS, MONTECARLO_RANGE);
	if (!check_mem_region((unsigned long)montecarlo_region,
				MONTECARLO_RANGE)) {
		request_region((unsigned long)montecarlo_region, MONTECARLO_RANGE, MONTECARLO_NAME);
	}

    k_ert = (unsigned short*) (montecarlo_region);
    sigma_sqrt_t = (unsigned short *)(montecarlo_region + 0x2);
    s_e05_sigma_t = (unsigned short *)(montecarlo_region + 0x4);
    M_count = (unsigned int *)(montecarlo_region + 0x6);
    mode = (unsigned short *)(montecarlo_region + 0xA);

    sum = (unsigned long *)(montecarlo_region + 0xE);
    square_sum = (unsigned long *)(montecarlo_region + 0x16);
    status = (unsigned short *)(montecarlo_region + 0x1E);

	current_count = (unsigned int *)(montecarlo_region + 0x20);

	printk(KERN_INFO"Open");
	printk_current_status();
    montecarlo_usage = 1;
    return 0;
}

int montecarlo_release(struct inode *inode, struct file *flip) {
	iounmap((void *)(montecarlo_region));
	release_region((unsigned long)montecarlo_region, MONTECARLO_RANGE);


    montecarlo_usage = 0;
	printk(KERN_INFO"Release");
	printk_current_status();
    return 0;
}

ssize_t montecarlo_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) {
    int ret;
    char result[256];
    printk(KERN_INFO"Read Started\n");

    sprintf(result, "%lu|%lu|%hu", *sum, *square_sum, *status);

	
    ret = copy_to_user(gdata, result, strlen(result));
    if (ret < 0) {
        printk(KERN_WARNING"Read Failed\n");
        return -1;
    }
    printk(KERN_INFO"Read Finished\n");
    printk_current_status();

    return length;
}

ssize_t montecarlo_write(struct file *inode,
                         const char *gdata, size_t length, loff_t *off_what) {
    unsigned int ret;
    unsigned int M_original;
	unsigned short *M_count_up = (unsigned short *)((void *)M_count + 2);
	unsigned short *M_count_down = (unsigned short *)(M_count);

   	unsigned short k_ert_original, sigma_sqrt_t_original, s_e05_sigma_t_original;
    char result[length];

    printk(KERN_INFO"Write Started\n");
    ret = copy_from_user(result, gdata, length);
    if (ret < 0) {
        printk(KERN_WARNING"Write Failed\n");
        return -1;
    }
    printk("Data: %s", result);
    sscanf(result, "%hu|%hu|%hu|%u", k_ert, sigma_sqrt_t, s_e05_sigma_t,
			&M_original);
	
	M_original = M_original / 5;
	*M_count_up = (unsigned short)(M_original >> 16);
	*M_count_down = (unsigned short)(M_original);
    printk(KERN_INFO"Write Finished\n");
    printk_current_status();

    return length;
}

static int montecarlo_ioctl(struct inode *inode,
                            struct file *flip, unsigned int cmd, unsigned long arg) {
    printk(KERN_INFO"IOCTL Started: %u\n", cmd);
    switch (cmd) {
    case IOCTL_10_STOP:
        *mode = MODE_0_STOP;
        break;
    case IOCTL_11_START:
        *mode = MODE_1_START;
        break;
    default:
        printk(KERN_WARNING"IOCTL Failed\n");
        return -EINVAL;
    }
    printk(KERN_INFO"IOCTL Finished\n");
    printk_current_status();
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



    result = register_chrdev(MONTECARLO_MAJOR, MONTECARLO_NAME, &montecarlo_fops);
    if (result < 0) {
        printk(KERN_WARNING"Can't get any major\n");
        return result;
    }

    printk(KERN_INFO"Init Module, Montecarlo Simulation Number: %d\n", MONTECARLO_MAJOR);
    return 0;
}

void montecarlo_exit(void) {
    unregister_chrdev(MONTECARLO_MAJOR, MONTECARLO_NAME);
    printk("driver: %s DRIVER EXIT\n", MONTECARLO_NAME);
}

module_init(montecarlo_init);

module_exit(montecarlo_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
