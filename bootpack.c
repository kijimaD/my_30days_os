void io_hlt(void);
void write_mem8(int addr, int data);

void HariMain(void){
  int i;

  for(i = 0xa0000; i <= 0xaffff; i++) {
    /* 縞模様 */
    /* 下位4ビットを残す。16画素ごとに色番号が繰り返される */
    write_mem8(i, i & 0x0f);
  }

  for(;;) {
    io_hlt();
  }
}
