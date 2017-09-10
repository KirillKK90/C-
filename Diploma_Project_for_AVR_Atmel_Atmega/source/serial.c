#include <avr/io.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include "serial.h"

#if (UART_COUNT==1)
uint16_t udr_addrs[UART_COUNT] = {_SFR_ADDR(UDR0)};
uint16_t ucsra_addrs[UART_COUNT] = {_SFR_ADDR(UCSR0A)};
uint16_t ucsrb_addrs[UART_COUNT] = {_SFR_ADDR(UCSR0B)};
uint16_t ucsrc_addrs[UART_COUNT] = {_SFR_ADDR(UCSR0C)};
uint16_t ubrrl_addrs[UART_COUNT] = {_SFR_ADDR(UBRR0L)};
uint16_t ubrrh_addrs[UART_COUNT] = {_SFR_ADDR(UBRR0H)};
#ifndef USART0_RX_vect
#define USART0_RX_vect USART_RX_vect
#endif
#ifndef USART0_UDRE_vect
#define USART0_UDRE_vect USART_UDRE_vect
#endif
#elif (UART_COUNT==2)
uint16_t udr_addrs[UART_COUNT] = {_SFR_ADDR(UDR0),_SFR_ADDR(UDR1)};
uint16_t ucsra_addrs[UART_COUNT] = {_SFR_ADDR(UCSR0A),_SFR_ADDR(UCSR1A)};
uint16_t ucsrb_addrs[UART_COUNT] = {_SFR_ADDR(UCSR0B),_SFR_ADDR(UCSR1B)};
uint16_t ucsrc_addrs[UART_COUNT] = {_SFR_ADDR(UCSR0C),_SFR_ADDR(UCSR1C)};
uint16_t ubrrl_addrs[UART_COUNT] = {_SFR_ADDR(UBRR0L),_SFR_ADDR(UBRR1L)};
uint16_t ubrrh_addrs[UART_COUNT] = {_SFR_ADDR(UBRR0H),_SFR_ADDR(UBRR1H)};
#else
#error "Unsupported UART port count"
#endif

#define _UDR(n) _MMIO_BYTE(udr_addrs[n])
#define _UCSRA(n) _MMIO_BYTE(ucsra_addrs[n])
#define _UCSRB(n) _MMIO_BYTE(ucsrb_addrs[n])
#define _UCSRC(n) _MMIO_BYTE(ucsrc_addrs[n])
#define _UBRRL(n) _MMIO_BYTE(ubrrl_addrs[n])
#define _UBRRH(n) _MMIO_BYTE(ubrrh_addrs[n])

// Очереди на прием/передачу
u8 txqueue[UART_COUNT][TX_QUEUE_LEN], rxqueue[UART_COUNT][RX_QUEUE_LEN];
// Указатели на начало/конец очередей
volatile u8 tx_head[UART_COUNT], tx_tail[UART_COUNT], rx_head[UART_COUNT], rx_tail[UART_COUNT];

u16 rx_timeout[UART_COUNT];

// Передача
static inline void do_tx(u8 num) {
    	_UDR(num)=txqueue[num][tx_head[num]];
	if (++tx_head[num]==TX_QUEUE_LEN) tx_head[num]=0;
}

// Обработка прерывания о готовности буфера, порт 0
SIGNAL(USART0_UDRE_vect) {
    if (tx_head[0]==tx_tail[0]) {
		// Очередь закончилась,
		// отключаем прерывание Data Register Empty.
		// До следующего вызова putc (и пополнения очереди)
		// сие прерывание возникать не будет
		UCSR0B &= ~(1<<UDRIE0);
		return;
	}
    while (!(UCSR0A&0x20)); //на случай, если прерывание возникло случайно
    do_tx(0);
}

// Обработка прерывания о приеме, порт 0
SIGNAL(USART0_RX_vect) {
	PORTD = PORTD ^ (1<<6);
    // Закончить обработку, если нет данных в буфере
    if (!(UCSR0A&0x80)) return; //вдруг мышка бежала, хвостиком махнула...
    if ((rx_tail[0]==rx_head[0]-1)||((rx_head[0]==0)&&(rx_tail[0]==RX_QUEUE_LEN-1))) return; //очередь переполнена
    rxqueue[0][rx_tail[0]]=UDR0;
    if (++rx_tail[0]==RX_QUEUE_LEN) rx_tail[0]=0;
}

