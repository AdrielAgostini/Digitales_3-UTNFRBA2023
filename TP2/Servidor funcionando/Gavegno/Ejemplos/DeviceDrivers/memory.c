// Hecho por Dario Alpern
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
//#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/cdev.h>

MODULE_LICENSE("Dual BSD/GPL");
/* Declaracion de funciones */
int memory_open(struct inode *inode, struct file *filp);
int memory_release(struct inode *inode, struct file *filp);
ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
ssize_t memory_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static void memory_exit(void);
static int memory_init(void);
/* Estructura que declara las funciones que se llaman */
/* en el driver cuando se lo opera como archivo */
struct file_operations memory_fops =
{
  read: memory_read,
  write: memory_write,
  open: memory_open,
  release: memory_release
};
/* Declaracion de funciones de inicio y fin */
module_init(memory_init);
module_exit(memory_exit);
/* Variables globales */
/* Numero mayor */
int memory_major = 60;
/* Buffer para guardar datos */
char *memory_buffer;
static struct cdev memory_cdev;

static int memory_init(void)
{
  dev_t dev;
  int result;
  /* Registrar el device driver */
  dev = MKDEV(memory_major, 0);
  cdev_init(&memory_cdev, &memory_fops);
  memory_cdev.owner = THIS_MODULE;
  memory_cdev.ops = &memory_fops;
  cdev_add(&memory_cdev, dev, 1);
  if (register_chrdev_region(dev, 1, "letras"))
  {
    printk(KERN_ALERT "Houston, we have a problem...\n");
  }
  /* Obtener memoria de kernel para el buffer */
  memory_buffer = kmalloc(1, GFP_KERNEL);
  if (!memory_buffer)
  {
    result = -ENOMEM;
    goto fail;
  }
  memset(memory_buffer, 0, 1);
  printk(KERN_ALERT "memory: Driver instalado.\n");
  return 0;
fail:
  memory_exit();
  return result;
}

static void memory_exit( void )
{
  cdev_del(&memory_cdev);
  unregister_chrdev_region(MKDEV(memory_major, 0), 1);
  printk(KERN_ALERT "memory: Driver desinstalado.\n");
}

int memory_open(struct inode *inode, struct file *filp)
{
  /* Apertura OK */
  return 0;
}

int memory_release(struct inode *inode, struct file *filp)
{
  /* Cierre OK */
  return 0;
}

ssize_t memory_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
/* Transferencia de datos a espacio de usuario */
  copy_to_user(buf,memory_buffer,1);
/* Cambiar puntero de lectura */
  if (*f_pos == 0)
  {
    *f_pos+=1;
    return 1;
  }
  else
  {
    return 0;
  }
}

ssize_t memory_write( struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
  const char *tmp;
  tmp=buf+count-1;
/* Transferencia de datos a espacio de kernel */
  copy_from_user(memory_buffer,tmp,1);
  return 1;
}
