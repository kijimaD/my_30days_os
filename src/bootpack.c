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