#if (UART_COUNT>=2)
// Обработка прерывания о готовности буфера, порт 1
SIGNAL(USART1_UDRE_vect) {
	if (tx_head[1]==tx_tail[1]) {
		// Очередь закончилась,
		// отключаем прерывание Data Register Empty.
		// До следующего вызова putc (и пополнения очереди)
		// сие прерывание возникать не будет
		UCSR1B &= ~(1<<UDRIE1);
		return;
	}
    while (!(UCSR1A&0x20)); //на случай, если прерывание возникло случайно
    do_tx(1);
}

// Обработка прерывания о приеме, порт 1
SIGNAL(USART1_RX_vect) {
    // Закончить обработку, если нет данных в буфере
    if (!(UCSR1A&0x80)) return; //вдруг мышка бежала, хвостиком махнула...
    if ((rx_tail[1]==rx_head[1]-1)||((rx_head[1]==0)&&(rx_tail[1]==RX_QUEUE_LEN-1))) return; //очередь переполнена
    rxqueue[1][rx_tail[1]]=UDR1;
    if (++rx_tail[1]==RX_QUEUE_LEN) rx_tail[1]=0;
}
#endif

//инициализация порта. Возвращает 0 при успехе или -1 при неудаче. Таймаут задаётся в мс.
char serial_init(unsigned char num, unsigned char opts, int baud, int timeout) {
	if (num>=UART_COUNT) return -1;

	cli();
	// cкорость
	_UBRRL(num) = (UART_CALC_BAUDRATE(baud) & 0xFF); // UART_BAUD_LOW
	_UBRRH(num) = (UART_CALC_BAUDRATE(baud)>>8) & 0xFF; // UART_BAUD_HIGH

	/* Enable receiver and transmitter */
	// И прерывания
	_UCSRB(num) = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0)|(1<<UDRIE0);

	// параметры
	// у &, оказывается, меньший приоритет, чем у ==
	_UCSRC(num) = 
	    // Parity
	    ( ((opts&0x0c)==SERIALPAR_E)?EVENPARITY:(((opts&0x0c)==SERIALPAR_O)?ODDPARITY:NOPARITY) )<<UPM00 |
	    // Stop bit
	    ( ((opts&0x30)==SERIALSTOP_2)?TWOSTOPBITS:(((opts&0x30)==SERIALSTOP_15)?ONE5STOPBITS:ONESTOPBIT) ) <<USBS0 |
	    // Character size
	    ( opts&3 ) << UCSZ00;

	rx_timeout[num] = timeout;
	tx_head[num]=tx_tail[num]=rx_head[num]=rx_tail[num]=0;	
	sei();
    return 0;
}

//передача байта. Возвращает -1 при неуспехе
s16 serial_putc(unsigned char num, unsigned char c) {

    // Если передан кривой порт --- вернуть -1
    if (num>=UART_COUNT) return -1;

    // Если очередь заполнена --- вернуть -1
    if ((tx_tail[num]==tx_head[num]-1)||((tx_head[num]==0)&&(tx_tail[num]==TX_QUEUE_LEN-1))) return -1;

    // Иначе добавить в очередь
    txqueue[num][tx_tail[num]]=c;
	cli();
    if ( ++tx_tail[num] == TX_QUEUE_LEN ) tx_tail[num]=0;
	sei();

	// Включаем прерывание Data Register Empty
	_UCSRB(num) |= (1<<UDRIE0);

    return 0;
}

//приём байта. 
s16 serial_getc(unsigned char num) {
	u16 count = 0;
    u8 ret;
    // Если передан кривой порт --- вернуть -1
    if (num>=UART_COUNT) return -1;

    while ( (rx_head[num]==rx_tail[num]) && (count++ < rx_timeout[num]) );
    if (count >= rx_timeout[num]) return -1;

    // Если очередь на порту пуста --- вернуть -1
    if (rx_head[num]==rx_tail[num]) return -1;

    ret=rxqueue[num][rx_head[num]];
	cli();
    if (++rx_head[num]==RX_QUEUE_LEN) rx_head[num]=0;
	sei();
    return ret;
}

void serial_close(unsigned char num) {
	cli();
	_UCSRB(num) = 0;
	tx_head[num]=tx_tail[num]=rx_head[num]=rx_tail[num]=0;	
	sei();
}
