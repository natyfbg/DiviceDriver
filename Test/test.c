// Name: Nathnael Gebre
// ID: 922065271
// CSC: 415-02 Operating Systems
// Github: natyfbg

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

// write a string, then read back the encrypted data
#define ENCRYPT _IO('a', 'a')
// write the encrypted string and read back the original message
#define DECRYPT _IO('a', 'b')
// write the key for encript the data
#define SET_KEY _IOW('a', 'b', int32_t *)

const char *str = "This is a non encrypted string";

int main(int argn, char *argv[])
{
    char buffer[1024];
    int ds;
    int key = 10;

    // open the device
    ds = open("/dev/my_device", O_RDWR);
    if (ds == -1)
    {
        fprintf(stderr, "Error opening the device: %s\n", strerror(errno));
        return 1;
    }

    // setting the key to 100
    if (ioctl(ds, SET_KEY, &key) == -1)
    {
        fprintf(stderr, "Error ioctl: %s\n", strerror(errno));
        return 1;
    }

    /* ************************************* Testing encryption mode *************************** */
    printf("Testing encrypt mode:\n");

    // set the device to encrypt the data
    if (ioctl(ds, ENCRYPT) == -1)
    {
        fprintf(stderr, "Error ioctl: %s\n", strerror(errno));
        return 1;
    }

    if (write(ds, str, strlen(str)) == -1)
    {
        fprintf(stderr, "Error writing the device: %s\n", strerror(errno));
        return 1;
    }

    // Write 0 to the buffer before to start writing inside it
    memset(buffer, 0x0, 1024);
    if (read(ds, buffer, 1024) == -1)
    {
        fprintf(stderr, "Error reading the device: %s\n", strerror(errno));
        return 1;
    }

    printf("The crypted version of the string is: %s\n", buffer);

    /* ************************************* Testing decryption mode *************************** */
    printf("Testing decrypt mode:\n");

    // setting the device to decrypt the data
    if (ioctl(ds, DECRYPT) == -1)
    {
        fprintf(stderr, "Error ioctl: %s\n", strerror(errno));
        return 1;
    }

    if (write(ds, buffer, strlen(buffer)) == -1)
    {
        fprintf(stderr, "Error writing the device: %s\n", strerror(errno));
        return 1;
    }

    // Write 0 to the buffer before to start writing inside it
    memset(buffer, 0x0, 1024);
    if (read(ds, buffer, 1024) == -1)
    {
        fprintf(stderr, "Error reading the device: %s\n", strerror(errno));
        return 1;
    }

    printf("The decrypted version of the string is: %s\n", buffer);

    return 0;
}
