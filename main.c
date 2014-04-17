/*
 * Controller code for a device that monitory the humidoty of a cigar storage
 * box. Check the README at Github for more information.
 *
 * (I might actually document this at some point in time.)
 *
 * Written by Gregor Ottmann, https://github.com/Zappes
 */
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lib/util.h"
#include "lib/uart.h"
#include "lib/am2302.h"
#include "lib/lcd-routines.h"

#define PORT_LED			PORTD
#define DIR_LED				DDRD
#define PIN_LED_WARN	PD6
#define PIN_LED_FAIL	PD7

// percentages for humidity. 67-74 is great, 62-79 will work, but it's not optimal
#define HUMIDITY_OK_LOW			670
#define HUMIDITY_OK_HIGH		740
#define HUMIDITY_WARN_LOW		620
#define HUMIDITY_WARN_HIGH	790

// when the humidity leaves a certain range (ok or warn) and reenters that range,
// it has to have a value of range_border +/- hysteresis in order to make the status
// change. example: the lower bound for OK is 670 (67.0%). the humidity drops to
// 66.9%, the status changes to "warn". for the status to switch back to "ok", the
// humidity now has to rise to 675 (67.5%). for too high humidity values, the
// hysteresis is applied inversly, i.e. after rising over the threshold, the value
// has to drop to threshold - hysteresis.
#define HYSTERESIS 3

#define STATUS_OK		0
#define STATUS_WARN 1
#define STATUS_FAIL	2
#define STATUS_WTF	4

#define STATUS_LOW	8
#define STATUS_HIGH	16

#define SYMBOL_OK					"Alles in Ordnung"
#define SYMBOL_WARN_LOW 	"Etwas zu trocken"
#define SYMBOL_WARN_HIGH 	"Etwas zu feucht"
#define SYMBOL_FAIL_LOW		"STAUBTROCKEN!"
#define SYMBOL_FAIL_HIGH	"KLATSCHNASS!"
#define SYMBOL_WTF				"Le WTF?"

void bt_init() {
	uart_init( UART_BAUD_SELECT(9600,F_CPU) );
}

void led_init() {
	DIR_LED		|= _BV(PIN_LED_FAIL) | _BV(PIN_LED_WARN);
	PORT_LED	&= ~(_BV(PIN_LED_FAIL) | _BV(PIN_LED_WARN));
}

void led_indicate_status(uint8_t status) {
	switch(status & 7) {
		case STATUS_WARN:
			PORT_LED	|= _BV(PIN_LED_WARN);
			PORT_LED	&= ~_BV(PIN_LED_FAIL);
			break;
		case STATUS_FAIL:
			PORT_LED	|= _BV(PIN_LED_FAIL);
			PORT_LED	&= ~_BV(PIN_LED_WARN);
			break;
		default:
			PORT_LED	&= ~(_BV(PIN_LED_FAIL) | _BV(PIN_LED_WARN));
	}
}

uint8_t humidity_status(uint16_t humidity) {
	uint8_t new_status = STATUS_WTF;
	static uint8_t old_status = STATUS_WTF;

	if(between(humidity, HUMIDITY_OK_LOW, HUMIDITY_OK_HIGH)) {
		if(old_status == STATUS_OK || old_status == STATUS_WTF || between(humidity, HUMIDITY_OK_LOW + HYSTERESIS, HUMIDITY_OK_HIGH - HYSTERESIS)) {
			new_status = STATUS_OK;
		} else {
			new_status = STATUS_WARN | (humidity < HUMIDITY_OK_LOW + HYSTERESIS ? STATUS_LOW : STATUS_HIGH);
		}
	} else if(between(humidity, HUMIDITY_WARN_LOW, HUMIDITY_WARN_HIGH)) {
		if(old_status == STATUS_OK || (old_status & STATUS_WARN)  || old_status == STATUS_WTF|| between(humidity, HUMIDITY_WARN_LOW + HYSTERESIS, HUMIDITY_WARN_HIGH - HYSTERESIS)) {
			new_status = STATUS_WARN | (humidity < HUMIDITY_OK_LOW ? STATUS_LOW : STATUS_HIGH);
		} else {
			new_status = STATUS_FAIL | (humidity < HUMIDITY_OK_LOW ? STATUS_LOW : STATUS_HIGH);
		}
	} else {
		new_status = STATUS_FAIL | (humidity < HUMIDITY_OK_LOW ? STATUS_LOW : STATUS_HIGH);
	}

	old_status = new_status;

	return new_status;
}

char* status_symbol(uint8_t status) {
	switch(status & 7) {
		case STATUS_OK:
			return SYMBOL_OK;
		case STATUS_WARN:
			return (status & STATUS_LOW) ? SYMBOL_WARN_LOW : SYMBOL_WARN_HIGH;
		case STATUS_FAIL:
			return (status & STATUS_LOW) ? SYMBOL_FAIL_LOW : SYMBOL_FAIL_HIGH;
		default:
			return SYMBOL_WTF;
	}
}

int main(void)
{
	led_init();
	bt_init();
	lcd_init();

	sei();

	lcd_setcursor(0,1);
	lcd_string("Humidorium v1.02");
	lcd_setcursor(0,2);
	lcd_string("(C) Zappes 2014");

	char buffer[32];
	uint8_t error;
	uint8_t status;
	uint16_t temp = 0;
	uint16_t humi = 0;

	while(1)
	{
		_delay_ms(2000);
		error = am2302(&humi, &temp);

		if(!error) {
			status = humidity_status(humi);

			sprintf(buffer, "%-16s", status_symbol(status));
			lcd_setcursor(0,1);
			lcd_string(buffer);

			sprintf(buffer, "%2i,%i%%      %2i,%iC", humi/10, humi%10, temp/10, temp%10);
			lcd_setcursor(0,2);
			lcd_string(buffer);

		}
		else {
			status = STATUS_WTF;
			sprintf(buffer, "%-16s", status_symbol(status));
			lcd_setcursor(0,1);
			lcd_string(buffer);

			sprintf(buffer, "Error:      %4d", error);
			lcd_setcursor(0,2);
			lcd_string(buffer);
		}

		led_indicate_status(status);

		sprintf(buffer, "%2i,%i%% %2i,%iC\r", humi/10, humi%10, temp/10, temp%10);
		uart_puts(buffer);
	}

	return 0;
}

