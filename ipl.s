.text
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

# シリンダ数
.set CYLS, 10
# シリンダ数のメモリ位置
.set _CYLS, 0xff0

entry:
        # init register
        movw $0, %ax
        movw %ax, %ss
        movw $0x7c00, %sp # メモリ上のブートセクタ開始アドレス
        movw %ax, %ds     # DSはデータセグメント

        # load disk
        movw $0x0820, %ax
        movw %ax, %es     # buffer address(ES:BX)
        movb $0, %ch      # cylinder 0
        movb $0, %dh      # head 0
        movb $2, %cl      # sector 2

readloop:
        movw $0, %si # retry counter

retry:
        movb $0x02, %ah   # ah=0x02 read
        movb $1, %al      # 1 sector
        movw $0, %bx      # buffer address(ES:BX)
        movb $0x00, %dl   # drive A
        int  $0x13        # interrupt bios
        jnc  next         # エラーがなければ次へ

        addw $1, %si      # エラーカウントをカウントアップ
        cmp  $5, %si
        jae  error        # エラーカウント5回でエラー処理へ

        # 再実行前にドライブ情報をリセット(drive A)
        movb $0x00, %ah
        movb $0x00, %dl   # drive A
        int  $0x13

        jmp retry

next:
        movw %es, %ax
        # 512 / 16 = 0x20
        # 対象のアドレスは(ES x 16 + BX)でキマるので、ESを0x20ずらすと、512byte分(1セクタ)ずらしたのとおなじになる
        add $0x20, %ax
        movw %ax, %es
        add $1, %cl
        cmp $18, %cl # セクター18まで読み込む
        jbe readloop # 以下ならループ

        movb $1, %cl
        add $1, %dh
        cmp $2, %dh
        jb readloop # より下ならループ

        movb $0, %dh
        addb $1, %ch
        cmp $CYLS, %ch
        jb readloop # より下ならループ

        movb $CYLS, (_CYLS)
        # jump to os program
        # 0x0800(buffer address) + 0x4200(first file place of FAT12)
        # セクタ2から先をメモリアドレス0x0820を先頭としてロード(0x0800-0x0820の512byteはIPL分としてスキップ)
        # ディスクイメージ上0x4200の位置にあるOSのプログラムは、0x0800 + 0x4200 = 0xC200にロードされる
        jmp 0xc200

fin:
        hlt               # CPUを停止
        jmp fin           # 無限ループさせる

error:
        movw $msg, %si    # msgのアドレスをロードする

putloop:
        movb (%si), %al
        addw $1, %si
        cmpb $0, %al
        je fin
        movb $0x0e, %ah
        movw $15, %bx
        int $0x10         # BIOSの文字出力割り込み
        jmp putloop       # 次の文字

msg:
        .string "\nload error!\n\n"

# end of boot sector
.org    0x01fe            # ここまでのバイナリサイズを510バイトに揃える
.byte   0x55, 0xaa        # ブートセクタのシグネチャ
