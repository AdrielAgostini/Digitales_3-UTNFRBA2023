#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/pm_runtime.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/uaccess.h>
#include <linux/pinctrl/consumer.h>

#define BASE_MINOR 0
#define MINOR_COUNT 1
#define DEVICE_PARENT NULL
#define DEVICE_NAME "acelerometro-td3"
#define DEVICE_CLASS_NAME "acelerometro-td3"

#define CM_PER_START 0x44E00000
#define CM_PER_END 0x44E003FF
#define CM_PER_SIZE CM_PER_END-CM_PER_START
#define CM_PER_I2C2_CLKCTRL 0x44

#define CONTROL_MODULE_START 0x44E10000 
#define CONTROL_MODULE_END 0x44E11FFF
#define CONTROL_MODULE_SIZE CONTROL_MODULE_END-CONTROL_MODULE_START
#define CONTROL_MODULE_I2C2_SDA 0x978
#define CONTROL_MODULE_I2C2_SCL 0x97C

#define I2C2_START 0x4819C000
#define I2C2_END   0x4819CFFF 
#define I2C2_SIZE I2C2_END - I2C2_START		
#define I2C2_PSC 0xB0
#define I2C2_SCLL 0xB4
#define I2C2_SCLH 0xB8
#define I2C2_OA 0xA8
#define I2C2_SA 0xAC
#define I2C2_CON 0xA4
#define I2C2_IRQENABLE_SET 0x2C
#define I2C2_IRQSTATUS_RAW 0x24
#define I2C2_IRQSTATUS 0x28
#define I2C2_CNT 0x98
#define I2C2_DATA 0x9C

#define FREE_BUS 1

static struct {
  struct class *devclass;
  struct device *dev;
  struct cdev chardev;
  dev_t devtype;
  int irq;
  int flag;
  int valid;
} state;

void __iomem *cm_per;
void __iomem *control_module;
void __iomem *i2c2;

static wait_queue_head_t waitqueue;


static irqreturn_t driver_isr(int irq, void *devid) {
  int aux;
  aux = ioread32(i2c2+I2C2_IRQSTATUS_RAW);

  if((aux|0x1000) == 0){
    state.flag = FREE_BUS;
    wake_up_interruptible(&waitqueue);
  }

  return IRQ_HANDLED;
}

static int i2c2_open(struct inode *inode, struct file *file) {
  uint32_t reg_aux = 0;
  
  reg_aux = ioread32(i2c2 + I2C2_PSC);
  iowrite32(0x2,(i2c2 + I2C2_PSC));  

  reg_aux = 55;
  iowrite32(reg_aux,(i2c2 + I2C2_SCLH)); /

  reg_aux = 53;
  iowrite32(reg_aux,(i2c2 + I2C2_SCLL)); 

  iowrite32(0x8600,(i2c2 + I2C2_CON)); 

  reg_aux = 0x69;
  iowrite32(reg_aux,(i2c2 + I2C2_SA)); 


  reg_aux = ioread32(i2c2 + I2C2_CNT);
  iowrite32(100,(i2c2 + I2C2_CNT)); 

  
  reg_aux = ioread32(i2c2 + I2C2_IRQSTATUS_RAW);
  printk(KERN_INFO "IRQ_STATUS = %X",reg_aux);
  
  reg_aux = ioread32(i2c2 + I2C2_CON);
  iowrite32(reg_aux|0x1,(i2c2 + I2C2_CON)); 
  iowrite32(0x4,(i2c2 + I2C2_DATA)); 
  

  return 0;
}

static ssize_t i2c2_read(struct file *file, char __user *buffer,
                           size_t size, loff_t *offset) {

  return 0;
}

static int i2c2_release(struct inode *inode, struct file *file) {
  iowrite32(0x0000, i2c2 + I2C2_CON);
  iowrite32(0x00, i2c2 + I2C2_PSC);
  iowrite32(0x00, i2c2 + I2C2_SCLL);
  iowrite32(0x00, i2c2 + I2C2_SCLH);
  iowrite32(0x00, i2c2 + I2C2_OA);
  iowrite32(0x00,i2c2 + I2C2_IRQENABLE_SET);

  return 0;
}

static const struct file_operations driver_file_op = {
  .owner = THIS_MODULE,
  .open = i2c2_open,
  .read = i2c2_read,
  .release = i2c2_release
};

//Punteros a los registros necesarios para el dispositivo

static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
  add_uevent_var(env, "DEVMODE=%#o", 0666);
  return 0;
}


