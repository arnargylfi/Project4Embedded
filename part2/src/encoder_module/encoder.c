#include <linux/init.h> 
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/gpio.h>                       // for the GPIO functions 
#include <linux/interrupt.h>                  // for the IRQ code
#include <linux/syscalls.h>

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Group 2 & 6"); 
MODULE_DESCRIPTION("A Encoder/LED test driver for the RPi"); 
MODULE_VERSION("0.1");

static unsigned int gpioLED = 17;             // pin 11 (GPIO17) 
static unsigned int gpioEncoderC1 = 23;          // (GPIO23) 
static unsigned int gpioEncoderC2 = 24;          // (GPIO24) 
static unsigned int irqNumber;                // share IRQ num within file 
static int pos = 0;                             //Pos value
static bool         ledOn = 0;                // used to invert state of LED



// prototype for the custom IRQ handler function, function below 
static irq_handler_t  erpi_gpio_irq_handler(unsigned int irq, 
                                            void *dev_id, struct pt_regs *regs);

static int __init erpi_gpio_init(void) 
{
    int result = 0;
    printk(KERN_INFO "ENCODER: Initializing the ENCODER LKM\n");

    if (!gpio_is_valid(gpioLED)) 
    {
        printk(KERN_INFO "ENCODER: invalid LED GPIO\n");
        return -ENODEV;
    }   

    ledOn = true;

    gpio_request(gpioLED, "sysfs");          // request LED GPIO
    gpio_direction_output(gpioLED, ledOn);   // set in output mode and on 
    // gpio_set_value(gpioLED, ledOn);       // not reqd - see line above
    gpio_export(gpioLED, false);             // appears in /sys/class/gpio
                                             // false prevents in/out change   
    gpio_request(gpioEncoderC1, "sysfs");       // set up gpioEncoder1   
    gpio_direction_input(gpioEncoderC1);        // set up as input   
    gpio_set_debounce(gpioEncoderC1, 200);      // debounce delay of 200ms
    gpio_export(gpioEncoderC1, false);          // appears in /sys/class/gpio

    gpio_request(gpioEncoderC2, "sysfs");       // set up gpioEncoder2   
    gpio_direction_input(gpioEncoderC2);        // set up as input   
    gpio_set_debounce(gpioEncoderC2, 200);      // debounce delay of 200ms
    gpio_export(gpioEncoderC2, false);          // appears in /sys/class/gpio

    printk(KERN_INFO "ENCODER: encoder1 value is currently: %d\n", 
           gpio_get_value(gpioEncoderC1));
    printk(KERN_INFO "ENCODER: encoder2 value is currently: %d\n", 
           gpio_get_value(gpioEncoderC2));

    irqNumber = gpio_to_irq(gpioEncoderC1);     // map GPIO to IRQ number
    printk(KERN_INFO "ENCODER: encoder mapped to IRQ: %d\n", irqNumber);

    // This next call requests an interrupt line    
    result = request_irq(irqNumber,                     // interrupt number requested            
        (irq_handler_t) erpi_gpio_irq_handler,          // handler function            
        IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,     // on rising edge and falling edge            
        "erpi_gpio_handler",                            // used in /proc/interrupts
        NULL);                                          // *dev_id for shared interrupt lines
    printk(KERN_INFO "ENCODER: IRQ request result is: %d\n", result);
    return result;

}

static void __exit erpi_gpio_exit(void) 
{   
    // printk(KERN_INFO "ENCODER: encoder value is currently: %d\n", 
    //        gpio_get_value(gpioEncoderC1));
    // printk(KERN_INFO "ENCODER: encoder value is currently: %d\n", 
    //        gpio_get_value(gpioEncoderC2));

    printk(KERN_INFO "ENCODER: Current pos: %d\n", pos);
    gpio_set_value(gpioLED, 0);              // turn the LED off
    gpio_unexport(gpioLED);                  // unexport the LED GPIO   
    free_irq(irqNumber, NULL);               // free the IRQ number, no *dev_id   
    gpio_unexport(gpioEncoderC1);               // unexport the Ecoder1 GPIO    
    gpio_unexport(gpioEncoderC2);               // unexport the Ecoder2 GPIO 
    gpio_free(gpioLED);                      // free the LED GPIO
    gpio_free(gpioEncoderC1);                   // free the Encoder1 GPIO
    gpio_free(gpioEncoderC2);                   // free the Encoder2 GPIO
    printk(KERN_INFO "ENCODER: Goodbye from the LKM!\n"); 
}

static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, 
                                           void *dev_id, struct pt_regs *regs) 
{   
    ledOn = !ledOn;                          // toggle the LED state   
    gpio_set_value(gpioLED, ledOn);          // set LED accordingly  
    // printk(KERN_INFO "ENCODER: Interrupt! (pulse is %d)\n", gpio_get_value(gpioEncoder));
    bool current_c1 = gpio_get_value(gpioEncoderC1);
    bool current_c2 = gpio_get_value(gpioEncoderC2);
    
    if (current_c1 != current_c2)
    {
        pos++; // clockwise rotation
    }
    else
    {
        pos--; // counterclockwise rotation
    }

    return (irq_handler_t) IRQ_HANDLED;      // announce IRQ handled 
}

asmlinkage int sys_get_pos(void){    //Getting the pos variable to be
    return pos;                      //used in main file
}

SYSCALL_DEFINE0(get_pos);

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);