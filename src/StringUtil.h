#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

extern "C++" { namespace StringUtil {

class StringReader {
    public:
        StringReader(const char *str);
        void reset(void);
        virtual char read();
        bool eof() { return _eof; };

    protected:
        const char *_cur;
        const char *_start;
        bool        _eof;
};

#ifdef F
class FlashStringReader: public StringReader {
    public:
        FlashStringReader(const __FlashStringHelper *str)
                          : StringReader((const char *) str) {};
        virtual char read();
};
#endif

class StringWriter : public Print {
    public:
        StringWriter(char *str);
        virtual size_t write(uint8_t c);

    protected:
        char   *_str;
};

size_t fprintf(Print &out, const char *fmt, ...);
size_t sprintf(char *str, const char *fmt, ...);
size_t vfprintf(Print &out, const char *fmt, va_list &args);
size_t vsprintf(char *str, const char *fmt, va_list &args);

#ifdef F
size_t fprintf(Print &out, const __FlashStringHelper *fmt, ...);
size_t sprintf(char *str, const __FlashStringHelper *fmt, ...);
size_t vfprintf(Print &out, const __FlashStringHelper *fmt, va_list &args);
size_t vsprintf(char *str, const __FlashStringHelper *fmt, va_list &args);
#endif

}} // namespace

#endif // STRING_UTIL_H
