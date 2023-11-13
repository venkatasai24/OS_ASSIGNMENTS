savedcmd_/home/venkatasai24/OS/file.mod := printf '%s\n'   file.o | awk '!x[$$0]++ { print("/home/venkatasai24/OS/"$$0) }' > /home/venkatasai24/OS/file.mod
