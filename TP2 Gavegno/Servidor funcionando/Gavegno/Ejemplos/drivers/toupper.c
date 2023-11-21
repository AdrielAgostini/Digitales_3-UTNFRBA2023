#define FIRST_MINOR 0
#define NBR_DEV 1
#define MAX_SIZE_STRING 200

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>


int toupper_open (struct inode * inode, struct file * file);
ssize_t toupper_write (struct file * file, const char __user * userbuff,size_t tamano, loff_t* offset);
ssize_t toupper_read (struct file * file, char __user * userbuff, size_t tamano, loff_t * offset);

MODULE_LICENSE("Dual BSD/GPL");

static dev_t mi_dispo;
static struct cdev * p_cdev;
static struct class * pclase;
static struct device * pdevice_sys;

static struct toupper_driver
{
  char *pstring;
  int longitud;
}* ptoupper_driver;

int toupper_open (struct inode * inode, struct file * file)
{
  printk(KERN_ALERT "Openn!!\n");
  return 0;
}

ssize_t toupper_write (struct file * file, const char __user * userbuff,size_t tamano, loff_t* offset)
{
  int i;
  char *ptr;
  if (tamano >= MAX_SIZE_STRING)
  {
    tamano = MAX_SIZE_STRING -1;
  }
  ptoupper_driver->longitud = copy_from_user(ptoupper_driver->pstring, userbuff, tamano);
  ptoupper_driver->longitud = tamano;
  *(ptoupper_driver->pstring + tamano) = 0;
  ptoupper_driver->longitud++; 
  ptr = ptoupper_driver->pstring;
  for (i=0; i<tamano; i++)
  {
    if ( *ptr >= 'a' &&  *ptr <= 'z' )
    {
      *ptr -= 'a' - 'A'; 
    }
    ptr++;
  }
  printk(KERN_ALERT "to_upper: string written = %s, longitud = %d\n", ptoupper_driver->pstring, ptoupper_driver->longitud);
  return tamano;
}

ssize_t toupper_read (struct file * file, char __user * userbuff, size_t tamano, loff_t * offset)
{
  int cant;

  if (tamano < ptoupper_driver->longitud)
  {
    printk(KERN_ALERT "to_upper: me voy con error");
    return -ENOMEM;
  }

  cant = copy_to_user(userbuff, ptoupper_driver->pstring, ptoupper_driver->longitud);
  return cant;
}

static struct file_operations toupper_ops =
{
  .owner = THIS_MODULE,
  .open = toupper_open,
  .read = toupper_read,
  .write = toupper_write
};

static char *toupper_devnode(struct device *dev, umode_t *mode)
{
  if (mode == NULL)
  {
    return NULL;
  }
  *mode = 0666;
  return NULL;
}
static int toupper_init(void)
{
 int i;
//TO DO = Error check
  alloc_chrdev_region(&mi_dispo, FIRST_MINOR, NBR_DEV,"toupper_td3");
  printk(KERN_ALERT "Numero mayor asignado = %d\n", MAJOR(mi_dispo));

//TO DO = Error check
  p_cdev = cdev_alloc();
//unregister_chrdev_region en caso de error

  p_cdev->ops = &toupper_ops;
  p_cdev->owner = THIS_MODULE;
  p_cdev->dev = mi_dispo;

  printk(KERN_ALERT "struct cdev allocada\n");

  cdev_add (p_cdev, mi_dispo, NBR_DEV);

  pclase = class_create (THIS_MODULE, "td3");
  pclase -> devnode = toupper_devnode;
  pdevice_sys = device_create (pclase, NULL,mi_dispo , NULL,"toupper");

  ptoupper_driver = (struct toupper_driver *) kmalloc(sizeof(struct toupper_driver), GFP_KERNEL);

  ptoupper_driver->pstring = (char *) kmalloc(MAX_SIZE_STRING, GFP_KERNEL);
  for (i=0; i<MAX_SIZE_STRING; i++)
  {
    *(ptoupper_driver->pstring + i)=0;
  }
  ptoupper_driver->longitud=0;
  return 0;
}

static void toupper_exit(void)
{
  kfree(ptoupper_driver->pstring);
  kfree(ptoupper_driver);
  device_destroy(pclase, mi_dispo);
  class_destroy(pclase);
  cdev_del(p_cdev);
  unregister_chrdev_region(mi_dispo, NBR_DEV);
}

module_init(toupper_init);
module_exit(toupper_exit);
