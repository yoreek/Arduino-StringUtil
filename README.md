# Arduino string formatting library that support float data type [![Build Status](https://travis-ci.org/yoreek/Arduino-StringUtil.svg?branch=master)](https://travis-ci.org/yoreek/Arduino-StringUtil)

* Version: 1.0.3
* Release Date: 2018-10-24

## What is this repository for? ##

Due to some performance reasons %f is not included in the Arduino's implementation of sprintf().
This library contains a simplified implementation of string formatting functions that support float data type.

## How do I get set up? ##

 * [Download](https://github.com/yoreek/Arduino-StringUtil/archive/master.zip) the Latest release from gitHub.
 * Unzip and modify the Folder name to "StringUtil".
 * Paste the modified folder on your library folder (On your `Libraries` folder inside Sketchbooks or Arduino software).
 * Download and Install [Time library](https://github.com/PaulStoffregen/Time).
 * Restart the Arduino Software


## Usage ##

```
#include <Arduino.h>
#include <StringUtil.h>

using namespace StringUtil;

void setup() {
    Serial.begin(9600);
    while (!Serial);
}

void loop() {
    char buf[256];

    fprintf(Serial, "int=%d padint=%05d\r\n", 123, 123);
    fprintf(Serial, "long=%l padlong=%08l\r\n", 123567, 1234567);
    fprintf(Serial, "str=%s substr=%*s\r\n", "abcde", 2, "abcde");
    fprintf(Serial, "float=%f float2=%.2f float3=%05.2f\r\n", 12.123456, 12.123456, 12.123456);
    fprintf(Serial, "char=%c pointer=%p\r\n", 'A', &buf);
    fprintf(Serial, "date=%F time=%T\r\n");
    fprintf(Serial, "hex=%xd HEX=%Xd\r\n", 127, 127);
#ifdef F
    fprintf(Serial, F("flashString: int=%d padint=%05d\r\n"), 123, 123);
#endif
    sprintf(buf, F("buffer: int=%d padint=%05d\r\n"), 123, 123);
    Serial.println(buf);

    while (1);
}
```

Results:
```
int=123 padint=00123
long=123567 padlong=01234567
str=abcde substr=ab
float=12.123456 float2=12.12 float3=00012.12
char=A pointer=07FA
date=1970-00-00 time=00:00:00
hex=7f HEX=7F
flashString: int=123 padint=00123
buffer: int=123 padint=00123
```

## Library Reference ##

- `size_t fprintf(Print &out, const char *fmt, ...)`
- `size_t sprintf(char *str, const char *fmt, ...)`
- `size_t vfprintf(Print &out, const char *fmt, va_list &args)`
- `size_t vsprintf(char *str, const char *fmt, va_list &args)`
- `size_t fprintf(Print &out, const __FlashStringHelper *fmt, ...)`
- `size_t sprintf(char *str, const __FlashStringHelper *fmt, ...)`
- `size_t vfprintf(Print &out, const __FlashStringHelper *fmt, va_list &args)`
- `size_t vsprintf(char *str, const __FlashStringHelper *fmt, va_list &args)`

## Supported formats ##

This library does provide an almost complete implementation of the sprintf function known from the standard c library.

The following formats are supported:

* `%%`: literal percent sign

* `%c`: ASCII character represented by the given value

* `%[0][width][u][x|X]d`: signed/unsigned int number

* `%[0][width][u][x|X]l`: signed/unsigned long number

* `%[0][width][.width]f`: floating point value

* `%[0][width][u][x|X]z`: size_t datatype

* `%p`: void *

* `%s`: null-terminated string

* `%*s`: length and string

* `%T`: current time 'HH:MM:SS'

* `%F`: current date 'YYYY-MM-DD'


## External dependencies ##

This library depends on the external library Time, which is found [here](https://github.com/PaulStoffregen/Time).


## Example ##

Included on example folder, available on Arduino IDE.


## Version History ##

 * 1.0.3 (2018-10-25): Fix conflicts with Time.h

 * 1.0.2 (2016-01-11): Fix bug with '%Y'

 * 1.0.1 (2016-01-10): Add formats: %Y, %M, %D, %H, %m, %S

 * 1.0.0 (2016-01-02): Initial version.


## Who do I talk to? ##

 * [Yoreek](https://github.com/yoreek)
