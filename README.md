# OS_ASSIGNMENT_1

Here are the steps for building and installing a Linux kernel on Fedora Linux using version 6.5.1

**Download the Linux Kernel Source**

Obtain the latest stable Linux kernel source code from the official website [kernel.org](kernel.org "kernel.org")

**Extract the Kernel Source**

Use the tar command to extract the downloaded source code archive.

<pre>tar -xvf linux-6.5.1.tar</pre>	

**Navigate to the Kernel Source Directory**

Change the current directory to the extracted kernel source directory.

<pre>cd linux-6.5.1</pre>

**Configure the Kernel**

Use your existing kernel's configuration as a starting point.

<pre>cp -v /boot/config-$(uname -r) .config</pre>

**Install Required Development Tools**

Ensure you have the necessary development tools and libraries installed.

<pre>sudo dnf install ncurses-devel bison flex openssl-devel dwarves</pre>

**Configure the Kernel**

Customize the kernel configuration according to your requirements.

<pre>make menuconfig</pre>

In the menuconfig interface, you can enable or disable various kernel features and modules.

**Compile the Kernel**

Start the kernel compilation process using all available CPU cores.

<pre>make -j $(nproc)</pre>	

**Install Kernel Modules**

Install the newly built kernel modules.

<pre>sudo make modules_install</pre>

**Install the New Kernel**

Install the compiled kernel and its associated files.

<pre>sudo make install</pre>

**Update GRUB Configuration**

Update the GRUB bootloader configuration to include the new kernel.

<pre>sudo grub2-mkconfig -o /boot/grub2/grub.cfg</pre>

**Reboot Your System**

Reboot your computer to boot into the newly installed kernel.

<pre>sudo reboot</pre>	

After following these steps, your Fedora system should boot into the newly compiled and installed Linux kernel version 6.5.1
