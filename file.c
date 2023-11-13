// Linux required Headers
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

// CDD - Custom Character Device Driver

// Parameters
int custom_kernel_version[2], custom_time;
module_param(custom_time, int, S_IRUSR | S_IWUSR);
module_param_array(custom_kernel_version, int, NULL, S_IRUSR | S_IWUSR);

// Variables
static int used_length;
// milliseconds
#define TIMEOUT 1000 

// Device Number
dev_t custom_dev_number;
// Device Class
static struct class *custom_dev_class;
// Cdev Struct
static struct cdev custom_cdev;
// Device File Struct
static struct device *custom_dev;
// CDD's Kernel Space Buffer Pointer
uint8_t *custom_kernel_buffer;
// Wait Thread
static struct task_struct *custom_wait_thread;
// Wait Queue Head and Usage Flag
wait_queue_head_t custom_wait_queue;
// Wait Queue Flag
static int custom_wait_queue_flag = 0;
// User Name Array
char custom_user_name[15];
// Timer Declaration
static struct timer_list custom_timer;
// Timer Expired Flag
static int custom_timer_expired = 0;
// Read Count
static uint32_t custom_read_count = 0;

//This function will be called when we open the device file
static int custom_cdd_open(struct inode *inode, struct file *file)
{
    pr_info("Driver Open Function Called...!!!");
    pr_info("\n");
    return 0;
}

//This function will be called when we close the device file
static int custom_cdd_release(struct inode *inode, struct file *file)
{
    pr_info("Driver Release Function Called...!!!");
    pr_info("\n");
    return 0;
}

//This function will be called when we read the device file
static ssize_t custom_cdd_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    used_length = strlen(custom_kernel_buffer);
    pr_info("Device Read Function Called....!!");
    pr_info("\n");
    /* If off is behind the end of a file, we have nothing to read */
    if (*off - used_length >= 0 ) return 0;
    /* If a user tries to read more than we have, read only as many bytes as we have */
    if (*off > used_length - len ) len = (used_length - *off);
    if (copy_to_user(buf, custom_kernel_buffer + *off, len) != 0)
    {
        pr_err("Data Read : ERR!");
        pr_info("\n");
        return -EFAULT;
    }
    // Move reading off 
    *off = *off + len;

    // Wake up the queue if a read signal is encountered
    custom_wait_queue_flag = 1;
    wake_up_interruptible(&custom_wait_queue);

    // Reset the timer
    mod_timer(&custom_timer, jiffies + msecs_to_jiffies(custom_time * TIMEOUT));

    return len;
}

// This function will be called when we write the device file
static ssize_t custom_cdd_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    pr_info("Driver Write Function Called...!!!");
    pr_info("\n");

    // If the offset is beyond our space
    if (*off - used_length >= 0 ) return 0;
    // If the user tries to access more than the size we have
    if (*off  > used_length - len ) len = used_length - *off;

    if (copy_from_user(custom_kernel_buffer + *off, buf, len))
    {
        pr_err("Data Write : ERR!");
        pr_info("\n");
        return -EFAULT;
    }

    // Move writing off
    *off = *off + len;

    // Wake up the wait queue if the order is satisfied
    custom_wait_queue_flag = 2;
    wake_up_interruptible(&custom_wait_queue);

    // Reset the timer
    mod_timer(&custom_timer, jiffies + msecs_to_jiffies(custom_time * TIMEOUT));

    return len;
}

// Timer Callback Function. This will be called when the timer expires
void custom_timer_callback(struct timer_list *data)
{
    pr_info("Timer of %d seconds Expired. No activity within this time.", custom_time);
    pr_info("\n");
    custom_timer_expired = 1;
}

// File Operations for the Cdev Struct 
struct file_operations custom_f_ops =  {
                                            .owner = THIS_MODULE,
                                            .read = custom_cdd_read,
                                            .write = custom_cdd_write,
                                            .open = custom_cdd_open,
                                            .release = custom_cdd_release,
                                        };

// Function for the Kernel Thread
static int custom_wait_function(void *unused)
{
    // Purpose is to print the number of reads and capture the event of exit
    while (true)
    {
        pr_info("Waiting For Event...");
        pr_info("\n");
        wait_event_interruptible(custom_wait_queue, custom_wait_queue_flag != 0);
        if (custom_wait_queue_flag - 2 == 0 && custom_read_count)
        {
            pr_info("Event Came From Write after Read Function");
            pr_info("\n");

            // Indicating the desired order of events
            custom_wait_queue_flag = 3;

            memcpy(custom_user_name, custom_kernel_buffer, used_length);
            return 0;
        }
        custom_read_count++;
        pr_info("Event Came From Read Function - %d",custom_read_count);
        pr_info("\n");
        custom_wait_queue_flag = 0;
    }
    do_exit(0);
    return 0;
}

