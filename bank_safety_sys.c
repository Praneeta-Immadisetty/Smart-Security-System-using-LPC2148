//Matrix 4 x 4 Keyboard
//Columns & Rows are pulled to +5v,if dont press key, we receive '1' on columns
//Method: Sending '0' to a selected row, checking for '0' on each column
//ROWS - ROW0-ROW3 -> P0.16,P0.17,P0.18,P0.19
//COLS - COL0-COL3 -> P1.19,P1.18,P1.17,P1.16
#include <lpc214x.h>
#define PLOCK 0x00000400
#define LED_OFF (IO0SET = 1U << 31)
#define LED_ON (IO0CLR = 1U << 31)
#define COL0 (IO1PIN & 1 <<19)
#define COL1 (IO1PIN & 1 <<18)
#define COL2 (IO1PIN & 1 <<17)
#define COL3 (IO1PIN & 1 <<16)
//#define IO0SET = 1U << 20 (IO0SET = 1U << 20)
//#define IO0CLR= 1U << 20 (IO0CLR = 1U << 20)
#define EN_ON (IO1SET = 1U << 25)
#define EN_OFF (IO1CLR = 1U << 25)
void SystemInit(void);
static void delay_ms(unsigned int j);//millisecond delay
static void delay_us(unsigned int count);//microsecond delay
static void LCD_SendCmdSignals(void);
static void LCD_SendDataSignals(void);
static void LCD_SendHigherNibble(unsigned char dataByte);
static void LCD_CmdWrite( unsigned char cmdByte);
static void LCD_DataWrite( unsigned char dataByte);
static void LCD_Reset(void);
static void LCD_Init(void);
static void call_lcd(int flag,int count);
static void call_b(void);
void LCD_DisplayString(const char *ptr_stringPointer_u8);
void SystemInit(void);
void delay_ms(unsigned int j);
unsigned char getAlphaCode(unsigned char alphachar);
void alphadisp7SEG(char *buf);
static void call_stepper(void);
static void keybord(void);
void uart_init(void);
unsigned char pss[7];
int readSensor(int sen_no);
unsigned char correct[] = "123abc";
unsigned int i,count=0,j=0;
	unsigned int flag=-1;
unsigned char lookup_table[4][4]={ {'0', '1', '2','3'},
{'4', '5', '6','7'},
{'8', '9', 'a','b'},
{'c', 'd', 'e','f'}};
unsigned char rowsel=0,colsel=0;
int main( ){
	SystemInit();
	
	call_lcd(flag,0);
	
	uart_init();//initialize UART0 port
	IO0DIR |= 1U << 31 | 0x00FF0000; // to set P0.16 to P0.23 as o/ps
	keybord();
	
}
static void keybord(void)	
{
	do{
		j=0;
		flag=0;
		while(j<6)
		{
		while(1){
			//check for keypress in row0,make row0 '0',row1=row2=row3='1'
			rowsel=0;IO0SET = 0X000F0000;IO0CLR = 1 << 16;
			if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
			//check for keypress in row1,make row1 '0'
			rowsel=1;IO0SET = 0X000F0000;IO0CLR = 1 << 17;
			if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
			//check for keypress in row2,make row2 '0'
			rowsel=2;IO0SET = 0X000F0000;IO0CLR = 1 << 18;//make row2 '0'
			if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
			//check for keypress in row3,make row3 '0'
			rowsel=3;IO0SET = 0X000F0000;IO0CLR = 1 << 19;//make row3 '0'
			if(COL0==0){colsel=0;break;};if(COL1==0){colsel=1;break;};
			if(COL2==0){colsel=2;break;};if(COL3==0){colsel=3;break;};
		}
		delay_ms(50); //allow for key debouncing
		while(COL0==0 || COL1==0 || COL2==0 || COL3==0);//wait for key release
		delay_ms(50); //allow for key debouncing
		IO0SET = 0X000F0000; //disable all the rows
		U0THR = lookup_table[rowsel][colsel]; //send to serial port(check on the terminal)
		pss[j++]=lookup_table[rowsel][colsel];	
	}

	i=0;
	while(i<6)
	{
		if(correct[i]!=pss[i])
		{	flag=1;
			count++;
			delay_ms(100);
//LCD_CmdWrite(0x80); 
			return;
		}
		
		i++;
	}
	call_lcd(flag,count);
}
	while(count<=3);
}
static void call_stepper(void)
{
	unsigned int no_of_steps_clk = 50;
	IO0DIR |= 1U << 31 | 0x00FF0000 | 1U << 30;
	do{
		IO0CLR = 0X000F0000;IO0SET = 0X00010000;delay_ms(500);if(--no_of_steps_clk == 0) break;
		IO0CLR = 0X000F0000;IO0SET = 0X00020000;delay_ms(500);if(--no_of_steps_clk == 0) break;
		IO0CLR = 0X000F0000;IO0SET = 0X00040000;delay_ms(500);if(--no_of_steps_clk == 0) break;
		IO0CLR = 0X000F0000;IO0SET = 0X00080000;delay_ms(500);if(--no_of_steps_clk == 0) break;
	}while(1);
	IO0CLR = 0X00FF0000;
}
	
