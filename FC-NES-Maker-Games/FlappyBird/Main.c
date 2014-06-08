#include "sinnes.h"
extern u8 map_nam_s, map_col_s, map_atb_s;

// �����ȥ��ַ����
#define map_nam (&map_nam_s)
#define map_col (&map_col_s)
#define map_atb (&map_atb_s)

#define NMI_ENABLE()	{PC1_VAL |= PC1_NMI; address(PPU_CTRL1) = PC1_VAL;}
#define NMI_DISABLE()	{PC1_VAL &= ~PC1_NMI; address(PPU_CTRL1) = PC1_VAL;}

// �жϵ�ǰ�Ƿ������ֱ�0
#define IS_NAME0()	((PC1_VAL & PC1_NAME_MASK) == PC1_NAME0)

typedef struct // sprite struct
{
	u8 y; // y
	u8 tile; // tile #
	u8 attr; // attribute: vhp000cc(V turn, H turn,  priorite, 000 , high 2 bits of color)
	u8 x; // x
}t_sprite;

#define SP_ATTR_VTURN	0x80	// V turn
#define SP_ATTR_HTURN	0x40	// H turn
#define SP_ATTR_PRI		0x20	// priorite
#define SP_ATTR_COL		0x03	// high 2 bits of color

#define sp	((t_sprite*)0x0200)

#define SP_MIDLEX	0x70
#define SP_MAXACC	18
#define SP_MINACC	-24
#define SP_Y_MIN	0x06
#define SP_Y_MAX	0xC4

void switchname()
{
	if(IS_NAME0()){
		// �е�1
		PC1_VAL &= ~PC1_NAME_MASK;
		PC1_VAL |= PC1_NAME1;
	}
	else{
		// �е�0
		PC1_VAL &= ~PC1_NAME_MASK;
		PC1_VAL |= PC1_NAME0;
	}
}

// ��������
void scrright()
{
	if(SOLX_VAL == 255){
		switchname();
		SOLX_VAL = 0;
	}
	else{
		++SOLX_VAL;
	}
}

// ��������
void scrleft()
{
	if(SOLX_VAL == 0){
		switchname();
		SOLX_VAL = 255;
	}
	else{
		--SOLX_VAL;
	}
}

void keyproc()
{
	if(JOY_VAL == JOY_RIGHT){
		scrright();
	}
	else if(JOY_VAL == JOY_LEFT){
		scrleft();
	}
}


void sptile(u8 spix, u8 chrix)
{
	sp[spix].tile = chrix;
	sp[spix+1].tile = chrix+1;
	sp[spix+2].tile = chrix+0x10;
	sp[spix+3].tile = chrix+0x11;
}

void splocal(u8 spix, u8 x, u8 y)
{
	sp[spix].x = x;
	sp[spix].y = y;
	sp[spix+1].x = x+8;
	sp[spix+1].y = y;
	sp[spix+2].x = x;
	sp[spix+2].y = y+8;
	sp[spix+3].x = x+8;
	sp[spix+3].y = y+8;
}

void main()
{
	u8 spx=0xc0, ix, running=0, okey;
	int acc = 0;
	int spy = 0x58;
	const u8 sps[] = {0x80, 0x82, 0x84};
	PC1_VAL = 0x00;
	PC2_VAL = 0x00;
	SOLX_VAL = 0x00;
	fillram(sp, 0x100, 0x00);	// ��վ���

	// ���ر���
	loadname0(map_nam);	// ������
	loadattr0(map_atb);	// ���Ա�
	
	loadbgpal(map_col);	// ��ɫ
	loadsppal(map_col);
	
	loadname1(map_nam+VRAM_NAME_LEN);
	loadattr1(map_atb+VRAM_ATTR_LEN);
	sptile(1, 0x80);
	splocal(1, spx, spy);
	PC1_VAL |= PC1_INC32;
	PC2_VAL = PC2_SHOW_BG | PC2_SHOW_L8BG | PC2_SHOW_SP | PC2_SHOW_L8SP;
	NMI_ENABLE();
	for(;;){
		readjoy();
		// keyproc();
		switch(JOY_VAL){
			case JOY_LEFT:
				--spx;
				break;
			case JOY_RIGHT:
				++spx;
				break;
			case JOY_UP:
				--spy;
				break;
			case JOY_DOWN:
				++spy;
				break;
			case JOY_B:
			case JOY_A:
				if(okey != JOY_VAL){
					beep();
					acc = SP_MINACC;
				}
				break;
			case JOY_START:
				if(okey != JOY_VAL){
					running = !running;
				}
				break;
		}
		okey = getjoy();
		++ix;
		if(!running){
			waitvblank();
			sptile(1,sps[(ix>>2)%3]);
			continue;
		}
		
		scrright();
		if(spx>SP_MIDLEX)
			--spx;
			
		spy += (acc/8+1);
		if(spy<SP_Y_MIN)
			spy = SP_Y_MIN;
		else if(spy>SP_Y_MAX)
			spy = SP_Y_MAX;
		if(acc<SP_MAXACC)
			++acc;
		waitvblank();
		splocal(1, (u8)spx, spy);
		sptile(1,sps[(ix>>2)%3]);
	}
}
