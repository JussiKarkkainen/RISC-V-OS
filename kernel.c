typedef unsigned char uint8_t;

static volatile uint8_t *uart = (void *)0x10000000;


static int putchar(uint8_t ch) {
  static uint8_t THR = 0x00;
  static uint8_t LSR = 0x05;
  static uint8_t LSR_RI = 0x40;

  while ((uart[LSR] & LSR_RI) == 0);
  return uart[THR] = ch;
}

void puts(char *s) {
  while (*s) putchar(*s++);
  putchar('\n');
}

void enter() {
  puts("Start of an os");
}
