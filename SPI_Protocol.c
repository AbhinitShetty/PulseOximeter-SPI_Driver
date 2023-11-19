#include <stdio.h>
#include <stdbool.h>

//Macro Definition - Chip Select values to access different modules  
#define DAC_CS 0x00		//Access DAC module 
#define PGA_CS 0x01		//Access Power Gain Amplifier module
#define ADC_CS 0x02		//Access ADC module 

//Macro-Definition - Lengths of Data bits 
#define Data_len_8bit 0x00		//8-bit data
#define Data_len_16bit 0x01		//16-bit data
#define Data_len_24bit 0x02		//24-bit data
#define Data_len_32bit 0x03		//32-bit data

#define SPI_mode0 0x00
#define SPI_mode1 0x01
#define SPI_mode2 0x02
#define SPI_mode3 0x03

#define SPI_Done 0x00
#define SPI_Start 0x01

#define SPI_Clk_1Mhz 0x00
#define SPI_Clk_2Mhz 0x01

const unsigned int DAC_external_ref = 0x380001;  //Set external reference voltage of DAC - 1.25V
const unsigned int PGA_gain5 = 0x4003;
const unsigned int ADC_test = 0x2100;
enum {SL0,SL1,SL2,SL3}  ;
enum {DAC,ADC,PGA};
	typedef struct {
		char Modes;
		char Data_len;
		char CS;
		bool Clock;
	} SPI_struct;
	
	static SPI_struct SPI_setting;

unsigned int SPI_config(struct SPI_set *p);
bool SPI_Tx(unsigned int data);
unsigned int SPI_Rx(void);
unsigned int Read_SR(char SR);
void Write_SR(char SR, unsigned int SR_data);

int main(void)
{  
    printf("SPI test");

	Send_Data_SPI(DAC,DAC_external_ref);
	Send_Data_SPI(PGA,PGA_gain5);
	Send_Data_SPI(ADC,ADC_test);
	
    return 0;
}

/*****functions to test SPI ***********/

void Send_Data_SPI(char device, unsigned int data){

    if (device == DAC){
		DAC_SPI_config();
	}
	else if((device == PGA) | (device ==ADC)){
		PGA_ADC_SPI_config();
	}

	SPI_config(&SPI_setting);
	SPI_Tx(data);	
}
void DAC_SPI_config(void){
	//// setting for DAC
    SPI_setting.Modes = SPI_mode1;
    SPI_setting.Data_len = Data_len_24bit;
    SPI_setting.CS = DAC_CS;
    SPI_setting.Clock= SPI_Clk_1Mhz;
    }

void PGA_ADC_SPI_config(void){
	//// setting for PGA & ADC
    SPI_setting.Modes = SPI_mode3;
    SPI_setting.Data_len = Data_len_16bit;
    SPI_setting.CS = PGA_CS;
    SPI_setting.Clock= SPI_Clk_1Mhz;
    }
/**************************************/
/*
 * Function: SPI_config
 * ----------------------------
 *   Configures the SPI, Frame the SPI configuration and write to slave register 0
 *
 *   arg1: pointer to the SPI structure
 *
 *   returns: none 
 */
void SPI_config(struct SPI_set *p)
{
    unsigned int temp_config =0;
    
    temp_config = SPI_Start |(SPI_setting.Modes << 1) | (SPI_setting.Data_len<<3) | (SPI_setting.CS<<5) | (SPI_setting.Clock<<7);
    printf("temp_config 0x%x",temp_config );
	Write_SR(SL0, temp_config); //  send spi config data to slave register 0
}

/*
 * Function: SPI_Tx
 * ----------------------------
 *   Initiates SPI data transmission , write to slave register 1
 *
 *   arg1: pointer to the SPI structure
 *
 *   returns: 0 - fail, 1- sucess 
 */
bool SPI_Tx(unsigned int data){
	
	char count =100;
	bool result =0;
	
	while(done); // wait until spi ready 
	Write_SR(SL1, data); //  send data via spi
	
	while(!count & result){ // wait untill timeout or spi data transmission done
		if(Read_SR(SL0) & done){ // check bit0 of slave register 0
		 result = 1;	
		}
	}
	return result;
}
/*
 * Function: SPI_Rx
 * ----------------------------
 *   To recieve the data from SPI slave , read from slave register 3
 *
 *   arg: none
 *
 *   returns: recieve data
 */

unsigned int SPI_Rx(void){
	unsigned int rx_data =0;

		if(Read_SR(SL0) & done){ // test if spi rx data ready
		 rx_data = Read_SR(SL3); // read data drom slave register 3
		}

	return rx_data;
}
/*
 * Function: Read_SR
 * ----------------------------
 *   To read slave register x via AXI bus
 *
 *   arg: SL number (0-3)
 *
 *   returns: slave register data
 */

unsigned int Read_SR(char SR){
	
	unsigned int sr_data=0;
	
		switch(SR){
			case 0:
				sr_data = POXIREG(0);
				break;
			case 1:
				sr_data = POXIREG(1);
				break;
			case 2:
				sr_data = POXIREG(2);
				break;
			case 3:
				sr_data = POXIREG(3);
				break;
			default:
		}
	
	return sr_data;
}
/*
 * Function: Write_SR
 * ----------------------------
 *   To write slave register x via AXI bus
 *
 *   arg1: SL number (0-3)
 *	 arg2: data to be written in slave registers
 *   returns: none
 */
void Write_SR(char SR, unsigned int SR_data){
		
		switch(SR){
			case 0:
				POXIREG(0) = SR_data;
				break;
			case 1:
				POXIREG(1) = SR_data;
				break;
			case 2:
				POXIREG(2) = SR_data;
				break;
			case 3:
				POXIREG(3) = SR_data;
				break;
			default:
		}
}
