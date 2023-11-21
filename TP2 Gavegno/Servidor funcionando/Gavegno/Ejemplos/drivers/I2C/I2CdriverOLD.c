#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/uaccess.h>
#include <linux/pm.h>
#include <linux/input-polldev.h>
#include <linux/dcache.h>
#include <linux/device.h>
#include <linux/pm_runtime.h>
#include <linux/debugfs.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include <i2c/smbus.h>

#include <stdio.h>
#include <stdlib.h>

#define FIRST_MINOR 0
#define NBR_DEV 1
#define BUF_SIZE 20

//https://linuxtv.org/downloads/v4l-dvb-internals/device-drivers/i2c.html

//device specific structure//
struct I2C_MPU6050_dev
{
    struct i2c_client *client;
    struct cdev cdev;
    struct kobject *kobj_conf_dir;
};

static dev_t I2C_MPU6050;
//static struct cdev *I2C_MPU6050_cdev;
static struct class *I2C_MPU6050_clase;
static struct device *I2C_MPU6050_device;

char kbuffer[BUF_SIZE]; //KERNEL buffer

/*

static struct I2C_MPU6050_driver
{
    uint16_t accel_xout;
    uint16_t accel_yout;
    uint16_t accel_zout;
    int16_t temp_out;
    uint16_t gyro_xout;
    uint16_t gyro_yout;
    uint16_t gyro_zout;
} * I2C_MPU6050_driver;

static struct I2C_MPU6050_dev
{
    struct class *clase;
    struct device *device;
    struct cdev cdev;
    dev_t devtype;
    struct i2c_client *client;
    unsigned int freq;
    void __iomem *base;
    int irq;
} I2C_MPU6050aux;
*/

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Gavegno Sebastian");
MODULE_DESCRIPTION("I2C DRIVER PARA MPU6050");

struct file_operations I2C_MPU6050_ops =
    {
        .owner = THIS_MODULE,
        .open = I2C_MPU6050_open,
        .release = I2C_MPU6050_release,
        .read = I2C_MPU6050_read,
        .write = I2C_MPU6050_write};

int I2C_MPU6050_open(struct inode *inode, struct file *file)
{
    struct I2C_MPU6050_dev *dev = filep->private_data = container_of(inode->i_cdev, struct I2C_MPU6050_dev, cdev);
    struct i2c_client *client = dev->client;
    printk("Funcion open\n");
    printk(KERN_ALERT "Open!!\n");
    return 0;
}

int I2C_MPU6050_release(struct inode *inode, struct file *file)
{
    printk("Cerrar funcion called\n");
    struct I2C_MPU6050_dev *dev = filep->private_data;
    struct i2c_client *client = dev->client;
    dev_info(&client->dev, "Cerrar funcion called");
    return 0;
}

ssize_t I2C_MPU6050_write(struct file *file, const char __user *userbuff, size_t tamano, loff_t *offset)
{
    return 0;
}

ssize_t I2C_MPU6050_read(struct file *file, char __user *userbuff, size_t tamano, loff_t *offset)
{
    int j = 0, addr;
    int command = 0x3B; //reading from ACCEL_XOUT_H

    printk("In read function\n");

    struct I2C_MPU6050_dev *dev = filep->private_data;
    struct i2c_client *client = dev->client;

    i2c_smbus_write_byte_data(client, 0x6B, 0x00); //configuring PWR_MGMT_1 register

    addr = i2c_smbus_read_byte_data(client, 0x75); //reading WHO_AM_I register
    printk("who am I register slave address=%x\n", addr);
    memset(kbuffer, '\0', BUF_SIZE);
    while (j < 15) //usar tamaño
    {
        kbuffer[j] = i2c_smbus_read_byte_data(client, command++); //SMBUS read command
        printk("data at %d-%d\n", j, kbuffer[j]);
        j++;
    }

    if (copy_to_user(userbuff, kbuffer, BUF_SIZE)) //copying data to user
        return -EFAULT;

    return 0;
}

