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
#include "./MPU6050.h"

#define ID    	        "SG"
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
#define I2C2_IRQSTATUS_CLR_ALL               0x00006FFF
#define I2C2_IRQSTATUS_RRDY                  0x00000008 
#define I2C2_IRQSTATUS_XRDY                  0x00000010 
#define I2C2_IRQSTATUS_ARDY                  0x00000004
#define I2C2_CON_MASK                        0x0000BFF3
#define I2C2_CON_DISABLE                     0x00000000
#define I2C2_CON_EN_MST_TX                   0x00008600
#define I2C2_CON_EN_MST_RX                   0x00008400
#define I2C2_CON_START                       0x00000001 
#define I2C2_CON_STOP                        0x00000002
#define I2C2_IRQENABLE_CLR                   0x00000030
#define I2C2_IRQENABLE_CLR_MASK              0x00006FFF
#define I2C2_IRQENABLE_CLR_ALL               0x00006FFF
#define I2C2_IRQENABLE_CLR_ACK               0x00000004
#define I2C2_IRQENABLE_CLR_RX                0x00000008
#define I2C2_IRQENABLE_CLR_TX                0x00000010 
#define Tx_MODO_LECTURA 0
#define Tx_MODO_ESCRITURA 1

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

volatile uint8_t tx_registro = 0;
volatile uint8_t tx_modo = 0;
volatile uint8_t tx_data = 0;
volatile uint8_t rx_data = 0;
volatile uint16_t rx_cant = 0;
uint8_t *bufferFIFO;

int Gscale = GFS_250DPS;
int Ascale = AFS_2G;
int Aaxis = AXIS_X;

volatile int queue_cond = 0;
wait_queue_head_t queue = __WAIT_QUEUE_HEAD_INITIALIZER(queue);
wait_queue_head_t queue_in = __WAIT_QUEUE_HEAD_INITIALIZER(queue_in);

void MPU6050_writebyte(uint8_t , uint8_t , uint8_t );
int8_t MPU6050_readbyte(void);


static irqreturn_t driver_isr(int irq, void *devid) {
  static uint8_t cont = 0;
	static uint32_t contRx = 0;
	uint32_t irq_status;
	uint32_t reg_valor;

 

	// check tipo de irq
	irq_status = ioread32(i2c2 + I2C2_IRQSTATUS);

	if (irq_status & I2C2_IRQENABLE_CLR_ACK)
	{
		pr_alert("%s: IRQ handler ACK\n", ID);
	}

	if (irq_status & I2C2_IRQSTATUS_RRDY)
	{
		if (rx_cant > 1)
		{
			bufferFIFO[contRx] = ioread8(i2c2 + I2C2_DATA);
            pr_alert("%s: IRQ FIFO handler source -> RX[%d]: 0x%x\n", ID, contRx, bufferFIFO[contRx]);
			//bufferFIFO[contRx] = rx_data;
		}
		else
		{
			// leo dato
			rx_data = ioread8(i2c2 + I2C2_DATA);
			pr_alert("%s: IRQ handler source -> RX: 0x%x\n", ID, rx_data);
   
		}

		contRx++;

		if (contRx == rx_cant)
		{	
			contRx = 0;
			rx_cant = 0;
			// desabilito interupcion rx
			reg_valor = ioread32(i2c2 + I2C2_IRQENABLE_CLR);
			reg_valor |= I2C2_IRQSTATUS_RRDY;
			iowrite32(reg_valor, i2c2 + I2C2_IRQENABLE_CLR);
			// wake up read
			queue_cond = 1;
			wake_up_interruptible(&queue);
		}
		// borro flags
		reg_valor = ioread32(i2c2 + I2C2_IRQSTATUS);
		reg_valor |= 0x27;
		iowrite32(reg_valor, i2c2 + I2C2_IRQSTATUS);
	}

	if (irq_status & I2C2_IRQSTATUS_XRDY)
	{
		if (tx_modo == Tx_MODO_ESCRITURA)
		{
			if (cont > 0)
			{ 
        
				// escribo dato
				iowrite8(tx_data, i2c2 + I2C2_DATA);
				cont = 0;

				// desabilito interupcion tx
				reg_valor = ioread32(i2c2 + I2C2_IRQENABLE_CLR);
				reg_valor |= I2C2_IRQSTATUS_XRDY;
				iowrite32(reg_valor, i2c2 + I2C2_IRQENABLE_CLR);

				// wake up write
				queue_cond = 1;
				wake_up(&queue_in); //ininterumpible
									//wake_up_interruptible(&queue);
			}
			else
			{
				pr_alert("%s: IRQ handler source -> TX_reg: 0x%x\n", ID, tx_registro);
				// write registro
				iowrite8(tx_registro, i2c2 + I2C2_DATA);
				cont++;
			}
		}
		if (tx_modo == Tx_MODO_LECTURA)
		{
			pr_alert("%s: IRQ handler source -> TX_reg: 0x%x\n", ID, tx_registro);
			// escribo registro
			iowrite8(tx_registro, i2c2 + I2C2_DATA);
			// wake up write
			queue_cond = 1;
			wake_up(&queue_in); //ininterumpible
		}

		// Borro flags
		reg_valor = ioread32(i2c2 + I2C2_IRQSTATUS);
		reg_valor |= 0x36;
		iowrite32(reg_valor, i2c2 + I2C2_IRQSTATUS);
	}

	// Borro todos los flags
	irq_status = ioread32(i2c2 + I2C2_IRQSTATUS);
	irq_status |= 0x3F;
	iowrite32(irq_status, i2c2 + I2C2_IRQSTATUS);

	return IRQ_HANDLED;
}