static void call_lcd(int flag,int count)
{
	
	IO0DIR |= 1U << 31 | 0x00FF0000 ; // to set P0.16 to P0.23 as o/ps
	IO1DIR |= 1U << 25; // to set P1.25 as o/p used for EN
	// make D7 Led on off for testing
	LCD_Reset();
	LCD_Init();
	if(flag==-1)
	{
		delay_ms(100);
	LCD_CmdWrite(0x80); LCD_DisplayString("Enter Password");
	}
	else if (flag==0)
	{
	delay_ms(100);
	LCD_CmdWrite(0x80); LCD_DisplayString("Password Correct");
		
		call_stepper();
		IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30 ; // to set as o/ps
	alphadisp7SEG("fine");
		delay_ms(5000);
		
	}
	else if (count<=3){
		delay_ms(100);
	LCD_CmdWrite(0x80); LCD_DisplayString("Password Incorrect");
		call_b();
		IO0DIR |= 1U << 31 | 1U << 19 | 1U << 20 | 1U << 30 ; // to set as o/ps
	alphadisp7SEG("alert");
		delay_ms(5000);
		
		keybord();
	}
	else  call_b();
}	


static void call_b()
{
	unsigned int val=0;
	PINSEL1 |= 0x00080000;
	int k=0;
//P0.25 used as AOUT - DAC output
//val - 0 to 1023 : 10 bit DAC, P0.25 used as AOUT generates analog output
	while(k<100)
	{
		k++;
DACR = ( (1<<16) | (val<<6) );
delay_ms(100);
		DACR = ( (1<<16) | (1023<<6) );
		delay_ms(100);

	}
}	
unsigned char getAlphaCode(unsigned char alphachar){
	switch (alphachar){
		// dp g f e d c b a - common anode: 0 segment on, 1 segment off
		case 'f':return 0x8e;
		case 'i':return 0xf9;
		case 'r':return 0xce;
		case 'e':return 0x86; // 1000 0110
		case 'h':return 0x89;
		case 'l':return 0xc7;
		case 'p':return 0x8c;
		case ' ': return 0xff;
		case 'a':return 0x88;
		case 't':return 0x87;
		case 'n':return 0xc8;
		//simmilarly add for other digit/characters
		default : break;
	}
	return 0xff;
}
void alphadisp7SEG(char *buf){
	unsigned char i,j;
	unsigned char seg7_data,temp=0;
	for(i=0;i<5;i++) // because only 5 seven segment digits are present
	{
		seg7_data = getAlphaCode(*(buf+i)); //instead of this look up table can be used
		//to shift the segment data(8bits)to the hardware (shift registers) using
		//Data,Clock,Strobe

		for (j=0 ; j<8; j++)
		{
			//get one bit of data for serial sending
			temp = seg7_data & 0x80; // shift data from Most significan bit (D7)
			if(temp == 0x80)
				IOSET0 |= 1 << 19; //IOSET0 | 0x00080000;
			else
				IOCLR0 |= 1 << 19; //IOCLR0 | 0x00080000;
			//send one clock pulse
			IOSET0 |= 1 << 20; //IOSET0 | 0x00100000;
			delay_ms(1);
			IOCLR0 |= 1 << 20; //IOCLR0 | 0x00100000;

			seg7_data = seg7_data << 1; // get next bit into D7 position

		}
}

// send the strobe signal
	IOSET0 |= 1 << 30; //IOSET0 | 0x40000000;
	delay_ms(1); //nop();
	IOCLR0 |= 1 << 30; //IOCLR0 | 0x40000000;
	return;
}
static void LCD_CmdWrite( unsigned char cmdByte){
	LCD_SendHigherNibble(cmdByte);
	LCD_SendCmdSignals();
	cmdByte = cmdByte << 4;
	LCD_SendHigherNibble(cmdByte);
	LCD_SendCmdSignals();
}
static void LCD_DataWrite( unsigned char dataByte){
	LCD_SendHigherNibble(dataByte);
	LCD_SendDataSignals();
	dataByte = dataByte << 4;
	LCD_SendHigherNibble(dataByte);
	LCD_SendDataSignals();
}
static void LCD_Reset(void){
	/* LCD reset sequence for 4-bit mode*/
	LCD_SendHigherNibble(0x30);
	LCD_SendCmdSignals();
	delay_ms(100);
	LCD_SendHigherNibble(0x30);
	LCD_SendCmdSignals();
	delay_us(200);
	LCD_SendHigherNibble(0x30);
	LCD_SendCmdSignals();
	delay_us(200);
	LCD_SendHigherNibble(0x20);
	LCD_SendCmdSignals();
	delay_us(200);
}
static void LCD_SendHigherNibble(unsigned char dataByte){
	//send the D7,6,5,D4(uppernibble) to P0.16 to P0.19
	IO0CLR = 0X000F0000;IO0SET = ((dataByte >>4) & 0x0f) << 16;
}
static void LCD_SendCmdSignals(void){
	IO0CLR= 1U << 20; // RS - 1
	EN_ON;delay_us(100);EN_OFF; // EN - 1 then 0
}

