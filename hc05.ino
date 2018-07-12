#include <Arduino.h>

#define cmdPin 12
#define statePin 4

int _statePin,_bufsize;
void HC05_init(void)
{
    HC05_begin(9600);
    pinMode(cmdPin, OUTPUT);
    pinMode(statePin, INPUT);
}


int HC05_read()
{
    return HC05.read();
}

void HC05_begin(unsigned long baud)
{
    HC05.begin(baud);
}


bool HC05_connected()
{
    return(digitalRead(statePin)?true:false);
}