static int i2c2_MPU6050_open(struct inode *inode, struct file *file) {
  uint16_t aux = 0;

	MPU6050_writebyte(WHO_AM_I, 0x00, Tx_MODO_LECTURA);
	aux = MPU6050_readbyte();
	pr_alert("%s: WHO_AM_I: %#04x\n", ID, aux);

	MPU6050_writebyte(PWR_MGMT_1, 0x00, Tx_MODO_ESCRITURA);

	msleep(100); 		//Delay de 100 ms para que PLL se establezca en el giroscopio del eje x

	// Fuente de reloj PLL con referencia de giroscopio del eje x, bits 2: 0 = 001
	MPU6050_writebyte(PWR_MGMT_1, 0x01, Tx_MODO_ESCRITURA); 

	// Configurar giroscopio y acelerómetro
	// DLPF_CFG = bits 2:0 = 010; establece la frecuencia de muestreo en 1 kHz para ambos
	MPU6050_writebyte(CONFIG, 0x03, Tx_MODO_ESCRITURA);

	// Sample rate = giroscopio output rate/(1 + SMPLRT_DIV)
	MPU6050_writebyte(SMPLRT_DIV, 0x63, Tx_MODO_ESCRITURA); // Use a 10 Hz sample rate 100ms

	// Configuracion giroscopio 
	MPU6050_writebyte(GYRO_CONFIG, 0x00, Tx_MODO_ESCRITURA);

	// Configuracion acelerómetro
	MPU6050_writebyte(ACCEL_CONFIG, 0x00, Tx_MODO_ESCRITURA);

	// Configurar interrupciones y habilitar Bypass
	MPU6050_writebyte(INT_PIN_CFG, 0x02, Tx_MODO_ESCRITURA);

	// Habilitar interrupción de datos listos (bit 0)
	MPU6050_writebyte(INT_ENABLE, 0x01, Tx_MODO_ESCRITURA);

	//bit 6 FIFO_EN 	a 1
	//bit 2 FIFO_RESET	a 1
	MPU6050_writebyte(USER_CTRL, 0x44, Tx_MODO_ESCRITURA);

	//Config FIFO
	//Habilito FIFO para Temp, Acel y Gyro
	MPU6050_writebyte(FIFO_EN, 0xF8, Tx_MODO_ESCRITURA);

}

