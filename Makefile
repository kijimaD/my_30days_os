# commands ================
CC = gcc
CFLAGS = -nostdlib -m32 -fno-pic
INCLUDE = -I include
LD = ld
LFLAGS = -m elf_i386
QEMU = qemu-system-x86_64

# files ================
TARGET_DIR = bin
LST_DIR = $(TARGET_DIR)/lst
TMP_DIR = $(TARGET_DIR)/tmp

OS_SRC_DIR = src
OS_SRC=$(wildcard $(OS_SRC_DIR)/*.c)
OS_LS = scripts/bootpack.lds
OS = $(TARGET_DIR)/os.bin

OS_ENTRY_POINT = HariMain

SYSTEM_IMG = bin/haribote.bin

ASMLIB_SRC = src/asm/asm_func.s
ASMLIB = $(TARGET_DIR)/asm_func.o

BINLIB = lib/hankaku.o

IPL_SRC = src/asm/boot/ipl.s
IPL_LS = scripts/ipl.lds
IPL = $(TARGET_DIR)/ipl.bin

OSL_SRC = src/asm/boot/asmhead.s
OSL_LS = scripts/asmhead.lds
OSL = $(TARGET_DIR)/asmhead.bin

IMG = $(TARGET_DIR)/haribote.img

# tasks ================
all: $(IMG)

# mformat: MS-DOS形式のディスクイメージを作成する。フロッピーディスクのイメージファイルにブートセクタを設定する
# -f: フロッピーディスクの容量
# -B: ブートセクタとして使用する。フロッピーディスクの先頭（ブートセクタ）にこのバイナリが書き込まれる
# -C: クラスタサイズを自動決定する
# -i: 操作対象のディスクイメージ
# ::  ドライブ指定
# mcopy: OSのバイナリをMS-DOS形式のフロッピーディスクイメージにコピーする
# 第一引数: コピーするファイル
# -i: 操作対象のディスクイメージ
$(IMG): $(IPL) $(OSL) $(OS)
	cat $(OSL) $(OS) > $(SYSTEM_IMG)
	mformat -f 1440 -B $(IPL) -C -i $(IMG) ::
	mcopy $(SYSTEM_IMG) -i $(IMG) ::

$(OS): $(addprefix $(TARGET_DIR)/, $(notdir $(OS_SRC:.c=.o))) $(ASMLIB) $(BINLIB)
	ld $(LFLAGS) -o $@ -T $(OS_LS) -e $(OS_ENTRY_POINT) --oformat=binary $^

$(ASMLIB): $(ASMLIB_SRC)
	$(CC) $(CFLAGS) -c -g -Wa,-a,-ad $< -o $@ > $(addprefix $(LST_DIR)/, $(notdir $(@F:.o=.lst)))

$(TARGET_DIR)/%.o : $(OS_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -nostdlib -m32 -c -o $@ $<

$(IPL): $(IPL_SRC)
	mkdir -p $(TARGET_DIR)
	mkdir -p $(LST_DIR)
	mkdir -p $(TMP_DIR)
	gcc $(CFLAGS) -o $@ -T$(IPL_LS) $(IPL_SRC)

$(OSL): $(OSL_SRC)
	$(CC) $(CFLAGS) -o $@ -T $(OSL_LS) $(OSL_SRC)
	$(CC) $(CFLAGS) -o $(addprefix $(TMP_DIR)/, $(notdir $(@F:.s=.o))) -T $(OSL_LS) -c -g -Wa,-a,-ad $(OSL_SRC) > $(addprefix $(LST_DIR)/, $(notdir $(@F:.bin=.lst)))

run: all
	$(QEMU) -m 32 -drive format=raw,file=$(IMG),if=floppy

debug: all
	$(QEMU) -drive format=raw,file=$(IMG),if=floppy -gdb tcp::10000 -S

clean:
	rm -rf $(TARGET_DIR)/*
	touch $(TARGET_DIR)/.gitkeep