// Module Init Function
static int __init custom_cdd_init(void)
{
    // Check kernel version
    if (KERNEL_VERSION(custom_kernel_version[0], custom_kernel_version[1], 0) !=
        KERNEL_VERSION(LINUX_VERSION_MAJOR, LINUX_VERSION_PATCHLEVEL, 0))
    {
        pr_err("Not compatible with this Kernel Version");
        pr_info("\n");
        return -1;
    }

    pr_info("Kernel Module Inserted Successfully...");
    pr_info("\n");

    // Allocate device number
    if ((alloc_chrdev_region(&custom_dev_number, 0, 1, "custom_cdd_device")) < 0)
    {
        pr_err("Unable to allocate device number");
        pr_info("\n");
        return -1;
    }

    // Printing major no. and minor no. of device no.
    pr_info("Major number is %d",MAJOR(custom_dev_number));
    pr_info("\n");
    pr_info("Minor number is %d",MINOR(custom_dev_number));
    pr_info("\n");
    pr_info("Timer Span is %d seconds",custom_time);
    pr_info("\n");
    // Create device class
    custom_dev_class = class_create(THIS_MODULE, "custom_cdd_class");

    if (IS_ERR(custom_dev_class))
    {
        pr_err("Unable to create struct class for the device");
        pr_info("\n");
        goto r_class;
    }

    // Create device
    if (IS_ERR(custom_dev = device_create(custom_dev_class, NULL, custom_dev_number, NULL, "custom_cdd_device")))
    {
        pr_err("Unable to create device");
        pr_info("\n");
        goto r_device;
    }

    // Cdev entry
    // Initialize cdev entry
    cdev_init(&custom_cdev, &custom_f_ops);

    // Add the cdev entry to the system
    if (cdev_add(&custom_cdev, custom_dev_number, 1) < 0)
    {
        pr_err("Unable to add Cdev entry");
        pr_info("\n");
        goto r_cdev;
    }

    // Allocating kernel space memory for our driver
    if (IS_ERR(custom_kernel_buffer = kmalloc(DEFAULT_SIZE, GFP_KERNEL)))
    {
        pr_err("Unable to allocate kernel space for the driver");
        pr_info("\n");
        goto r_cdev;
    }
    strcpy(custom_kernel_buffer, "Default\n");

    /* Setup your timer to call custom_timer_callback */
    timer_setup(&custom_timer, custom_timer_callback, 0);

    mod_timer(&custom_timer, jiffies + msecs_to_jiffies(custom_time * TIMEOUT));
    pr_info("Timer Started");
    pr_info("\n");

    // Initializing a wait queue
    init_waitqueue_head(&custom_wait_queue);

    // Create a wait thread named "custom_wait_thread"
    custom_wait_thread = kthread_create(custom_wait_function, NULL, "CustomWaitThread");
    pr_info("Creating a Thread on custom_wait_function");
    if (custom_wait_thread)
    {
        pr_info("Thread Created successfully");
        pr_info("\n");
        wake_up_process(custom_wait_thread);
    }
    else
    {
        pr_info("Thread creation failed");
        pr_info("\n");
    }

    // For returning normally if no errors are encountered
    return 0;

    // If the entry is not added due to an error
    r_cdev:
    device_del(custom_dev);

    // If the device can't be created, destroy the created class and dev_number
    r_device:
    class_destroy(custom_dev_class);

    // If the class can't be created, unregister the dev_number
    r_class:
    unregister_chrdev_region(custom_dev_number, 1);

    return -1;
}

// Module Exit Function
static void __exit custom_cdd_exit(void)
{
    // Remove the timer while unloading the module
    del_timer(&custom_timer);

    if (custom_wait_queue_flag - 3 == 0  && !custom_timer_expired)
    {
        pr_info("Successfully completed the actions within time");
        pr_info("\n");
        //higlighting username 
        pr_info("User Name is \033[48;5;8m%s\033[0m", custom_user_name);
        pr_info("\n");
    }
    else
    {
        pr_info("Failure");
        pr_info("\n");
    }

    // Freeing the space we used
    kfree(custom_kernel_buffer);

    // Delete cdev entry
    cdev_del(&custom_cdev);

    // Delete device
    device_del(custom_dev);

    // Destroy the device class
    class_destroy(custom_dev_class);

    // Unregistering the device number while exiting
    unregister_chrdev_region(custom_dev_number, 1);

    pr_info("Kernel Module Removed Successfully...");
    pr_info("\n");
}

// Register init and exit functions
module_init(custom_cdd_init);
module_exit(custom_cdd_exit);

// License
MODULE_LICENSE("GPL");
MODULE_AUTHOR("VEDURUPAKA VENKATA SAI");
MODULE_DESCRIPTION("A CUSTOM CHARACTER DEVICE DRIVER");
MODULE_VERSION("6:5.9");  
