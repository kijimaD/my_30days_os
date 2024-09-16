#include "bootpack.h"

void HariMain(void){
  char str[32] = {0};
  char mcursor[16 * 16];
  int mx;
  int my;

  /* 番地をセット */
  struct BOOTINFO *binfo = (struct BOOTINFO *) 0x0ff0;

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  sprintf(str, "scrnx = %d", binfo->scrnx);
  putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, str);

  putfonts8_asc(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "ABC 123");
  putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "Haribote OS."); // 影
  putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "Haribote OS.");

  init_mouse_cursor8(mcursor, COL8_008484);
  mx = (binfo->scrnx - 16) / 2;
  my = (binfo->scrny - 28 - 16) / 2;
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  for(;;) {
    io_hlt();
  }
}

void init_mouse_cursor8(char *mouse, char bc) {
  int x;
  int y;

  static char cursor[16][16] = {
    "**************..",
    "*OOOOOOOOOOO*...",
    "*OOOOOOOOOO*....",
    "*OOOOOOOOO*.....",
    "*OOOOOOOO*......",
    "*OOOOOOO*.......",
    "*OOOOOOO*.......",
    "*OOOOOOOO*......",
    "*OOOO**OOO*.....",
    "*OOO*..*OOO*....",
    "*OO*....*OOO*...",
    "*O*......*OOO*..",
    "**........*OOO*.",
    "*..........*OOO*",
    "............*OO*",
    ".............***"
  };

  for(y = 0; y < 16; y++) {
    for(x = 0; x < 16; x++) {
      if(cursor[y][x] =='*') {
        mouse[y * 16 + x] = COL8_000000;
      }
      if(cursor[y][x] =='O') {
        mouse[y * 16 + x] = COL8_FFFFFF;
      }
      if(cursor[y][x] =='.') {
        mouse[y * 16 + x] = bc;
      }
    }
  }
}

void putblock8_8(char *vram, int vxsize, int pxsize, int pysize, int px0, int py0, char *buf, int bxsize) {
  int x;
  int y;

  for (y = 0; y < pysize; y++) {
    for (x = 0; x < pxsize; x++) {
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
    }
  }
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s) {
  extern char hankaku[4096];
  while(*s != '\0') {
    putfont8(vram, xsize, x, y, c, (hankaku + *s * 16));
    x += 8;
    s++;
  }
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font) {
  int i;
  char d;
  char *p;

  for(i = 0; i < 16; i++) {
    p = vram + (y + i) * xsize + x;
    d = font[i];

    if((d & 0x80) != 0) p[0] = c;
    if((d & 0x40) != 0) p[1] = c;
    if((d & 0x20) != 0) p[2] = c;
    if((d & 0x10) != 0) p[3] = c;
    if((d & 0x08) != 0) p[4] = c;
    if((d & 0x04) != 0) p[5] = c;
    if((d & 0x02) != 0) p[6] = c;
    if((d & 0x01) != 0) p[7] = c;
  }
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
    int x, y;
    for(y = y0; y <= y1; y++){
        for(x = x0; x <= x1; x++){
            vram[y * xsize + x] = c;
        }
    }
}

void init_palette(void) {
  /* DB命令にする */
  /* 0~255を明示する */
  static unsigned char table_rgb[16 * 3] = {
    0x00, 0x00, 0x00,
    0xff, 0x00, 0x00,
    0x00, 0xff, 0x00,
    0xff, 0xff, 0x00,
    0x00, 0x00, 0xff,
    0xff, 0x00, 0xff,
    0x00, 0xff, 0xff,
    0xff, 0xff, 0xff,
    0xc6, 0xc6, 0xc6,
    0x84, 0x00, 0x00,
    0x00, 0x84, 0x00,
    0x84, 0x84, 0x00,
    0x00, 0x00, 0x84,
    0x84, 0x00, 0x84,
    0x00, 0x84, 0x84,
    0x84, 0x84, 0x84
  };

  set_palette(0, 15, table_rgb);
  return;
}

void set_palette(int start, int end, unsigned char *rgb) {
  int i;
  int eflags;

  eflags = io_load_eflags();    /* 割り込み許可フラグの値を記録 */
  io_cli();                     /* 割り込み禁止 */
  io_out8(0x03c8, start);

  for(i = start; i <= end; i++) {
    /* RGBの順に0x03c9に書き込む */
    io_out8(0x03c9, rgb[0] / 4);
    io_out8(0x03c9, rgb[1] / 4);
    io_out8(0x03c9, rgb[2] / 4);
    rgb += 3;
  };

  io_store_eflags(eflags);      /* 割り込み許可フラグを戻す */
  return;
}

void init_screen(char *vram, int xsize, int ysize) {
  boxfill8(vram, xsize, COL8_008484, 0, 0, xsize - 1, ysize - 29);
  boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28);
  boxfill8(vram, xsize, COL8_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
  boxfill8(vram, xsize, COL8_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);

  boxfill8(vram, xsize, COL8_FFFFFF, 3, ysize - 24, 59, ysize - 24);
  boxfill8(vram, xsize, COL8_FFFFFF, 2, ysize - 24, 2, ysize - 4);
  boxfill8(vram, xsize, COL8_848484, 3, ysize - 4, 59, ysize - 4);
  boxfill8(vram, xsize, COL8_848484, 59, ysize - 23, 59, ysize - 5);
  boxfill8(vram, xsize, COL8_000000, 2, ysize - 3, 59, ysize - 3);
  boxfill8(vram, xsize, COL8_000000, 60, ysize - 24, 60, ysize - 3);

  boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize - 4, ysize - 24);
  boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize - 4);
  boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize - 3, xsize - 4, ysize - 3);
  boxfill8(vram, xsize, COL8_FFFFFF, xsize - 3, ysize - 24, xsize - 3, ysize - 3);
}

/* GDTはCPUのメモリ保護機能を設定するのに使う */
/* IDTはCPUの割り込み処理を設定するのに使う */
void init_gdtidt(void) {
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) 0x0026f800;
  int i;

  for (i = 0; i < 8092; i++) {
    /* gdtは8バイトの構造体へのポインタという宣言となっているので、gdtに1を足すと番地が8増える */
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  /* 全メモリを表しているセグメント */
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
  /* 512KBまでを表すセグメント。bootpackのためにある */
  set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
  load_gdtr(0xffff, 0x00270000);

  for (i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(0x7ff, 0x0026f800);
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar) {
  if(limit > 0xffffffff) {
    ar = ar | 0x8000;
    limit = limit / 0x1000;
  }

  sd->limit_low = limit & 0xffff;
  sd->base_low = base & 0xffff;
  sd->base_mid = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high = (base >> 24) & 0xff;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar) {
  gd->offset_low = offset & 0xffff;
  gd->selector = selector;
  gd->dw_count = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high = (offset >> 16) & 0xffff;
}
