#include "mbed.h"

BufferedSerial pc(P1_7, P1_6, 9600);  //TX, RX
SPI spi(P0_9, P0_8, P0_6);    //mosi, miso, sclk
DigitalOut cs(P0_7);

//uart read
uint8_t read();
const char delimiter='\r';
const uint8_t buf_size=10;
char buf[buf_size];
uint8_t number;

//MCLK freq.
const uint32_t mclk=25000000;
const uint32_t max_freq=3000000;
const uint64_t res=268435456;   //2^28
uint32_t freq, freq_reg;
uint16_t lsb,msb;
uint8_t i,k;
uint32_t pow10;

int main(){
    thread_sleep_for(2000);
    printf("Booting... Use CR code!!\r\n");
    spi.format(16,2);   //spi mode setting. 2byte transfer, mode 2
    cs=1;              //cs init.
    while(true) {
        printf("Ready!!\r\n");
        number=read();
        freq=0;
        for(i=0;i<number;++i){
            pow10=1;
            for(k=0;k<number-i-1;++k){
                pow10=10*pow10;
            }
            freq=freq+(buf[i]-48)*pow10;
        }
        //range check
        if(freq>max_freq){
            printf("Frequency is Out of range.\r\n");
            freq=max_freq;
        }
        printf("Set Freq=%d Hz\r\n",freq);
            
        //SPI write
        freq_reg=freq*res/mclk;
        lsb=(freq_reg&0x3FFF)+0x4000;
        msb=(freq_reg>>14)+0x4000;
        cs=0;
        spi.write(0x2100);
        spi.write(lsb);
        spi.write(msb);
        cs=1;
        //reset
        cs=0;
        spi.write(0x2000);
        cs=1;
    }
}

//uart delimiter read func.
uint8_t read(){
    uint8_t i;
    char local[1];
    for (i=0;i<buf_size;++i){
        pc.read(local,1);
        buf[i]=local[0];
        if(local[0]==delimiter)break;
    }
    return i;
}
