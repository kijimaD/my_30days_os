; Initial Program Loader
; hello-os
; TAB=4

; 標準的なFAT32フォーマットのための記述

    DB      0xeb, 0x4e, 0x90
    DB      "HELLOIPL"      ; ブートセレクタの名前(8byte)
    DW      512             ; 1セクタの大きさ
    DB      1               ; クラスタの大きさ
    DW      1               ; FATがどこから始まるか
    DB      2               ; FATの個数
    DW      224             ; ルートディレクトリ領域の大きさ
    DW      2880            ; このドライブの大きさ
    DB      0xf0            ; メディアタイプ
    DW      9               ; FAT領域の長さ
    DW      18              ; 1トラックにいくつのセクタがあるか
    DW      2               ; ヘッドの数
    DD      0               ; パーティションを使っていないのでここは必ず0
    DD      2880            ; このドライブの大きさをもう一度書く
    DB      0, 0, 0x29      ; よくわからないけどこの値にしておくといいらしい
    DD      0xffffffff      ; たぶんボリュームシリアル番号
    DB      "HELLO-OS   "   ; ディスクの名前(11byte)
    DB      "FAT12   "      ; フォーマットの名前(8byte)
    RESB    18              ; とりあえず18バイト空けておく。0x00で埋める

; Program Main Body

    DB 0xb8, 0x00, 0x00, 0x8e, 0xd0, 0xbc, 0x00, 0x7c
    DB 0x8e, 0xd8, 0x8e, 0xc0, 0xbe, 0x74, 0x7c, 0x8a
    DB 0x04, 0x83, 0xc6, 0x01, 0x3c, 0x00, 0x74, 0x09
    DB 0xb4, 0x0e, 0xbb, 0x0f, 0x00, 0xcd, 0x10, 0xeb
    DB 0xee, 0xf4, 0xeb, 0xfd

; Message

    DB      0x0a, 0x0a          ; 改行を2つ
    DB      "hello, world!"     ; DB命令は文字列を書ける
    DB      0x0a                ; 改行
    DB      0

    RESB    0x1fe-($-$$)        ; ドルマークは先頭から何バイト目かを示す

    DB      0x55, 0xaa          ; ブートセクタの最後の2バイト

; ブート以外の記述

    DB      0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
    RESB    4600
    DB      0xf0, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00
    RESB    1469432