/*  ID table */
static struct i2c_device_id MPU6050_id[] = {
    {"MPU6050", 0}};

/*
* Función para verificar que el dispositivo esté presente dentro del platform-bus, se reservan los recursos de hardware
* del sistema que sean necesarios (Memoria de kernel, IRQ line, handler de interrupción, …) 
* y se configura el platform device para el funcionamiento del mismo
*/
int I2C_MPU6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int retval;
    struct I2C_MPU6050_dev *dev;
    //int command=0x3B;

    printk("In probe function\n");
    dev_info(&client->dev, "MPU6050 device probed");

    dev = devm_kzalloc(&client->dev, sizeof(*dev), GFP_KERNEL); //allocating memory for struct dev
    if (!dev)
    {
        dev_err(&client->dev, "Failed to allocate memory for private data\n");
        return -ENOMEM;
    }

    dev_info(&client->dev, "Allocated memory for private data\n");

    i2c_set_clientdata(client, dev);
    dev->client = client;

    cdev_init(&dev->cdev, &I2C_MPU6050_ops);

    retval = cdev_add(&dev->cdev, I2C_MPU6050, 1);
    if (retval)
    {
        dev_err(&client->dev, "Falla Cdev_Add");
        return retval;
    }

    dev_info(&client->dev, "Added cdev Structure\n");

    I2C_MPU6050_device = device_create(I2C_MPU6050_clase, NULL, I2C_MPU6050, NULL, "I2C_MPU6050"); //creating a device
    if (I2C_MPU6050_device == NULL)
    {
        dev_err(&client->dev, "Falla al crear ddvice");
        cdev_del(&dev->cdev);
        return retval;
    }

    dev_info(&client->dev, "Device creado\n");

    printk("WHO M I=%x\n", i2c_smbus_read_byte_data(client, 0x75)); //Slave address=0x68

    i2c_smbus_write_byte_data(client, 0x6B, 0x02);
    printk("power management register=%d\n", i2c_smbus_read_byte_data(client, 0x6B));
    i2c_smbus_write_byte_data(client, 0x19, 0x07);
    printk("SMPLRT_DIV register=%d\n", i2c_smbus_read_byte_data(client, 0x19));
    i2c_smbus_write_byte_data(client, 0x1B, 0x08);
    printk("GYRO_CONFIG register=%d\n", i2c_smbus_read_byte_data(client, 0x1B));
    i2c_smbus_write_byte_data(client, 0x1C, 0x00);
    printk("ACCEL_CONFIG register=%d\n", i2c_smbus_read_byte_data(client, 0x1C));

    return 0;
}

//remove function
/*
* Función para remover un dispositivo dentro del platform-bus
*/
int I2C_MPU6050_remove(struct i2c_client *client)
{
    struct I2C_MPU6050_dev *dev;

    dev = i2c_get_clientdata(client);
    dev_info(&client->dev, "Remove function called\n");

    device_destroy(I2C_MPU6050_clase, I2C_MPU6050);
    dev_info(&client->dev, "Device destroyed\n");

    cdev_del(&dev->cdev);
    dev_info(&client->dev, "Cdev deleted\n");
    dev_info(&client->dev, "I2C_MPU6050 device removed");
    return 0;
}

//driver functions
static struct i2c_driver I2C_MPU6050_driver = {
    .driver = {
        .name = "I2C_MPU6050",
    },
    .probe = I2C_MPU6050_probe,
    .remove = I2C_MPU6050_remove,
    .id_table = MPU6050_id,

};

static char *I2C_MPU6050_devnode(struct device *dev, umode_t *mode)
{
    if (mode == NULL)
    {
        return NULL;
    }
    *mode = 0666;
    return NULL;
}

