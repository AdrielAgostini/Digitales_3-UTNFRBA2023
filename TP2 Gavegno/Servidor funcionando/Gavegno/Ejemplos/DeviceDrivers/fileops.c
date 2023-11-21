// Hecho por Dario Alpern
#include <linux/kernel.h>    // Las dos primeras declaraciones
#include <linux/module.h>    // son obligatorias para modulos
#include <linux/sched.h>
#include <linux/tty.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Dario Alpern");
MODULE_DESCRIPTION("Muestra file operations");

// Funcion que muestra texto en la consola (TTY)
void print_string(char *str)
{
  struct tty_struct *my_tty;
  // Obtener handle de tty para la tarea actual
  my_tty = get_current_tty();

  if (my_tty != NULL)
  {
    (my_tty->driver->ops->write)(
        my_tty,        // Handle de tty
        str,           // String a mostrar
        strlen(str));  // Longitud del string
    (my_tty->driver->ops->write)(
      my_tty,          // Handle de tty.
      "\015\012",      // CR - LF en octal.
      2);              // Longitud.
  }
}

static int fileops_open(struct inode * inode, struct file * file)
{
  printk(KERN_CRIT "fileops: abriendo.\n");
  return 0;
}

static ssize_t fileops_read (struct file *file, char *buf, size_t count, loff_t *offset)
{
  printk (KERN_CRIT "fileops: leyendo %d bytes.\n", count);
  return count;
}

static ssize_t fileops_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
  printk (KERN_CRIT "fileops: escribiendo %d bytes.\n", count);
  print_string("Prueba de escritura en pantalla.");
  return count;
}

static int fileops_release (struct inode *inode, struct file *file)
{
  printk (KERN_CRIT "fileops: cerrando.\n");
  return 0;
}

struct file_operations fileops_fops =
{
  .read = fileops_read,
  .write = fileops_write,
  .open = fileops_open,
  .release = fileops_release,
};


int init_module()
{
  if (register_chrdev(201, "fileops", &fileops_fops))
  {
    printk(KERN_CRIT "fileops: init_module ha FALLADO intentando instalar el driver.\n");
    return -EIO;
  }

  printk (KERN_CRIT "fileops: Modulo cargado\n");
  return 0;
}

void cleanup_module()
{
  unregister_chrdev(201, "fileops");
  print_string("Chau");
  printk (KERN_CRIT "fileops: Modulo eliminado\n");
}
