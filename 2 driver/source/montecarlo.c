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

#define KERT_ADDRESS 0x88000100
#define KERT_RANGE 0x2

#define SIGMA_SQRT_T_ADDRESS 0x88000102
#define SIGMA_SQRT_T_RANGE 0x2

#define S_E05_SIGMA_T_ADDRESS 0x88000104
#define S_E05_SIGMA_T_RANGE 0x2

#define M_COUNT_ADDRESS 0x88000106
#define M_COUNT_RANGE 0x4

#define SUM_ADDRESS 0x8800010E
#define SUM_RANGE 0x8

#define SQUARE_SUM_ADDRESS 0x88000116
#define SQUARE_SUM_RANGE 0x8

#define MODE_ADDRESS 0x8800010a
#define MODE_RANGE 0x2

#define STATUS_ADDRESS 0x8800011e
#define STATUS_RANGE 0x2

#define CURRENT_COUNT_ADDRESS 0x88000120
#define CURRENT_COUNT_RANGE 0x4

#define MODE_0_STOP 0
#define MODE_1_START 1

#define STATUS_0_WORKING 0
#define STATUS_1_FINISHED 1

#define IOCTL_10_STOP 0x10
#define IOCTL_11_START 0x11

static unsigned short *k_ert;
static unsigned short *sigma_sqrt_t;
static unsigned short *s_e05_sigma_t;
static unsigned int *m_count;


static unsigned long long *sum;
static unsigned long long *square_sum;

static unsigned short *mode;
static unsigned short *status;

static unsigned int *current_count;

static int montecarlo_usage;

void printk_current_status(void) {
    printk(KERN_INFO"K_ERT:                   %hu\n", *k_ert);
    printk(KERN_INFO"SIGMA_SQRT_T:            %hu\n", *sigma_sqrt_t);
    printk(KERN_INFO"S_E05_SIGMA_T:           %hu\n", *s_e05_sigma_t);
    printk(KERN_INFO"M_count (divided by 5):  %u\n", *m_count);
    printk(KERN_INFO"SUM:                     %llu\n", *sum);
    printk(KERN_INFO"SQUARE_SUM:              %llu\n", *square_sum);
    printk(KERN_INFO"MODE:                    %hu\n", *mode);
    printk(KERN_INFO"STATUS:                  %hu\n", *status);
	printk(KERN_INFO"CURRENT_COUNT:           %u\n", *current_count);
}

int montecarlo_open(struct inode *inode, struct file *flip) {
    if (montecarlo_usage !=0) return -EBUSY;
	
	k_ert = ioremap(KERT_ADDRESS, KERT_RANGE);
	sigma_sqrt_t = ioremap(SIGMA_SQRT_T_ADDRESS, SIGMA_SQRT_T_RANGE);
	s_e05_sigma_t = ioremap(S_E05_SIGMA_T_ADDRESS, S_E05_SIGMA_T_RANGE);
	m_count = ioremap(M_COUNT_ADDRESS, M_COUNT_RANGE); 
	sum = ioremap(SUM_ADDRESS, SUM_RANGE); 
	square_sum = ioremap(SQUARE_SUM_ADDRESS, SQUARE_SUM_RANGE); 
	mode = ioremap(MODE_ADDRESS, MODE_RANGE); 
	status = ioremap(STATUS_ADDRESS, STATUS_RANGE);
	current_count = ioremap(CURRENT_COUNT_ADDRESS, CURRENT_COUNT_RANGE); 
	if (!check_mem_region((unsigned long)k_ert,KERT_RANGE)
	&& !check_mem_region((unsigned long)sigma_sqrt_t,SIGMA_SQRT_T_RANGE)
	&& !check_mem_region((unsigned long)s_e05_sigma_t,S_E05_SIGMA_T_RANGE)
	&& !check_mem_region((unsigned long)m_count,M_COUNT_RANGE)
	&& !check_mem_region((unsigned long)sum,SUM_RANGE)
	&& !check_mem_region((unsigned long)square_sum,SQUARE_SUM_RANGE)
	&& !check_mem_region((unsigned long)mode,MODE_RANGE)
	&& !check_mem_region((unsigned long)status,STATUS_RANGE)
	&& !check_mem_region((unsigned long)current_count,CURRENT_COUNT_RANGE))
	{
		request_mem_region((unsigned long)k_ert, KERT_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)sigma_sqrt_t,SIGMA_SQRT_T_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)s_e05_sigma_t,S_E05_SIGMA_T_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)m_count,M_COUNT_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)sum,SUM_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)square_sum,SQUARE_SUM_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)mode,MODE_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)status,STATUS_RANGE,MONTECARLO_NAME);
		request_mem_region((unsigned long)current_count,CURRENT_COUNT_RANGE,MONTECARLO_NAME);	
	}
	printk(KERN_INFO"Open");
    montecarlo_usage = 1;
    return 0;
}

int montecarlo_release(struct inode *inode, struct file *flip) {
	iounmap(k_ert);
	iounmap(sigma_sqrt_t);
	iounmap(s_e05_sigma_t);
	iounmap(m_count);
	iounmap(sum);
	iounmap(square_sum);
	iounmap(mode);
	iounmap(status);
	iounmap(current_count);

	release_mem_region((unsigned long)k_ert,KERT_RANGE);
	release_mem_region((unsigned long)sigma_sqrt_t,SIGMA_SQRT_T_RANGE);
	release_mem_region((unsigned long)s_e05_sigma_t,S_E05_SIGMA_T_RANGE);
	release_mem_region((unsigned long)m_count,M_COUNT_RANGE);
	release_mem_region((unsigned long)sum,SUM_RANGE);
	release_mem_region((unsigned long)square_sum,SQUARE_SUM_RANGE);
	release_mem_region((unsigned long)mode,MODE_RANGE);
	release_mem_region((unsigned long)status,STATUS_RANGE);
	release_mem_region((unsigned long)current_count,CURRENT_COUNT_RANGE);	
    
	montecarlo_usage = 0;
	printk(KERN_INFO"Release");
    return 0;
}

ssize_t montecarlo_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) {
    int ret;
    char result[256];
    printk(KERN_INFO"Read Started\n");

    sprintf(result, "%llu|%llu|%hu", *sum, *square_sum, *status);

	
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
    unsigned int m_original;
	unsigned short *m_count_up = (unsigned short *)((void *)m_count + 2);
	unsigned short *m_count_down = (unsigned short *)((void *)m_count);
    char result[length];

    printk(KERN_INFO"Write Started\n");
    ret = copy_from_user(result, gdata, length);
    if (ret < 0) {
        printk(KERN_WARNING"Write Failed\n");
        return -1;
    }
    printk("Data: %s", result);
    sscanf(result, "%hu|%hu|%hu|%u", k_ert, sigma_sqrt_t, s_e05_sigma_t,
			&m_original);
	
	m_original = m_original / 5;
	*m_count_up = (unsigned short)(m_original >> 16);
	*m_count_down = (unsigned short)(m_original);

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
