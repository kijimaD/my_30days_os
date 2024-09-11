.text
.code16

# ビデオモード
movb $0x13, %al
# ビデオモード設定
movb $0x00, %ah
# BIOS割り込みでビデオモードを変更する
int $0x10

fin:
        hlt
        jmp fin