void MPU6050_writebyte(uint8_t registro, uint8_t data, uint8_t modo)
{
	uint32_t i = 0;
	uint32_t reg_valor = 0;
	//uint32_t estado = 0;


	// Chequeo si la línea esta BUSY
	reg_valor = ioread32(i2c2 + I2C2_IRQSTATUS_RAW);
	if((reg_valor >> 12) & 0x01)
	{
		return;
	}

	// cargo datos a transmitir
	tx_data = data;
	tx_registro = registro;
	tx_modo = modo;

	if (modo == Tx_MODO_ESCRITURA)
	{
		//Tx por escritura
		// data len = 2 byte
		iowrite32(2, i2c2 + I2C2_CNT);
	}
	if (modo == Tx_MODO_LECTURA)
	{
		//Tx por lectura
		// data len = 1 byte
		iowrite32(1, i2c2 + I2C2_CNT);
	}

	// config registro -> ENABLE & MASTER & TX
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor |= 0x600;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	// habilito tx interupcion
	iowrite32(I2C2_IRQSTATUS_XRDY, i2c2 + I2C2_IRQENABLE_SET);

	// Genero condición de START
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor |= I2C2_CON_START;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	// Pongo el proceso ininterumpible (por estar escribiendo registros) a esperar a que la IRQ termine de transmitir.
	wait_event_interruptible(queue_in, queue_cond > 0); 

	queue_cond = 0;

	// Genero condición de STOP
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor &= 0xFFFFFFFE;
	reg_valor |= I2C2_CON_STOP;
	iowrite32(reg_valor, i2c2 + I2C2_CON);


	msleep(1);
}

int8_t MPU6050_readbyte(void)
{
	uint32_t i = 0;
	uint32_t reg_valor = 0;
	uint32_t estado = 0;
	int8_t nuevo_dato;

	// Chequeo si la línea esta BUSY
	reg_valor = ioread32(i2c2 + I2C2_IRQSTATUS_RAW);
	if((reg_valor >> 12) & 0x01)
	{
		pr_alert("%s: writebyte ERROR Device busy\n", ID);
		return;
	}
	pr_alert("%s: readbyte\n", ID);

	// data = n byte
	rx_cant = 1;
	iowrite32(1, i2c2 + I2C2_CNT);

	// config registro -> ENABLE & MASTER & RX
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor = 0x8400;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	// Habilito interupcion rx
	iowrite32(I2C2_IRQSTATUS_RRDY, i2c2 + I2C2_IRQENABLE_SET);

	// Genero condición de START
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor &= 0xFFFFFFFC;
	reg_valor |= I2C2_CON_START;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	// Espero a que la recepción finalice poniendo en espera el proceso.
	if ((estado = wait_event_interruptible(queue, queue_cond > 0)) < 0)
	{
		queue_cond = 0;
		pr_alert("%s: readbyte ERROR read\n", ID);
		return estado;
	}

	queue_cond = 0;

	// Genero condición de STOP
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor &= 0xFFFFFFFE;
	reg_valor |= I2C2_CON_STOP;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	pr_alert("%s: readbyte OK\n", ID);
	nuevo_dato = rx_data;

	return nuevo_dato;
}

int8_t MPU6050_readFIFO(uint16_t cant)
{
	uint32_t i = 0;
	uint32_t reg_valor = 0;
	uint32_t estado = 0;

	// Chequeo si la línea esta BUSY
	reg_valor = ioread32(i2c2 + I2C2_IRQSTATUS_RAW);
	if((reg_valor >> 12) & 0x01)
	{
		pr_alert("%s: writebyte ERROR Device busy\n", ID);
		return;
	}
	
	pr_alert("%s: readFIFO\n", ID);

	// data = n byte
	rx_cant = cant;
	iowrite32(cant, i2c2 + I2C2_CNT);

	// config registro -> ENABLE & MASTER & RX
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor = 0x8400;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	// Habilito interupcion rx
	iowrite32(I2C2_IRQSTATUS_RRDY, i2c2 + I2C2_IRQENABLE_SET);

	// Genero condición de START
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor &= 0xFFFFFFFC;
	reg_valor |= I2C2_CON_START;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	// Espero a que la recepción finalice poniendo en espera el proceso.
	if ((estado = wait_event_interruptible(queue, queue_cond > 0)) < 0)
	{
		queue_cond = 0;
		//pr_alert("%s: readFIFO ERROR read\n", ID);
		return estado;
	}

	queue_cond = 0;

	// Genero condición de STOP
	reg_valor = ioread32(i2c2 + I2C2_CON);
	reg_valor &= 0xFFFFFFFE;
	reg_valor |= I2C2_CON_STOP;
	iowrite32(reg_valor, i2c2 + I2C2_CON);

	pr_alert("%s: readFIFO OK\n", ID);

	return 0;
}

