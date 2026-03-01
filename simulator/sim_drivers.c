/**
 * @file sim_drivers.c
 * @brief PC simulator stubs for Raspberry Pi 4 hardware drivers
 *
 * Provides host-native (x86/x86_64) implementations of:
 *   - UART communication  (uart_comm.h) → stdout / stdin
 *   - GPIO / LED          (uart_comm.h) → console messages
 *   - HDMI Framebuffer    (framebuffer.h) → ANSI terminal output
 *
 * Compiled together with app/main.c and simulator/sim_os.c by
 * build_sim.sh / build_sim.bat instead of the hardware-specific
 * uart_comm.c and framebuffer.c.
 */

#include <stdio.h>
#include <stdint.h>

#include "uart_comm.h"
#include "framebuffer.h"

/* ------------------------------------------------------------------ */
/*  UART Simulation                                                    */
/* ------------------------------------------------------------------ */

void uart_init(uint32_t baudrate)
{
    (void)baudrate;
    /* No hardware initialisation on the host */
}

void uart_putc(char c)
{
    putchar((unsigned char)c);
    fflush(stdout);
}

char uart_getc(void)
{
    int c = getchar();
    return (char)(c == EOF ? '\0' : c);
}

void uart_puts(const char *str)
{
    if (!str) return;
    fputs(str, stdout);
    fflush(stdout);
}

uint8_t uart_available(void)
{
    return 0; /* No asynchronous input simulation */
}

void uart_put_hex(uint32_t value)
{
    printf("0x%08X", value);
    fflush(stdout);
}

void uart_put_dec(uint32_t value)
{
    printf("%u", value);
    fflush(stdout);
}

/* ------------------------------------------------------------------ */
/*  GPIO / LED Simulation                                              */
/* ------------------------------------------------------------------ */

void gpio_init(void)
{
    printf("[SIM] GPIO initialized (LED on GPIO17)\n");
    fflush(stdout);
}

void gpio_led_toggle(void)
{
    static uint8_t state = 0;
    state = !state;
    printf("[SIM] LED %s\n", state ? "ON " : "OFF");
    fflush(stdout);
}

void gpio_led_set(uint8_t on)
{
    printf("[SIM] LED %s\n", on ? "ON " : "OFF");
    fflush(stdout);
}

void delay_ms(uint32_t ms)
{
    /* In the simulator the OS scheduler provides real timing;
       busy-wait delays are not needed here. */
    (void)ms;
}

/* ------------------------------------------------------------------ */
/*  Framebuffer / HDMI Simulation                                      */
/* ------------------------------------------------------------------ */

/* Simple ANSI cursor tracking */
static uint32_t sim_cur_x = 0;
static uint32_t sim_cur_y = 0;

int fb_init(void)
{
    printf("[SIM] Framebuffer initialized (1024x768 @ 32bpp)\n");
    fflush(stdout);
    return 0;
}

void fb_putc(char c)
{
    putchar((unsigned char)c);
    fflush(stdout);
}

void fb_puts(const char *str)
{
    if (!str) return;
    fputs(str, stdout);
    fflush(stdout);
}

void fb_put_hex(uint32_t value)
{
    printf("0x%08X", value);
    fflush(stdout);
}

void fb_put_dec(uint32_t value)
{
    printf("%u", value);
    fflush(stdout);
}

void fb_clear(void)
{
    /* ANSI: clear screen and move cursor to home */
    printf("\033[2J\033[H");
    fflush(stdout);
    sim_cur_x = 0;
    sim_cur_y = 0;
}

void fb_clear_line(void)
{
    /* ANSI: erase to end of line, then carriage-return */
    printf("\033[K");
    fflush(stdout);
}

void fb_set_cursor(uint32_t x, uint32_t y)
{
    sim_cur_x = x;
    sim_cur_y = y;
    /* ANSI row/col are 1-based */
    printf("\033[%u;%uH", y + 1, x + 1);
    fflush(stdout);
}

void fb_set_color(uint8_t r, uint8_t g, uint8_t b)
{
    /* Color not simulated on the host terminal */
    (void)r; (void)g; (void)b;
}