/*
    intalacion del driver
comandos:
    sudo insmod driver-i2c.ko
    lsmod | grep driver-i2c  --> me dice los modulos que tengo instalados. con grep filtro 
*/
static int I2C_MPU6050_init(void)
{
    int ret;
    printk("Init Fuction\n");

    alloc_chrdev_region(&I2C_MPU6050, 0, 1, "I2C_MPU6050");

    I2C_MPU6050_clase = class_create(THIS_MODULE, "I2C_MPU6050");
    if (I2C_MPU6050_clase < 0)
    {
        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
        exit(1);
    }
    ret = i2c_add_driver(&I2C_MPU6050_driver);
    if (ret)
    {
        printk("Failed to add the i2c driver\n");
        class_destroy(I2C_MPU6050_clase);
        unregister_chrdev_region(I2C_MPU6050, 1);
        return ret;
    }
    printk("I2C driver added\n");
    return 0;
}

/*
    desintalacion del driver
    sudo rmmod driver-i2c
*/
static void I2C_MPU6050_exit(void)
{
    printk("Exit\n");
    i2c_del_driver(&I2C_MPU6050_driver);
    printk("I2C driver deleted\n");
    class_destroy(I2C_MPU6050_clase);
    printk("Class destroyed\n");
    unregister_chrdev_region(I2C_MPU6050, 1);
    printk("Unregistered Char Device\n");
}

module_init(I2C_MPU6050_init);
module_exit(I2C_MPU6050_exit);

/*
static int I2C_MPU6050_init(void)
{
    int i;

    if (alloc_chrdev_region(&I2C_MPU6050, FIRST_MINOR, NBR_DEV, "I2C_MPU6050") < 0)
    {

        exit(1);
    }

    printk(KERN_ALERT "Numero mayor asignado = %d\n", MAJOR(I2C_MPU6050));
    I2C_MPU6050_cdev = cdev_alloc(); //Reservar memoria para la estructura cdev del kernel
    if (I2C_MPU6050_cdev < 0)
    {
        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
        exit(1);
    }
    I2C_MPU6050_cdev->ops = &I2C_MPU6050_ops; //Asociar las operaciones del dispositivo con la estructura cdev
    I2C_MPU6050_cdev->owner = THIS_MODULE;    //Registrar el dueño del dispositivo en la estructura cdev
    I2C_MPU6050_cdev->dev = I2C_MPU6050;      //

    printk(KERN_ALERT "struct cdev allocada\n");
    //Asociar la estructura cdev para el dispositivo y el número de dispositivo
    if (cdev_add(I2C_MPU6050_cdev, I2C_MPU6050, NBR_DEV) < 0)
    {
        cdev_del(I2C_MPU6050_cdev);
        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
        exit(1);
    }

    //otorga permisos al nodo
    I2C_MPU6050_clase = class_create(THIS_MODULE, "I2C_MPU6050"); //Creación del tipo de dispositivo y del I-nodo en el espacio de usuario
    if (I2C_MPU6050_clase < 0)
    {
        cdev_del(I2C_MPU6050_cdev);
        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
        exit(1);
    }

    I2C_MPU6050_clase->devnode = I2C_MPU6050_devnode;

    I2C_MPU6050_device = device_create(I2C_MPU6050_clase, NULL, I2C_MPU6050, NULL, "I2C_MPU6050");
    if (I2C_MPU6050_device < 0)
    {
        class_destroy(I2C_MPU6050_clase);
        cdev_del(I2C_MPU6050_cdev);
        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
        exit(1);
    }

    I2C_MPU6050_driver = (struct I2C_MPU6050_driver *)kmalloc(sizeof(struct I2C_MPU6050_driver), GFP_KERNEL);
    if (I2C_MPU6050_driver == NULL)
    {
        device_destroy(I2C_MPU6050_clase, I2C_MPU6050);
        class_destroy(I2C_MPU6050_clase);
        cdev_del(I2C_MPU6050_cdev);
        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
        exit(1);
    }

    return 0;
}

static void I2C_MPU6050_exit(void)
{
    kfree(I2C_MPU6050_driver);
    device_destroy(I2C_MPU6050_clase, I2C_MPU6050);
    class_destroy(I2C_MPU6050_clase);
    cdev_del(I2C_MPU6050_cdev);
    unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
}


*/