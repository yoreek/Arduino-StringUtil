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