ssize_t i2c2_MPU6050_read(struct file* file, char __user  *userbuff, size_t len, loff_t * offset)
{
	uint16_t countFIFO;
	uint8_t buffcountFIFO[2];

	pr_alert("%s: I2C driver read\n", ID);

	if (access_ok(VERIFY_WRITE, userbuff, len) == 0)
	{
		pr_alert("%s: Falla buffer de usuario\n", ID);
		return -1;
	}
	
	//Nunca leer mas del tamaño limite de memoria de datos.	1022
	if (len > 1022)
	{
		len = 1022; //Si me pasan un tamaño mayor al de la estructura lo seteo con el maximo
	}
	
	if (len % 14 != 0)
	{
		pr_alert("%s: El tamaño pasado no es multiplo de 14\n", ID);
		return -1;
	}

	bufferFIFO = (uint8_t *) kmalloc( len * sizeof(uint8_t), GFP_KERNEL);
	if(bufferFIFO == NULL)
	{
		pr_alert("%s: No se pude pedir memoria en Kernel\n", ID);
		return -1;
	}

	//bit 6 FIFO_EN 	a 1
	//bit 2 FIFO_RESET	a 1
	MPU6050_writebyte(USER_CTRL, 0x44, Tx_MODO_ESCRITURA);
	do
	{
		// leo cant de datos FIFO
		MPU6050_writebyte(FIFO_COUNTH, 0x00, Tx_MODO_LECTURA);
		buffcountFIFO[0] = MPU6050_readbyte();

		MPU6050_writebyte(FIFO_COUNTL, 0x00, Tx_MODO_LECTURA);
		buffcountFIFO[1] = MPU6050_readbyte();
		countFIFO = (uint16_t)((buffcountFIFO[0] << 8) | buffcountFIFO[1]);
		pr_info("%s: Comparo Tam FIFO %d Tam Lectura %d\n", ID, countFIFO, len);
	} while (countFIFO < len);

	MPU6050_writebyte(FIFO_R_W, 0x00, Tx_MODO_LECTURA);
	MPU6050_readFIFO(len);

	if (copy_to_user(userbuff, bufferFIFO, len) > 0) //en copia correcta devuelve 0
	{
		pr_alert("%s: Falla en copia de buffer de kernel a buffer de usuario\n", ID);
		return -1;
	}
	
	kfree(bufferFIFO);

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
  .open = i2c2_MPU6050_open,
  .read = i2c2_MPU6050_read,
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
  
  //Solicito un id para nuestra interrupcion
 if ((state.irq = platform_get_irq(pdev, 0)) < 0) {
    dev_err(&pdev->dev, "No se pudo obtener la interrupcion\n");
    return 1;
  }

  //Asocia la línea de interrupcion con mi handler
  if ((request_irq(state.irq, driver_isr,IRQF_TRIGGER_RISING, pdev->name, NULL)) < 0) {
    dev_err(&pdev->dev, "No se pudo asociar el handler a la linea\n");
    return 1;
  }
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


  reg_aux = ioread32(i2c2 + I2C2_PSC);
  iowrite32(0x2,(i2c2 + I2C2_PSC));  //Preescaler para obtener f=12MHz como indica el manual
  reg_aux = 55;
  iowrite32(reg_aux,(i2c2 + I2C2_SCLH)); //Se obtienen 100kbps
  reg_aux = 53;
  iowrite32(reg_aux,(i2c2 + I2C2_SCLL)); //Se obtienen 100kbps
  iowrite32(0x8600,(i2c2 + I2C2_CON)); //Habilitando i2c
  reg_aux = 0x68;
  iowrite32(reg_aux,(i2c2 + I2C2_SA)); //Indico la direccion del sensor

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
  free_irq(state.irq,NULL);
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
