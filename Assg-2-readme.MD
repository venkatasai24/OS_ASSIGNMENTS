<style>
  body {
    font-family: "Arial", sans-serif;
  }
</style>

# Operating Systems Assignment-2

## Developer Information

- **Name:** Vedurupaka Venkata Sai
- **Roll No:** B210437CS
- **Batch:** CS02
- **Date:** 13-11-2023

Here are the steps for developing a Character Device Driver with specific functionalities after completing the device driver code and makefile :

## Compiling Driver

Compile the driver using the following command.

`make`

## Enter into Super Mode

Enter into the super user mode using the following command, to
enter the super user mode you must enter your password.

`su`

## Inserting the module

The module with specific parameters are given using following
command

`insmod filename.ko kernel_version=a,b time=c`

## Contents of driver

Displaying the contents of the character device driver is done by
the following command of the files present in /dev files.
Mostly it will be unreadable but in this case it will print Default.

`cat /dev/driver_directory`

## Writing current username

The following writes the current username (stored in the `$USER`
environment variable) followed by a space character to the file
located at dev/driver_directory.

`echo " $USER " > /dev/driver_directory`

## Removing module

The following command is used to remove a module (kernel
module) from the running kernel.

`rmmod filename`

## Extracting from Kernel

The dmesg command often logs events such as module loading
and unloading, hardware changes, and other kernel-related
activities.
The last 20 lines are displayed from the kernel buffer to know the status of our device driver.

`dmesg | tail -20`

After following these steps,you can see the status of your character device driver.
