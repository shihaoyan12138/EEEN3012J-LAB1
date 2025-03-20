#include <C8051F020.h>

sbit SS=P0^7;
long val_adc = 0;
unsigned int last_adc = 0;
unsigned int a1,a2,a3,b1,b2,b3;


void init_System(){
	WDTCN = 0xDE;	//watchdog
	WDTCN = 0xAD;	//watchdog
}

void init_ADC(){
	REF0CN=0x03;
	AMX0SL=0x02;
	ADC0CN=0x80;
}


void Delay(int i){
	while(i--);
}

void SPI_Send(int val_dat){
	SS = 0;
	SPI0DAT = val_dat;
	while(TXBSY);
	SS = 1;
	Delay(10);
}

void Print(int val_disp_num){
	SPI_Send(0x77);		//dicimal command
	SPI_Send(0x01);		//dicimal data: 0~63

	SPI_Send(0x79);		//cursor command
	SPI_Send(0);		//cursor position: 0~3

	SPI_Send(val_disp_num/1000%10);
	SPI_Send(val_disp_num/100%10);
	SPI_Send(val_disp_num/10%10);
	SPI_Send(val_disp_num%10);

}

void init_IO(){
	P0MDOUT = 0xFF;
	
	XBR0 = 0x06;	//crossbar for SPI
	XBR2 = 0x40;	//cro3ssbar enable

	SPI0CFG = 0x07;
	SPI0CN = 0x07;
	SPI0CKR = 0x09;	//SPI_clk == sys_clk/((0x09+1)*2)
	SS = 1;

	SPI_Send(0x7A);	//brightness control command
	SPI_Send(150);	//brightness control d 0~255
	SPI_Send(0x76);	//clear display
}

unsigned int get_filtered_adc(){
    
    AD0BUSY = 1;  // ??ADC??
    while(!AD0INT);  // ??????
    val_adc =  (int)((ADC0H * 256 + ADC0L)*2.402/4.096);// ??ADC????
	  a1 = val_adc/1000;
	  a2 = (val_adc/100)%10;
	  a3 = (val_adc/10)%10+val_adc%10;
	  b1 = last_adc/1000;
	  b2 = (last_adc/100)%10;
	  b3 = (last_adc/10)%10+last_adc%10;
	  AD0INT = 0;  // ??ADC????
	
	  while((a1 == b1)&&(a2 == b2)&&(((a3-b3)<1)||((b3-a3)< 100))){
       get_filtered_adc();
    }		
		
    last_adc = val_adc;
    return val_adc;  // ????
}
void main(){
    init_System();    // ?????(??????)
    init_IO();        // IO?????
    init_ADC();       // ADC???
    
    while(1){
        //AD0BUSY = 1;       // ??ADC??
        //while(!AD0INT);    // ??????(AD0INT??)
        
        //val_adc = (int)((ADC0H * 256 + ADC0L)*2.402/4.096);  // ??ADC?
			  
        Print(get_filtered_adc());                 // ??ADC?
        
        //AD0INT = 0;         // ??ADC????
			  Delay(100);

    }
}