#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

#define UART_COUNT 1
//128
#define TX_QUEUE_LEN 2
#define RX_QUEUE_LEN 2

// ��������� opts
#define SERIALCLEN_5 0x00
#define SERIALCLEN_6 0x01
#define SERIALCLEN_7 0x02
#define SERIALCLEN_8 0x03
#define SERIALPAR_N 0x00
#define SERIALPAR_E 0x04
#define SERIALPAR_O 0x08
#define SERIALSTOP_1 0x00
#define SERIALSTOP_15 0x10
#define SERIALSTOP_2 0x20

#define UART_CALC_BAUDRATE(baudRate) ((uint32_t)(F_CPU) / ((uint32_t)(baudRate)*16) -1)
#define NOPARITY 0
#define EVENPARITY 3
#define ODDPARITY 2
#define TWOSTOPBITS 1
#define ONESTOPBIT 0
#define ONE5STOPBITS 1

//�������������
//void serial_init(void);
char serial_init(unsigned char num, unsigned char opts, int baud, int timeout);

//�������� �����. ���������� -1 ��� ��������
s16 serial_putc(unsigned char num, unsigned char c);

//���� �����.
s16 serial_getc(unsigned char num);

void serial_close(unsigned char num);

#endif
