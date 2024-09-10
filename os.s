.code16

jmp entry
.byte   0x90           # ブートセレクタの名前(8byte)
.ascii  "HELLOIPL"     # 1セクタの大きさ
.word   512            # クラスタの大きさ
.byte   1              # FATがどこから始まるか
.word   1              # FATの個数
.byte   2              # ルートディレクトリ領域の大きさ
.word   224            # このドライブの大きさ
.word   2880           # メディアタイプ
.byte   0xf0           # FAT領域の長さ
.word   9              # 1トラックにいくつのセクタがあるか
.word   18             # ヘッドの数
.word   2              # パーティションを使っていないのでここは必ず0
.int    0              # このドライブの大きさをもう一度書く
.int    2880           # よくわからないけどこの値にしておくといいらしい
.byte   0, 0, 0x29     # たぶんボリュームシリアル番号
.int    0xffffffff     # ディスクの名前(11byte)
.ascii  "HELLO-OS   "  # フォーマットの名前(8byte)
.ascii  "FAT12   "     # とりあえず18バイト空けておく。0x00で埋める
.skip   18, 0

entry:
        # init register
        movw $0, %ax
        movw %ax, %ss
        movw $0x7c00, %sp
        movw %ax, %ds

        # load disk
        movw $0x0820, %ax
        movw %ax, %es    # buffer address(ES:BX)
        movb $0, %ch     # cylinder 0
        movb $0, %dh     # head 0
        movb $2, %cl     # sector 2

        movb $0x02, %ah  # ah=0x02 read
        movb $1, %al     # 1 sector
        movw $0, %bx     # buffer address(ES:BX)
        movb $0x00, %dl  # drive A
        int  $0x13       # interrupt bios
        jc   error

fin:
        hlt
        jmp fin

error:
        movw $msg, %si

putloop:
        movb (%si), %al
        addw $1, %si
        cmpb $0, %al
        je fin
        movb $0x0e, %ah
        movw $15, %bx
        int $0x10
        jmp putloop

msg:
        .string "\nload error\n\n"

# end of boot sector
.org    0x01fe
.byte   0x55, 0xaa
