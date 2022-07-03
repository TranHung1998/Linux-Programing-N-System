#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gpio.h"

int main() {
    int ret = 0;
    int fd = -1;

    GPIOExport(31);
    GPIODirection(31, IN);
    
    GPIOExport(30);
    GPIODirection(30, OUT);
    GPIOWrite(30,LOW);

    char option = 'q';
    
    
    printf("\tCtrl + C (to quit the application)\n");
    while (1) {
        if( GPIORead(31) == HIGH ){
            if( GPIORead(30) == 1 )
                GPIOWrite(30,LOW);
            else
                GPIOWrite(30,HIGH);
        }
        sleep(1);
    };
}