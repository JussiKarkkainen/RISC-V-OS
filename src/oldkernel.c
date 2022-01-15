volatile unsigned char* uart = (unsigned char *)0x10000000;
volatile unsigned char* IER = (unsigned char *)0x10000000 + 1;
volatile unsigned char* FCR = (unsigned char *)0x10000000 + 2;
volatile unsigned char* LCR = (unsigned char *)0x10000000 + 3; 


void init_uart() {
  int set = (1 << 0) | (1 << 1);        // Set first and second bit of LCR to one
  *LCR |= set;                          // Sets word length to 8 bits 
  
  *FCR |= (1 << 0);                     // Set FIFO

  *IER |= (1 << 0);                     // Enable interrupts 
}
  

void putchar(char* str) {
  while (*str != '\0') {
    *uart = *str;
    str++;
  }
}

volatile unsigned char* LSR = (unsigned char *)0x10000000 + 5;

char getchar() {
  while (1) {
    if ((*LSR & 1) == 0) {
      continue;
    }
    else {
      return *uart;
    }
  }

}



void enter() {
  putchar("Working\n");
  init_uart();
  putchar("Hello World\n");
}




