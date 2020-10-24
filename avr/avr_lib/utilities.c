#include "utilities.h"
#include "../../common/TinyFrame.h"
#include "requests.h"
#include "uart.h"
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "lcd.h"

uint8_t (*funs[NUM_FUNS])(char **, char *) = {
    get_avr_name,
    get_avr_channels,
    set_name,
    set_channel_name,
    get_channel_value,
    set_channel_value,
    get_temperature,
};

char *funs_name[NUM_FUNS] = {
    "get_avr_name",
    "get_avr_channels",
    "set_name",
    "set_channel_name",
    "get_channel_value",
    "set_channel_value",
    "get_temperature",
};

char payload[TF_MAX_PAYLOAD_RX] = { 0 };
struct UART *uart = NULL;
TinyFrame *tf = NULL;

void TF_WriteImpl(TinyFrame *tf, const uint8_t *buff, uint32_t len) {

    send_response(buff, len);
}

TF_Result replyListener(TinyFrame *tf, TF_Msg *msg) {
    //lcd_clrscr();
    //lcd_puts("REPLYLIST");
    if (perform_request((char *)msg->data)) {
        msg->type = ERROR_MSG;
        msg->data = (uint8_t *)0x15;
    } else {
        msg->type = REPLY_MSG;
        msg->data = (uint8_t *)payload;
    }
    msg->len = (TF_LEN)(strlen((char *)msg->data) + 1);
    TF_Respond(tf, msg);

    memset(payload, 0, TF_MAX_PAYLOAD_RX);
    return TF_STAY;
}

void digital_out_init(void) {

    DDRB |= 0xff;
    DDRL |= 0xff;
}

void digital_in_init(void) {

    DDRC = 0;
}

void adc_init(void) {

    //AREF = AVcc
    ADMUX = (1 << REFS0);

    //set prescaler 128 and adc enable
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t ch) {

    //AND operation with 7
    ch &= 0b0000111;

    //clears the bottom 3 bit befor ORing
    ADMUX = (ADMUX & 0xf8) | ch;

    //adc start convertion
    ADCSRA |= (1 << ADSC);

    //wait for conversion complete
    while (ADCSRA & (1 << ADSC))
        ;

    return ADC;
}

void pwm_init(void) {

    TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << CS10);
    TCCR2A = (1 << WGM20) | (1 << COM2A1);
    TCCR2B = (1 << CS20);

    OCR2A = 0;
    OCR1A = 0;
    OCR1B = 0;
}

void uart_init(void) {
    uart = UART_init("uart_0", UART_BAUDRATE);
}

uint8_t get_request(char *buff) {

    buff[0] = 0;
    uint8_t idx = 0, data_len = 0;

    while (1) {

        uint8_t c = UART_getChar(uart);

        if (c == 0xff)
            return 0;

        //if(idx > 6)
            //lcd_putc(c);

        buff[idx] = c;

        if (idx == 3)
            data_len = c;

        idx++;
        buff[idx] = 0;

        if (idx == data_len + MSG_SIZE_NO_DATA) {
            //lcd_putc('\n');
            return idx;
        }

        _delay_ms(10);
    }
}

uint8_t perform_request(char *buffer) {
  
    char *token;
    char *params[NUM_ARGS] = { 0 };
    int idx = 0;

    token = strtok(buffer, " :\n");
    while (token != NULL) {
        params[idx++] = token;
        token = strtok(NULL, " :\n");
    }

    if (params[0] != NULL) {
        for (uint8_t i = 0; i < NUM_FUNS; i++) {
            if (strcmp(params[0], funs_name[i]) == 0) {
                return funs[i](params, payload);
            }
        }
    }

    return EXIT_FAILURE;
}

void send_response(const uint8_t *str, uint8_t len) {

    for (int i = 0; i < len; i++, str++) {
        UART_putChar(uart, *str);
    }
}

void handle_request(char *buff, int recvBytes) {

    if (tf == NULL) {
        tf = TF_Init(TF_SLAVE);
        TF_AddTypeListener(tf, REQUEST_MSG, replyListener);
    }
    _delay_ms(100);
    TF_Accept(tf, (uint8_t *)buff, recvBytes);
}

void detect_motion(void) {

    //motion detected
    if (((PINC) & (1 << MOTION_SENS_PIN)) == 1) {
        lcd_clrscr();
        lcd_puts("MOVIMENTO\n RILEVATO\n");
        for (int8_t i = 0; i < 4; i++) {
            //turn on buzzer alarm - motion
            PORTL |= (1 << MOTION_BUZZER_PIN);
            PORTB |= (1 << MOTION_LED_PIN);
            _delay_ms(100);
            //turn off buzzer alarm - motion
            PORTL &= ~(1 << MOTION_BUZZER_PIN);
            PORTB &= ~(1 << MOTION_LED_PIN);
            _delay_ms(100);
        }
        lcd_clrscr();
    }
}

void detect_fire(void) {

    //fire detected
    if (((PINC) & (1 << FIRE_SENS_PIN)) == 0) {

        lcd_clrscr();
        lcd_puts("ALLARME INCENDIO");
        for (int8_t j = 0; j < 4; j++) {
            //turn on buzzer/led alarm - fire
            PORTL |= (1 << FIRE_BUZZER_PIN);
            PORTB |= (1 << FIRE_LED_PIN);
            _delay_ms(100);
            //turn off buzzer/led alarm - fire
            PORTL &= ~(1 << FIRE_BUZZER_PIN);
            PORTB &= ~(1 << FIRE_LED_PIN);
            _delay_ms(100);
        }
        lcd_clrscr();
    }
}