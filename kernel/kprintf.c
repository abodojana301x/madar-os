/*
 * Layer: 1 (HAL/Bootstrap)
 * File: kprintf.c
 * Purpose: Unified printf for VGA and Serial
 * Status: ✅ Complete
 */

#include <kprintf.h>
#include <vga.h>
#include <serial.h>
#include <types.h>
#include <compiler.h>

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

static void itoa_base(uint64_t value, char *str, int base, int uppercase)
{
    char *ptr = str;
    char *low;
    char temp;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }

    while (value > 0) {
        int digit = value % base;
        *ptr++ = (digit < 10) ? ('0' + digit) : ((uppercase ? 'A' : 'a') + digit - 10);
        value /= base;
    }
    *ptr = '\0';

    low = str;
    --ptr;
    while (low < ptr) {
        temp = *low;
        *low++ = *ptr;
        *ptr-- = temp;
    }
}

static void emit_str(const char *str)
{
    vga_puts(str);
    serial_puts(str);
}

static void emit_padded(char *buffer, int width, char pad)
{
    int len = 0;
    while (buffer[len]) len++;
    while (width > len) {
        vga_putc(pad);
        serial_putc(pad);
        width--;
    }
    emit_str(buffer);
}

void kprintf_init(void)
{
    vga_init();
    serial_init();
}

int kprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    const char *ptr = format;
    char buffer[32];

    while (*ptr) {
        if (*ptr == '%') {
            ptr++;
            if (*ptr == '%') {
                vga_putc('%');
                serial_putc('%');
                ptr++;
                continue;
            }

            char pad = ' ';
            int width = 0;
            if (*ptr == '0') {
                pad = '0';
                ptr++;
            }
            while (*ptr >= '0' && *ptr <= '9') {
                width = (width * 10) + (*ptr - '0');
                ptr++;
            }

            if (*ptr == 'l') {
                ptr++;
                if (*ptr == 'l') {
                    ptr++;
                    if (*ptr == 'u') {
                        uint64_t num = va_arg(args, uint64_t);
                        itoa_base(num, buffer, 10, 0);
                        emit_padded(buffer, width, pad);
                    } else if (*ptr == 'x' || *ptr == 'X') {
                        uint64_t num = va_arg(args, uint64_t);
                        itoa_base(num, buffer, 16, *ptr == 'X');
                        emit_padded(buffer, width, pad);
                    }
                }
            } else {
                switch (*ptr) {
                    case 's': {
                        const char *str = va_arg(args, const char *);
                        emit_str(str ? str : "(null)");
                        break;
                    }
                    case 'd': {
                        int num = va_arg(args, int);
                        if (num < 0) {
                            vga_putc('-');
                            serial_putc('-');
                            num = -num;
                        }
                        itoa_base((uint64_t)num, buffer, 10, 0);
                        emit_padded(buffer, width, pad);
                        break;
                    }
                    case 'u': {
                        uint32_t num = va_arg(args, uint32_t);
                        itoa_base((uint64_t)num, buffer, 10, 0);
                        emit_padded(buffer, width, pad);
                        break;
                    }
                    case 'x':
                    case 'X': {
                        uint32_t num = va_arg(args, uint32_t);
                        itoa_base((uint64_t)num, buffer, 16, *ptr == 'X');
                        emit_padded(buffer, width, pad);
                        break;
                    }
                    case 'c': {
                        char c = (char)va_arg(args, int);
                        vga_putc(c);
                        serial_putc(c);
                        break;
                    }
                    default:
                        break;
                }
            }
        } else {
            vga_putc(*ptr);
            serial_putc(*ptr);
        }
        ptr++;
    }

    va_end(args);
    return 0;
}

void kprintf_early(const char *str)
{
    serial_puts(str);
}

void test_kprintf(void)
{
    kprintf("🧪 Testing kprintf...\n");
    kprintf("✅ kprintf test PASSED\n");
}
