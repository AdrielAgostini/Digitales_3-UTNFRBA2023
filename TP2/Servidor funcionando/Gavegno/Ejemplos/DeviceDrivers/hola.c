// Hecho por Dario Alpern
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Dario Alpern");
MODULE_DESCRIPTION("Primer device driver");
static int hello_init(void)
{
  printk(KERN_CRIT "Hola mundo!!!\n");
  return 0;
}
static void hello_exit(void)
{
  printk(KERN_CRIT "Adios mundo cruel!!!\n");
}
module_init(hello_init);
module_exit(hello_exit);