#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void itoa(unsigned int n, char *buf) {
    int i;

    if (n < 10) {
        buf[0] = n + '0';
        buf[1] = '\0';
        return;
    }
    itoa(n / 10, buf);

    for (i = 0; buf[i] != '\0'; i++)
        ;

    buf[i] = (n % 10) + '0';

    buf[i + 1] = '\0';
}

void xtoa(unsigned int n, char *buf) {
    int i;

    if (n < 16) {
        if (n < 10) {
            buf[0] = n + '0';
        } else {
            buf[0] = n - 10 + 'a';
        }
        buf[1] = '\0';
        return;
    }
    xtoa(n / 16, buf);

    for (i = 0; buf[i] != '\0'; i++)
        ;

    if ((n % 16) < 10) {
        buf[i] = (n % 16) + '0';
    } else {
        buf[i] = (n % 16) - 10 + 'a';
    }
    buf[i + 1] = '\0';
}

int is_digit(unsigned char c) {
    if (c >= '0' && c <= '9')
        return 1;
    else
        return 0;
}

int is_letter(unsigned char c) {
    if (c >= 'a' && c <= 'z')
        return 1;
    else if (c >= 'A' && c <= 'Z')
        return 1;
    else
        return 0;
}

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int count = 0;
    char c;
    char *s;
    int n;

    char buf[65];
    char digit[16];

    memset(buf, 0, sizeof(buf));
    memset(digit, 0, sizeof(digit));

    while (*fmt != '\0') {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    n = va_arg(ap, int);
                    if (n < 0) {
                        putch('-');
                        n = -n;
                    }
                    itoa(n, buf);
                    printf(buf);
                    break;
                }
                case 'c': {
                    c = va_arg(ap, int);
                    putch(c);

                    break;
                }
                case 'x': {
                    n = va_arg(ap, int);
                    xtoa(n, buf);
                    printf(buf);
                    break;
                }
                case 's': {
                    s = va_arg(ap, char *);
                    printf(s);
                    break;
                }
                case '%': {
                    putch('%');
                    break;
                }
                default:
                    break;
            }
        } else {
            putch(*fmt);
        }
        fmt++;
    }

    va_end(ap);
    return count;
}


int vsnprintf(char *out, size_t _n, const char *fmt, va_list ap) {
    int count = 0;
    char c;
    char *s;
    int n;

    char *_out = out;

    char buf[65];
    char digit[16];

    memset(buf, 0, sizeof(buf));
    memset(digit, 0, sizeof(digit));

    while (*fmt != '\0' && (out - _out) < _n) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    n = va_arg(ap, int);
                    if (n < 0) {
                        *out = '-';
                        out++;
                        n = -n;
                    }
                    itoa(n, buf);
                    memcpy(out, buf, strlen(buf));
                    out += strlen(buf);
                    break;
                }
                case 'c': {
                    c = va_arg(ap, int);
                    *out = c;
                    out++;

                    break;
                }
                case 'x': {
                    n = va_arg(ap, int);
                    xtoa(n, buf);
                    memcpy(out, buf, strlen(buf));
                    out += strlen(buf);
                    break;
                }
                case 's': {
                    s = va_arg(ap, char *);
                    memcpy(out, s, strlen(s));
                    out += strlen(s);
                    break;
                }
                case '%': {
                    *out = '%';
                    out++;
                    break;
                }
                default:
                    break;
            }
        } else {
            *out = *fmt;
            out++;
        }
        fmt++;
    }
    *out = '\0';
    return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int count = 0;
    count = vsnprintf(out, n, fmt, ap);

    va_end(ap);

    return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int count = 0;
    count = vsprintf(out, fmt, ap);

    va_end(ap);

    return count;
}


#endif
