savedcmd_/home/venkatasai24/OS/file.ko := ld -r -m elf_x86_64 -z noexecstack --build-id=sha1  -T scripts/module.lds -o /home/venkatasai24/OS/file.ko /home/venkatasai24/OS/file.o /home/venkatasai24/OS/file.mod.o;  make -f ./arch/x86/Makefile.postlink /home/venkatasai24/OS/file.ko