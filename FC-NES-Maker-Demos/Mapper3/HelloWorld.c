#include <conio.h>

// ֱ�ӷ��ʵ�ַ���Ĵ�����
#define REG(_addr)	(*((u8*)(_addr)))
// �ֱ�1�Ĵ���
#define JOY1	0x4016

typedef unsigned char u8;
typedef unsigned int u16;

// һ��ȫ�ֵ�ҳ�ţ�0����1֮���л�
u8 num = 0;

// �ֱ�1
u8 joy1()
{
	u8 n=8,joy_state=0;
	REG(JOY1)=01;
	REG(JOY1)=00;
	while(n){
		joy_state=(joy_state<<1)|REG(JOY1)&1;
		--n;
	}
	return joy_state;
}

// ��ʱ
void delay(u16 ct)
{
	while(ct--)
		waitvblank();
}

void mynmi()
{
	REG(0x8000) = num; // ��ҳ��д��$8000�Ĵ�����ʼ��ҳ
}

void main()
{
	
	gotoxy(10,5);
	cprintf("Mapper3 Demo");
	gotoxy(6,7);
	cprintf("It's failed on FCEUX");
	gotoxy(9,9);
	cprintf("Try VirtuaNES");
	gotoxy(6,12);
	cprintf("Any Key To Change CHR");
	while(1){
		if(joy1()){
			// ��num��0��1֮��ѭ������Ϊ����ֻ����ҳchr��
			++num;
			num &= 0x01;
			
			
			// 
			gotoxy(13,14);
			cprintf("num:%02x", num);
			
			// ��ʱһ�²�Ȼ��̫����
			delay(10);
		}
	}
}