static int driver_probe(struct platform_device *pdev) {
  int status = 0;
  state.flag = 0;
  uint32_t reg_aux;
  //struct resource *res = NULL;

  dev_info(&pdev->dev, "Entro al probe\n");
  
  //Obtengo el puntero para poder acceder a los registrod del clock 
  cm_per = ioremap((unsigned long)CM_PER_START,(unsigned long)CM_PER_SIZE);
  if(cm_per == NULL){
    dev_err(&pdev->dev, "No se pudieron mapear los registros CM_PER\n");
    return 1;
  }
  reg_aux = ioread32(cm_per + CM_PER_I2C2_CLKCTRL);
  iowrite32(reg_aux|0x2,(cm_per + CM_PER_I2C2_CLKCTRL)); //Habilito el clock

  i2c2 = ioremap((unsigned long)I2C2_START,(unsigned long)I2C2_SIZE);
  if(i2c2 == NULL){
    iounmap(cm_per);
    iounmap(control_module);
    dev_err(&pdev->dev, "No se pudieron mapear los registros I2C2\n");
    return 1;
  }

  //Obtengo el puntero para poder acceder a los registrod del clock 
  control_module = ioremap((unsigned long)CONTROL_MODULE_START,(unsigned long)CONTROL_MODULE_SIZE);
  if(control_module == NULL){
    iounmap(cm_per);
    dev_err(&pdev->dev, "No se pudieron mapear los registros CONTROL_MODULE\n");
    return 1;
  }
  reg_aux = ioread32(control_module + CONTROL_MODULE_I2C2_SDA);
  iowrite32(reg_aux|0x23,(control_module + CONTROL_MODULE_I2C2_SDA));
  reg_aux = ioread32(control_module + CONTROL_MODULE_I2C2_SCL);
  iowrite32(reg_aux|0x23,(control_module + CONTROL_MODULE_I2C2_SCL));

  if ((status = alloc_chrdev_region(
          &state.devtype,
          BASE_MINOR,
          MINOR_COUNT,
          DEVICE_NAME)) != 0) {
    free_irq(state.irq,NULL);
    iounmap(cm_per);
    iounmap(control_module);
    dev_err(&pdev->dev, "No se pudo reserver la char device region\n");
    return status;
  }

  state.devclass = class_create(THIS_MODULE, DEVICE_CLASS_NAME);
  if (IS_ERR(state.devclass)) {
    dev_err(&pdev->dev, "No se pudo crear la clase\n");
    free_irq(state.irq,NULL);
    iounmap(cm_per);
    iounmap(control_module);
    unregister_chrdev_region(
        state.devtype, MINOR_COUNT);
    return PTR_ERR(state.devclass);
  }

  state.devclass-> dev_uevent = my_dev_uevent;

  state.dev = device_create(state.devclass, DEVICE_PARENT,state.devtype, NULL, DEVICE_NAME);
  if (IS_ERR(state.dev)) {
    dev_err(&pdev->dev, "No se pudo crear el char device [%d]\n", status);
    free_irq(state.irq,NULL);
    iounmap(cm_per);
    iounmap(control_module);
    class_destroy(state.devclass);
    unregister_chrdev_region(
        state.devtype, MINOR_COUNT);
    return PTR_ERR(state.dev);
  }

  cdev_init(&state.chardev, &driver_file_op);

  if ((status = cdev_add(&state.chardev, state.devtype,
                         MINOR_COUNT)) != 0) {
    dev_err(&pdev->dev, "No se pudo agregar el char device\n");
    free_irq(state.irq,NULL);
    iounmap(cm_per);
    iounmap(control_module);
    device_destroy(state.devclass, state.devtype);
    class_destroy(state.devclass);
    unregister_chrdev_region(
        state.devtype, MINOR_COUNT);
    return status;
  }

  dev_info(&pdev->dev, "Salgo del probe\n");

  return 0;
}

static int driver_remove(struct platform_device *pdev) {
  dev_info(&pdev->dev, "Desmontando Driver\n");
  //free_irq(state.irq,NULL);
  iounmap(cm_per);
  iounmap(control_module);
  iounmap(i2c2);
  cdev_del(&state.chardev);
  device_destroy(state.devclass, state.devtype);
  class_destroy(state.devclass);
  unregister_chrdev_region(
      state.devtype, MINOR_COUNT);
  dev_info(&pdev->dev, "Driver desmontado\n");
  return 0;
}

static const struct of_device_id driver_of_match[] = {
  { .compatible = "td3,omap4-i2c" },
  { },
};

MODULE_DEVICE_TABLE(of, driver_of_match);

static struct platform_driver driver = {
  .probe = driver_probe,
  .remove = driver_remove,
  .driver = {
    .name = "driver_acelerometro",
    .of_match_table = of_match_ptr(driver_of_match),
  },
};

static int __init init_driver(void) {
  return platform_driver_register(&driver);
}
module_init(init_driver);

static void __exit exit_driver(void) {
  platform_driver_unregister(&driver);
}
module_exit(exit_driver);

MODULE_AUTHOR("Bruno");
MODULE_DESCRIPTION("Driver de prueba para TD3");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:prueba-td3");
