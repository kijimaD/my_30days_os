.arch i486
.text

# void io_hlt(void)
.global io_hlt
# void write_mem8(int addr, int data)
.global write_mem8

io_hlt:
        hlt
        ret

# C言語から呼び出す場合に使えるのはEAX, ECX, EDXの3つだけ
write_mem8:
        # ESP+4にaddrが入っているのでECXに読み込む
        movl 4(%esp), %ecx
        # ESP+8にdataが入っているのでALに読み込む
        movb 8(%esp), %al
        mov %al, (%ecx)
        ret
