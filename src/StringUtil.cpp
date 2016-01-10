#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <stdarg.h>
#include "StringUtil.h"

#define USE_TIME

#ifdef USE_TIME
#include <Time.h>
#endif

#ifndef __NUM_MAX_LEN__
#define __NUM_MAX_LEN__ (sizeof("-2147483648") - 1)
#endif

#ifndef __PTR_SIZE__
#define __PTR_SIZE__ (sizeof(void *))
#endif

extern "C++" { namespace StringUtil {

static const char *lcHex = "0123456789abcdef";
static const char *ucHex = "0123456789ABCDEF";

#ifdef USE_TIME
static time_t cacheTime;
#endif

StringReader::StringReader(const char *str) {
    _start = _cur = str;
    _eof   = false;
}

char StringReader::read() {
    char c = '\0';

    if (!_eof) {
        c = *_cur++;

        if (c == '\0') {
            _eof = true;
        }
    }

    return c;
}

#ifdef F
char FlashStringReader::read() {
    char c = '\0';

    if (!_eof) {
        c = pgm_read_byte(_cur++);

        if (c == '\0') {
            _eof = true;
        }
    }

    return c;
}
#endif

StringWriter::StringWriter(char *str) {
    _str   = str;
}

size_t StringWriter::write(uint8_t c) {
    *_str++ = c;
    *_str   = '\0';

    return 1;
}

static size_t printNum(Print &out, uint32_t num, char zero, const char *hex, unsigned int width) {
    char   *p, temp[__NUM_MAX_LEN__ + 1];
    size_t  len, n = 0;

    p = temp + __NUM_MAX_LEN__;

    if (hex == NULL) {
        do {
            *--p = (char) (num % 10 + '0');
        } while (num /= 10);
    }
    else {
        do {

            *--p = hex[(num & 0xf)];

        } while (num >>= 4);

    }

    /* zero or space padding */
    len = (temp + __NUM_MAX_LEN__) - p;
    while (len++ < width) {
        n += out.write(zero);
    }

    /* number safe copy */
    len = (temp + __NUM_MAX_LEN__) - p;
    while (len--) {
        n += out.write(*p++);
    }

    return n;
}

static size_t printOneArg(Print &out, StringReader &fmt, va_list &args) {
    size_t        slen, n = 0;
    char          c, zero, *p;
    unsigned long width, sign, fracWidth, scale, i;
    int32_t       i32;
    uint32_t      ui32, frac;
    double        f;
    const char   *hex;

    if ((c = fmt.read()) == '\0')
        goto ERROR;

    zero       = ((c == '0') ? '0' : ' ');
    width      = 0;
    sign       = 1;
    hex        = NULL;
    fracWidth  = 6;
    slen       = (size_t) -1;

    while (c >= '0' && c <= '9') {
        width = width * 10 + c - '0';
        if ((c = fmt.read()) == '\0')
            goto ERROR;
    }

    for ( ;; ) {
        switch (c) {
            case 'u':
                sign = 0;
                c = fmt.read();
                continue;
            case 'X':
                hex = ucHex;
                sign = 0;
                c = fmt.read();
                continue;
            case 'x':
                hex = lcHex;
                sign = 0;
                c = fmt.read();
                continue;
            case '.':
                fracWidth = 0;
                while ((c = fmt.read()) != '\0' && c >= '0' && c <= '9') {
                    fracWidth = fracWidth * 10 + c - '0';
                }
                break;
            case '*':
                slen = va_arg(args, size_t);
                c = fmt.read();
                continue;
            default:
                break;
        }
        break;
    }

    switch (c) {
        case 's':
            p = va_arg(args, char *);
            if (slen == (size_t) -1) {
                while (*p && out.write(*p++)) {
                    n++;
                }
            }
            else {
                while (slen-- && *p && out.write(*p++)) {
                    n++;
                }
            }
            goto FINISH;
        case 'z':
            if (sign) {
                i32 = (int32_t) va_arg(args, size_t);
            } else {
                ui32 = (uint32_t) va_arg(args, size_t);
            }
            break;
        case 'i':
            if (sign) {
                i32 = (int32_t) va_arg(args, int);
            } else {
                ui32 = (uint32_t) va_arg(args, unsigned int);
            }
            break;
        case 'd':
            if (sign) {
                i32 = (int32_t) va_arg(args, int);
            } else {
                ui32 = (uint32_t) va_arg(args, unsigned int);
            }
            break;
        case 'l':
            if (sign) {
                i32 = (int32_t) va_arg(args, long);
            } else {
                ui32 = (uint32_t) va_arg(args, unsigned long);
            }
            break;
        case 'f':
            f = va_arg(args, double);

            if (f < 0) {
                out.write('-');
                f = -f;
            }

            ui32 = (int32_t) f;
            frac = 0;

            if (fracWidth) {
                scale = 1;
                for (i = fracWidth; i; i--) {
                    scale *= 10;
                }

                frac = (uint32_t) ((f - (double) ui32) * scale + 0.5);

                if (frac == scale) {
                    ui32++;
                    frac = 0;
                }
            }

            n += printNum(out, ui32, zero, 0, width);

            if (fracWidth) {
                out.write('.');

                n += printNum(out, frac, '0', 0, fracWidth);
            }
            goto FINISH;
        case 'p':
            ui32 = (uint32_t) va_arg(args, void *);
            hex = ucHex;
            sign = 0;
            zero = '0';
            width = __PTR_SIZE__ * 2;
            break;
        case 'c':
            ui32 = (uint32_t) va_arg(args, int);
            n += out.write((char) (ui32 & 0xff));
            goto FINISH;
        case '%':
            n += out.write('%');
            goto FINISH;
#ifdef USE_TIME
        case 'F':
            // YYYY-MM-DD
            n += printNum(out, year(cacheTime), '0', 0, 4);
            n += out.write('-');
            n += printNum(out, month(cacheTime), '0', 0, 2);
            n += out.write('-');
            n += printNum(out, day(cacheTime), '0', 0, 2);
            goto FINISH;
        case 'T':
            // HH:MM:SS
            n += printNum(out, hour(cacheTime), '0', 0, 2);
            n += out.write(':');
            n += printNum(out, minute(cacheTime), '0', 0, 2);
            n += out.write(':');
            n += printNum(out, second(cacheTime), '0', 0, 2);
            goto FINISH;
        case 'Y':
            // YYYY
            n += printNum(out, year(cacheTime), '0', 0, 4);
        case 'M':
            // MM
            n += printNum(out, month(cacheTime), '0', 0, 2);
            goto FINISH;
        case 'D':
            // DD
            n += printNum(out, day(cacheTime), '0', 0, 2);
            goto FINISH;
        case 'H':
            // HH
            n += printNum(out, hour(cacheTime), '0', 0, 2);
            goto FINISH;
        case 'm':
            // MM
            n += printNum(out, minute(cacheTime), '0', 0, 2);
            goto FINISH;
        case 'S':
            // SS
            n += printNum(out, second(cacheTime), '0', 0, 2);
            goto FINISH;
#endif
        default:
            if (c != '\0') {
                n += out.write(c);
            }
            goto FINISH;
    }

    if (sign) {
        if (i32 < 0) {
            out.write('-');
            ui32 = (uint32_t) -i32;
        }
        else {
            ui32 = (uint32_t) i32;
        }
    }

    n += printNum(out, ui32, zero, hex, width);

FINISH:
    return n;
ERROR:
    return 0;
}

static size_t printf(Print &out, StringReader &fmt, va_list &args) {
    char   c;
    size_t n = 0;

#ifdef USE_TIME
    cacheTime = now();
#endif
    while ((c = fmt.read()) != '\0') {
        if (c == '%') {
            n += printOneArg(out, fmt, args);
        }
        else {
            n += out.write(c);
        }
    }

    return n;
}

size_t fprintf(Print &out, const char *fmt, ...) {
    StringReader      _fmt(fmt);
    va_list           args;
    size_t            n;

    va_start(args, fmt);
    n = printf(out, _fmt, args);
    va_end(args);

    return n;
}

size_t vfprintf(Print &out, const char *fmt, va_list &args) {
    StringReader      _fmt(fmt);

    return printf(out, _fmt, args);
}

size_t sprintf(char *str, const char *fmt, ...) {
    StringWriter      _out(str);
    StringReader      _fmt(fmt);
    va_list           args;
    size_t            n;

    va_start(args, fmt);
    n = printf(_out, _fmt, args);
    va_end(args);

    return n;
}

size_t vsprintf(char *str, const char *fmt, va_list &args) {
    StringWriter      _out(str);
    StringReader      _fmt(fmt);

    return printf(_out, _fmt, args);
}

#ifdef F
size_t fprintf(Print &out, const __FlashStringHelper *fmt, ...) {
    FlashStringReader _fmt(fmt);
    va_list           args;
    size_t            n;

    va_start(args, fmt);
    n = printf(out, _fmt, args);
    va_end(args);

    return n;
}

size_t vfprintf(Print &out, const __FlashStringHelper *fmt, va_list &args) {
    FlashStringReader _fmt(fmt);

    return printf(out, _fmt, args);
}

size_t sprintf(char *str, const __FlashStringHelper *fmt, ...) {
    StringWriter      _out(str);
    FlashStringReader _fmt(fmt);
    va_list           args;
    size_t            n;

    va_start(args, fmt);
    n = printf(_out, _fmt, args);
    va_end(args);

    return n;
}

size_t vsprintf(char *str, const __FlashStringHelper *fmt, va_list &args) {
    StringWriter      _out(str);
    FlashStringReader _fmt(fmt);

    return printf(_out, _fmt, args);
}
#endif

}} // namespace
