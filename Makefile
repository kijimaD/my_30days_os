run:
	gcc -nostdlib -T link.lds os.s -o os.img
	qemu-system-i386 os.img
