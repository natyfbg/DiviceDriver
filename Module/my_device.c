// Name: Nathnael Gebre
// ID: 922065271
// CSC: 415-02 Operating Systems
// Github: natyfbg

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

// write a string, then read back the encrypted data
#define ENCRYPT _IO('a', 'a')
// write the encrypted string and read back the original message
#define DECRYPT _IO('a', 'b')
// write the key for encript the data
#define SET_KEY _IOW('a', 'b', int32_t *)

#define BUFFER_LEN 1024

static int isOpen = 0;
static int isEncrypt = 1;
static int key = 0;
static char message[BUFFER_LEN];

static struct class *mclass;
static struct cdev mcdev;
dev_t dev = 0;

// definition of device driver functions
static int mydevice_open(struct inode *inode, struct file *filp);
static ssize_t mydevice_read(struct file *filp, char __user *ubuffer, size_t nbytes, loff_t *offset);
static ssize_t mydevice_write(struct file *filp, const char __user *ubuffer, size_t nbytes, loff_t *offset);
static long mydevice_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int mydevice_release(struct inode *inode, struct file *filep);

static int __init mydevice_init(void);
static void __exit mydevice_exit(void);

// file operation structure
static struct file_operations fops =
    {
        .open = mydevice_open,
        .read = mydevice_read,
        .write = mydevice_write,
        .unlocked_ioctl = mydevice_ioctl,
        .release = mydevice_release,
};

// This is a simple crypto alghoritm that switch the ASCII alpha numeric characters from an offset given by the key
void crypt_the_message(char *buf)
{
    int i, j, temp;

    for (i = 0, j = 0; message[i] != '\0'; i++)
    {
        // printf("%c ",message[i]);
        if (message[i] == ' ')
            buf[j++] = message[i];
        else
        {
            // if message contains number from 0 to 9
            if (message[i] >= 48 && message[i] <= 57)
            {
                temp = message[i] + key;
                if (temp > 57)
                    buf[j++] = (char)(48 + (temp - 58));
                else
                    buf[j++] = (char)temp;
            }
            // else if message contains alpha characthers
            else if (message[i] >= 97 && message[i] <= 123)
            {
                temp = message[i] + key;
                if (temp > 122)
                    buf[j++] = (char)(97 + (temp - 123));
                else
                    buf[j++] = (char)temp;
            }
            else
                buf[j++] = message[i];
        }
    }
}

// This is a simple crypto alghoritm
void decrypt_the_message(char *buf)
{
    int i, j, temp;

    for (i = 0, j = 0; message[i] != '\0'; i++)
    {
        if (message[i] == ' ')
            buf[j++] = message[i];
        else
        {
            // if message contains number from 0 to 9
            if (message[i] >= 48 && message[i] <= 57)
            {
                temp = message[i] - key;
                if (temp < 48)
                    buf[j++] = (char)(58 - (48 - temp));
                else
                    buf[j++] = temp;
            }
            // else if message contains alpha characthers
            else if (message[i] >= 97 && message[i] <= 123)
            {
                temp = message[i] - key;
                if (temp < 97)
                    buf[j++] = 123 - (97 - temp);
                else
                    buf[j++] = temp;
            }
            else
                buf[j++] = message[i];
        }
    }
    buf[j] = '\0';
}

// function for opening the device file
static int mydevice_open(struct inode *inode, struct file *filp)
{
    // if file is open value is 1 else 0
    if (isOpen == 1)
    {
        printk(KERN_INFO "Error: device already opened \n");
        return -EBUSY; // return the error
    }

    // Open the device
    isOpen = 1;
    printk(KERN_INFO "Device Driver File Opened\n");
    return 0;
}

// function for reading from device driver
static ssize_t mydevice_read(struct file *filp, char __user *ubuffer, size_t nbytes, loff_t *offset)
{
    char buffer[BUFFER_LEN];
    int toread;

    if (nbytes > BUFFER_LEN)
        return -ENOBUFS;

    if (isEncrypt)
        crypt_the_message(buffer); // if ioctl set the Encryption state
    else
        decrypt_the_message(buffer); // if ioctl set the Decryption state

    toread = strlen(buffer);

    // Copy the message to user space
    if (copy_to_user(ubuffer, buffer, toread) != 0)
        return -EINVAL;

    return toread;
}

// function for writing to device driver
static ssize_t mydevice_write(struct file *filp, const char __user *ubuffer, size_t nbytes, loff_t *offset)
{
    // If the number of bytes of the buffer passed to write (from user space) is grather than message size buffer, returns an error
    if (nbytes >= BUFFER_LEN)
        return -ENOBUFS;

    // Set memory message to 0 before to write inside it
    memset(message, 0x0, BUFFER_LEN);
    // get the message from user space
    if (copy_from_user(message, ubuffer, nbytes) > 0)
        return -EINVAL;

    return nbytes;
}

// function for IOCTL operation in device file
static long mydevice_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    // cmd contains the command we want execute from ioctl
    switch (cmd)
    {
    case ENCRYPT:
        isEncrypt = 1;
        printk(KERN_INFO "Encrypt mode\n");
        break;
    case DECRYPT:
        isEncrypt = 0;
        printk(KERN_INFO "Decrypt mode\n");
        break;
    case SET_KEY:
        // get_user get a variable from an address in user space. The argument arg stores a user space address where the variable is saved.
        get_user(key, (int32_t *)arg);
        printk(KERN_INFO "Setting key to %d\n", key);
        break;
    }

    return 0;
}

// function to release the device driver
static int mydevice_release(struct inode *inode, struct file *filep)
{
    // if device not opened
    if (isOpen == 0)
    {
        printk(KERN_INFO "ERROR: device was not opened \n");
        return -EBUSY;
    }

    printk(KERN_INFO "Device Driver File Closed\n");

    // keeping track of device file
    isOpen = 0;

    return 0;
}

static int __init mydevice_init(void)
{
    // Register the device
    if ((alloc_chrdev_region(&dev, 0, 1, "my_device module")) < 0)
    {
        printk(KERN_INFO "ERROR: Device init failed");
        return -EINVAL;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    cdev_init(&mcdev, &fops);

    if ((cdev_add(&mcdev, dev, 1)) < 0)
    {
        printk(KERN_INFO "ERROR: could not add to driver.\n");
        goto destroy_device;
    }

    if ((mclass = class_create(THIS_MODULE, "my_class")) == NULL)
    {
        printk(KERN_INFO "ERROR: could not create struct class\n");
        goto destroy_device;
    }

    // create the /dev/my_device file to open this device from user space
    if ((device_create(mclass, NULL, dev, NULL, "my_device")) == NULL)
    {
        printk(KERN_INFO "ERROR: could not create Device Driver\n");
        goto destroy_class;
    }

    printk(KERN_INFO "\tDevice Driver Installation Succeed\n");
    return 0;

destroy_class:
    class_destroy(mclass);
destroy_device:
    unregister_chrdev_region(dev, 1);
    return -EINVAL;
}

static void __exit mydevice_exit(void)
{
    // destroy the device
    device_destroy(mclass, dev);
    class_destroy(mclass);
    cdev_del(&mcdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver removed\n");
}

module_init(mydevice_init);
module_exit(mydevice_exit);
MODULE_LICENSE("GPL");
