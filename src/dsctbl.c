#include "bootpack.h"

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
    /* access right */
    ar = ar | 0x8000;
    limit = limit / 0x1000;
  }

  /* セグメントの情報を、CPUの仕様にしたがって8バイトにまとめて書き込む */
  /* baseは3箇所に分断されているが、4バイトある */
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
