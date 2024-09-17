#include "bootpack.h"

/* GDTはCPUのメモリ保護機能を設定するのに使う */
/* IDTはCPUの割り込み処理を設定するのに使う */
void init_gdtidt(void) {
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADDR_GDT;
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) ADDR_IDT;
  int i;

  for (i = 0; i < 8092; i++) {
    /* gdtは8バイトの構造体へのポインタという宣言となっているので、gdtに1を足すと番地が8増える */
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  /* 全メモリを表しているセグメント */
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  /* 512KBまでを表すセグメント。bootpackのためにある */
  set_segmdesc(gdt + 2, LIMIT_BOTPAK, 0x00280000, AR_CODE32_ER);
  load_gdtr(LIMIT_GDT, ADDR_GDT);

  for (i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  load_idtr(LIMIT_IDT, ADDR_IDT);

  /* 2<<3はセグメント。セグメント番号2番。8倍するのは、セグメント番号の下位3ビットは別の意味があって、ここでは0にしておかないといけないから */
  /* AR_INTGATE32は、IDTに対する属性設定で、割り込み処理用の有効な設定ということを表す */
  set_gatedesc(idt + 0x21, (int)asm_inthandler21, 2 << 3, AR_INTGATE32);
  set_gatedesc(idt + 0x27, (int)asm_inthandler27, 2 << 3, AR_INTGATE32);
  set_gatedesc(idt + 0x2c, (int)asm_inthandler2c, 2 << 3, AR_INTGATE32);
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
