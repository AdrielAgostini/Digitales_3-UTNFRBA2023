
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h> 			            // agregar en /dev
#include <linux/device.h> 			            // agregar en /dev
#include <linux/cdev.h>                         // Char device: File operation struct,
#include <linux/fs.h>                           // Header for the Linux file system support (alloc_chrdev_region y unregister_chrdev_region)
#include <linux/module.h>                       // Core header for loading LKMs into the kernel
#include <linux/of_address.h>                   // of_iomap
#include <linux/platform_device.h>              // platform_device
#include <linux/of.h>                           // of_match_ptr
#include <linux/io.h>                           // ioremap
#include <linux/interrupt.h>                    // request_irq
#include <linux/delay.h>                        // msleep, udelay y ndelay
#include <linux/uaccess.h>                      // copy_to_user - copy_from_user
#include <linux/types.h>                        // typedefs varios
#include <linux/slab.h>				            // kmalloc
#include <linux/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/*-------------DEFINE-------------*/
#define FIRST_MINOR 0
#define CANT_DISP 1
#define BUF_SIZE 20

#define ID    	        "SG"
#define CLASS_NAME      "I2Cdriver_class"
#define COMPATIBLE      "I2Cdriver"
#define NOMBRE			COMPATIBLE

#define Tx_MODO_LECTURA 0
#define Tx_MODO_ESCRITURA 1

static struct {
	dev_t I2C_MPU6050;

	struct cdev *I2C_MPU6050_cdev;
	struct class *I2C_MPU6050_clase;
    struct device *I2C_MPU6050_device;
}dev;

/*------------------Prototipos------------------*/
int I2C_MPU6050_open(struct inode *inode, struct file *file);
int I2C_MPU6050_release(struct inode *inode, struct file *file);
ssize_t I2C_MPU6050_write(struct file *file, const char __user *userbuff, size_t tamano, loff_t *offset);
ssize_t I2C_MPU6050_read(struct file *file, char __user *userbuff, size_t tamano, loff_t *offset);
irqreturn_t i2c_irq_handler(int irq, void *dev_id, struct pt_regs *regs);

int I2C_MPU6050_probe(struct platform_device *i2c_pd);
int I2C_MPU6050_remove(struct platform_device *i2c_pd);
static int __init I2C_MPU6050_init(void);
static void __exit I2C_MPU6050_exit(void);

void initMPU6050(void);
void MPU6050_writebyte(uint8_t registro, uint8_t data, uint8_t modo);
int8_t MPU6050_readbyte(void);

int8_t MPU6050_readFIFO(uint16_t cant);

static int I2C_MPU6050_devnode(struct device *dev, struct kobj_uevent_env *env);

struct file_operations I2C_MPU6050_ops =
{
        .owner = THIS_MODULE,
        .open = I2C_MPU6050_open,
        .release = I2C_MPU6050_release,
        .read = I2C_MPU6050_read,
        .write = I2C_MPU6050_write
};


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Gavegno Sebastian");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("I2C DRIVER PARA MPU6050");