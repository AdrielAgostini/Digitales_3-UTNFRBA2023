//#include <linux/init.h>
//#include <linux/kernel.h>
//#include <linux/module.h>
//#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
//#include <linux/uaccess.h>
//#include <linux/pm.h>
//#include <linux/input-polldev.h>
//#include <linux/dcache.h>
//#include <linux/device.h>
//#include <linux/pm_runtime.h>
//#include <linux/debugfs.h>
//#include <linux/delay.h>
//#include <linux/types.h>
//#include <linux/kdev_t.h>
//#include <linux/fs.h>
//#include <linux/cdev.h>
//
//#include <stdio.h>
//#include <stdlib.h>
//
//#define FIRST_MINOR 0
//#define NBR_DEV 1
//#define BUF_SIZE 20
//
////device specific structure//
//struct I2C_MPU6050_dev
//{
//    struct i2c_client *client;
//    struct cdev cdev;
//    struct kobject *kobj_conf_dir;
//};
//
//static dev_t I2C_MPU6050;
////static struct cdev *I2C_MPU6050_cdev;
//static struct class *I2C_MPU6050_clase;
//static struct device *I2C_MPU6050_device;
//
//char kbuffer[BUF_SIZE]; //KERNEL buffer
//
///*
//
//static struct I2C_MPU6050_driver
//{
//    uint16_t accel_xout;
//    uint16_t accel_yout;
//    uint16_t accel_zout;
//    int16_t temp_out;
//    uint16_t gyro_xout;
//    uint16_t gyro_yout;
//    uint16_t gyro_zout;
//} * I2C_MPU6050_driver;
//
//static struct I2C_MPU6050_dev
//{
//    struct class *clase;
//    struct device *device;
//    struct cdev cdev;
//    dev_t devtype;
//    struct i2c_client *client;
//    unsigned int freq;
//    void __iomem *base;
//    int irq;
//} I2C_MPU6050aux;
//*/
//
//MODULE_LICENSE("Dual BSD/GPL");
//MODULE_AUTHOR("Gavegno Sebastian");
//MODULE_DESCRIPTION("I2C DRIVER PARA MPU6050");
//
//struct file_operations I2C_MPU6050_ops =
//    {
//        .owner = THIS_MODULE,
//        .open = I2C_MPU6050_open,
//        .release = I2C_MPU6050_release,
//        .read = I2C_MPU6050_read,
//        .write = I2C_MPU6050_write};
//
//int I2C_MPU6050_open(struct inode *inode, struct file *file)
//{
//
//    printk(KERN_ALERT "Open!!\n");
//    return 0;
//}
//
//int I2C_MPU6050_release(struct inode *inode, struct file *file)
//{
//
//    printk(KERN_ALERT "Close!!\n");
//    return 0;
//}
//
//ssize_t I2C_MPU6050_write(struct file *file, const char __user *userbuff, size_t tamano, loff_t *offset)
//{
//    return 0;
//}
//
//ssize_t I2C_MPU6050_read(struct file *file, char __user *userbuff, size_t tamano, loff_t *offset)
//{
//
//    return 0;
//}
//
///*
//* Función para verificar que el dispositivo esté presente dentro del platform-bus, se reservan los recursos de hardware
//* del sistema que sean necesarios (Memoria de kernel, IRQ line, handler de interrupción, …) 
//* y se configura el platform device para el funcionamiento del mismo
//*/
//int I2C_MPU6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
//{
//
//}
//
///*
//* Función para remover un dispositivo dentro del platform-bus
//*/
//int I2C_MPU6050_remove(struct i2c_client *client)
//{
// 
//    return 0;
//}
//
////driver functions
//static struct i2c_driver I2C_MPU6050_driver = {
//    .driver = {
//        .name = "I2C_MPU6050",
//    },
//    .probe = I2C_MPU6050_probe,
//    .remove = I2C_MPU6050_remove,
//    .id_table = MPU6050_id,
//
//};
//
//static char *I2C_MPU6050_devnode(struct device *dev, umode_t *mode)
//{
//    if (mode == NULL)
//    {
//        return NULL;
//    }
//    *mode = 0666;
//    return NULL;
//}
//
///*
//    intalacion del driver
//comandos:
//    sudo insmod driver-i2c.ko
//    lsmod | grep driver-i2c  --> me dice los modulos que tengo instalados. con grep filtro 
//*/
//
//static int I2C_MPU6050_init(void)
//{
//    int i;
//
//    if (alloc_chrdev_region(&I2C_MPU6050, FIRST_MINOR, NBR_DEV, "I2C_MPU6050") < 0)
//    {
//        exit(1);
//    }
//
//    printk(KERN_ALERT "Numero mayor asignado = %d\n", MAJOR(I2C_MPU6050));
//    I2C_MPU6050_cdev = cdev_alloc(); //Reservar memoria para la estructura cdev del kernel
//    if (I2C_MPU6050_cdev < 0)
//    {
//        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
//        exit(1);
//    }
//    I2C_MPU6050_cdev->ops = &I2C_MPU6050_ops; //Asociar las operaciones del dispositivo con la estructura cdev
//    I2C_MPU6050_cdev->owner = THIS_MODULE;    //Registrar el dueño del dispositivo en la estructura cdev
//    I2C_MPU6050_cdev->dev = I2C_MPU6050;      //
//
//    printk(KERN_ALERT "struct cdev allocada\n");
//    //Asociar la estructura cdev para el dispositivo y el número de dispositivo
//    if (cdev_add(I2C_MPU6050_cdev, I2C_MPU6050, NBR_DEV) < 0)
//    {
//        cdev_del(I2C_MPU6050_cdev);
//        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
//        exit(1);
//    }
//
//    //otorga permisos al nodo
//    I2C_MPU6050_clase = class_create(THIS_MODULE, "I2C_MPU6050"); //Creación del tipo de dispositivo y del I-nodo en el espacio de usuario
//    if (I2C_MPU6050_clase < 0)
//    {
//        cdev_del(I2C_MPU6050_cdev);
//        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
//        exit(1);
//    }
//
//    I2C_MPU6050_clase->devnode = I2C_MPU6050_devnode;
//
//    I2C_MPU6050_device = device_create(I2C_MPU6050_clase, NULL, I2C_MPU6050, NULL, "I2C_MPU6050");
//    if (I2C_MPU6050_device < 0)
//    {
//        class_destroy(I2C_MPU6050_clase);
//        cdev_del(I2C_MPU6050_cdev);
//        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
//        exit(1);
//    }
//
//    I2C_MPU6050_driver = (struct I2C_MPU6050_driver *)kmalloc(sizeof(struct I2C_MPU6050_driver), GFP_KERNEL);
//    if (I2C_MPU6050_driver == NULL)
//    {
//        device_destroy(I2C_MPU6050_clase, I2C_MPU6050);
//        class_destroy(I2C_MPU6050_clase);
//        cdev_del(I2C_MPU6050_cdev);
//        unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
//        exit(1);
//    }
//
//    return 0;
//}
//
///*
//    desintalacion del driver
//    sudo rmmod driver-i2c
//*/
//static void I2C_MPU6050_exit(void)
//{
//    kfree(I2C_MPU6050_driver);
//    device_destroy(I2C_MPU6050_clase, I2C_MPU6050);
//    class_destroy(I2C_MPU6050_clase);
//    cdev_del(I2C_MPU6050_cdev);
//    unregister_chrdev_region(I2C_MPU6050, NBR_DEV);
//}
//
//module_init(I2C_MPU6050_init);
//module_exit(I2C_MPU6050_exit);
//
//
//
//
//
//
//