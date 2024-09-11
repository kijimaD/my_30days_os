IPL_LINK_SCRIPT=ipl.lds
OS_LINK_SCRIPT=os.lds
BOOTPACK_LINK_SCRIPT=bootpack.lds

IPL_SRC=ipl.s
OS_SRC=asmhead.s

BOOTPACK_SRC=bootpack.c

TARGET_DIR=bin
IPL_BIN=$(TARGET_DIR)/ipl.bin
OS_BIN=$(TARGET_DIR)/asmhead.bin
BOOTPACK_BIN=$(TARGET_DIR)/bootpack.bin
SYSTEM_IMG=$(TARGET_DIR)/haribote.sys
TARGET_IMG=$(TARGET_DIR)/haribote.img

#debug
DEBUG_DIR=debug
LIST_IPL=$(DEBUG_DIR)/ipl.lst
LIST_OS=$(DEBUG_DIR)/os.lst

QEMU=qemu-system-x86_64

all: $(TARGET_IMG)

$(OS_BIN): $(OS_SRC) $(OS_LINK_SCRIPT)
	gcc -nostdlib -o $@ -T$(OS_LINK_SCRIPT) $(OS_SRC)
	gcc -T $(OS_LINK_SCRIPT) -c -g -Wa,-a,-ad $(OS_SRC) -o bin/os.o > $(LIST_OS)

$(IPL_BIN): $(IPL_SRC) $(IPL_LINK_SCRIPT)
	gcc -nostdlib -o $@ -T$(IPL_LINK_SCRIPT) $(IPL_SRC)
	gcc -T $(IPL_LINK_SCRIPT) -c -g -Wa,-a,-ad $(IPL_SRC) -o bin/ipl.o > $(LIST_IPL)

$(BOOTPACK_BIN): $(BOOTPAC_SRC)
	gcc -nostdlib -m32 -fno-pic -T $(BOOTPACK_LINK_SCRIPT) -o $@ $(BOOTPACK_SRC)

$(SYSTEM_IMG): $(OS_BIN) $(BOOTPACK_BIN)
	cat $(OS_BIN) $(BOOTPACK_BIN) > $@

# mformat: MS-DOS形式のディスクイメージを作成する。フロッピーディスクのイメージファイルにブートセクタを設定する
# -f: フロッピーディスクの容量
# -B: ブートセクタとして使用する。フロッピーディスクの先頭（ブートセクタ）にこのバイナリが書き込まれる
# -C: クラスタサイズを自動決定する
# -i: 操作対象のディスクイメージ
# ::  ドライブ指定
# mcopy: OSのバイナリをMS-DOS形式のフロッピーディスクイメージにコピーする
# 第一引数: コピーするファイル
# -i: 操作対象のディスクイメージ
$(TARGET_IMG): $(SYSTEM_IMG) $(IPL_BIN)
	mformat -f 1440 -B $(IPL_BIN) -C -i $(TARGET_IMG) ::
	mcopy $(SYSTEM_IMG) -i $(TARGET_IMG) ::

run: all
	$(QEMU) -m 32 -drive format=raw,file=$(TARGET_IMG),if=floppy

debug: all
	$(QEMU) -drive format=raw,file=$(TARGET_IMG),if=floppy -gdb tcp::10000 -S

clean:
	rm -rf $(TARGET_DIR)