static void LCD_SendDataSignals(void){
	IO0SET = 1U << 20;// RS - 1
	EN_ON;delay_us(100);EN_OFF; // EN - 1 then 0
}
static void LCD_Init(void){
	delay_ms(100);
	LCD_Reset();
	LCD_CmdWrite(0x28u); //Initialize the LCD for 4-bit 5x7 matrix type
	LCD_CmdWrite(0x0Eu); // Display ON cursor ON
	LCD_CmdWrite(0x01u); //Clear the LCD
	LCD_CmdWrite(0x80u); //go to First line First Position
}
void LCD_DisplayString(const char *ptr_string){
	// Loop through the string and display char by char
	while((*ptr_string)!=0)
	LCD_DataWrite(*ptr_string++);
}

void uart_init(void){
	//configurations to use serial port
	PINSEL0 |= 0x00000005; // P0.0 & P0.1 ARE CONFIGURED AS TXD0 & RXD0
	U0LCR = 0x83; /* 8 bits, no Parity, 1 Stop bit */
	U0DLM = 0; U0DLL = 8; // 115200 baud rate
	U0LCR = 0x03; /* DLAB = 0 */
	U0FCR = 0x07; /* Enable and reset TX and RX FIFO. */
}
void SystemInit(void){
	PLL0CON = 0x01;
	PLL0CFG = 0x24;
	PLL0FEED = 0xAA;
	PLL0FEED = 0x55;
	while( !( PLL0STAT & PLOCK ))
	{ ; }
	PLL0CON = 0x03;
	PLL0FEED = 0xAA; // lock the PLL registers after setting the required PLL
	PLL0FEED = 0x55;
	//VPBDIV = 0x01; // PCLK is same as CCLK i.e 60Mhz
}
void delay_ms(unsigned int j){
	unsigned int x,i;
	for(i=0;i<j;i++){
		for(x=0; x<500; x++);
	}
}
static void delay_us(unsigned int count){
	unsigned int j=0,i=0;
	for(j=0;j<count;j++){
		for(i=0;i<10;i++);
	}
}


