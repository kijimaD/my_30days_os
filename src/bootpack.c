#include "bootpack.h"

void HariMain(void){
  char str[32] = {0};
  char mcursor[16 * 16];
  int mx;
  int my;
  int i;
  int j;

  /* 番地をセット */
  struct BOOTINFO *binfo = (struct BOOTINFO *) ADDR_BOOTINFO;

  init_gdtidt();
  init_pic();

  /* 割り込みを受け付ける */
  io_sti();

  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  _sprintf(str, "scrnx = %d", binfo->scrnx);
  putfonts8_asc(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, str);

  /* _sprintf(str, "(%d, %d)", mx, my); */
  /* putfonts8_asc(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, str); */

  putfonts8_asc(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "Haribote OS."); // 影
  putfonts8_asc(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "Haribote OS.");

  init_mouse_cursor8(mcursor, COL8_008484);
  mx = (binfo->scrnx - 16) / 2;
  my = (binfo->scrny - 28 - 16) / 2;
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  /* PIC1とキーボードを許可(11111001) */
  io_out8(PIC0_IMR, 0xf9);
  /* マウスを許可(11101111) */
  io_out8(PIC1_IMR, 0xef);

  for(;;) {
    /* 割り込み禁止 */
    io_cli();
    if (keybuf.next == 0) {
      /* 割り込み許可と停止 */
      /* HLT命令はもしPICから連絡があればCPUを目覚めさせてくれる */
      io_stihlt();
    } else {
      i = keybuf.data[0];
      keybuf.next--;
      for(j = 0; j < keybuf.next; j++) {
        keybuf.data[j] = keybuf.data[j + 1];
      }

      /* 割り込み許可 */
      io_sti();
      _sprintf(str, "%02X", i);

      boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
      putfonts8_asc(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, str);
    }
  }
}
