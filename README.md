# DiviceDriver

Project Overview
This project consists of a Linux kernel module that serves as a device driver, providing encryption and decryption functionality via a device file (/dev/my_device). The driver supports custom IOCTL commands for setting the encryption mode, decryption mode, and encryption key. It's designed for educational purposes to demonstrate the creation and operation of a Linux device driver.

Features
Encryption and Decryption: Convert plain text to encrypted form and vice versa.
IOCTL Commands: ENCRYPT, DECRYPT, SET_KEY for controlling driver behavior.
User/Application Interaction: Write and read functionality through standard Linux file operations.
Kernel Integration: Loadable and unloadable as a module in the Linux kernel.
Prerequisites
Linux environment with kernel development tools installed.
Basic understanding of C programming and Linux device drivers.
Installation
Clone the Repository:

bash
Copy code
git clone https://github.com/natyfbg/File_System.git
Navigate to the Module Directory:
Ensure there are no spaces in the entire path to the directory.

Compile the Kernel Module:
Inside the Module directory, run:

go
Copy code
make
Loading the Device Driver
Insert the Module:

Copy code
sudo insmod mydevice.ko
Check dmesg to confirm successful loading.

Create a Device File (if not automatically created):

bash
Copy code
sudo mknod /dev/my_device c [major number] 0
Replace [major number] with the major number found in dmesg.

Using the Device Driver
Set Encryption Key:
Use IOCTL command SET_KEY with a user-defined key.

Encrypt a String:

Write a plain string to /dev/my_device.
Read back the encrypted string.
Decrypt a String:

Write an encrypted string to /dev/my_device.
Read back the decrypted string.
Unloading the Device Driver
Copy code
sudo rmmod mydevice
Check dmesg to confirm successful unloading.

Test Application
Located in the Test directory, this user-space application demonstrates interaction with the device driver. Follow the instructions in the Test directory to compile and run the test application.

Documentation and Screenshots
Detailed description of the device driver's functionality.
Instructions on building, loading, and interacting with the driver.
Screenshots showing all elements of functionality (loading, unloading, application interaction).
Issues and Resolutions
Compilation Errors: Resolved issues related to missing header files and undefined functions.
Incorrect Behavior: Adjusted encryption and decryption logic for expected results.
Memory Allocation Issues: Fixed memory leaks and implemented input validation.
Author
Nathnael Gebre
GitHub: natyfbg
CSC415-02 Operating Systems
License
This project is licensed under the GPL License - see the LICENSE file for details.

