void io_hlt(void);
void io_cli(void);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);

void HariMain(void){
  int i;
  char *p;

  init_palette();

  p = (char *)0xa0000;

  for(i = 0xa0000; i <= 0xaffff; i++) {
    /* 番地を代入する */
    p = (char *) i;
    /* 番地の値を代入する */
    /* 縞模様。下位4ビットを残す。16画素ごとに色番号が繰り返される */
    *p = i & 0x0f;
  }

  for(;;) {
    io_hlt();
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
