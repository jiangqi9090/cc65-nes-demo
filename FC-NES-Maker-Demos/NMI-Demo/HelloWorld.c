#include <conio.h>

// ȫ�ֱ���
unsigned int nmict = 0;

// �жϺ���
void mynmi()
{
	++nmict;
}

void main()
{
	while(1){
		// ���һ��
		gotoxy(10,10);
		cprintf("nmict=%05d", nmict);
	}
}
