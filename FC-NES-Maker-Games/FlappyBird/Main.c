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

typedef struct{
	u8 length;	// length(count) of data bytes
	u8 *rambuf;	// data buf in ram
	u16	vramst;	// vram start
}t_pputask;

#define TASKS_MAX	8	// max count of task
#define	tasks ((t_pputask*)0x0300)


#define SCRREN_TITLNUM_V		26	// ��Ч��Ļ��(��λ8pix)
#define DOOR_HEIGHT				7	// �ܵ��м�ķ�϶���(��λ8pix)
#define DOOR_SPACE_BLOCK		4	// ���ܵ����ֵļ��(��λ16pix)
#define DOOR_WIDTH				2	// �ܵ����(��λ8pix)
u8 tiles_buf[2][SCRREN_TITLNUM_V];
u8 door_buf[64/DOOR_WIDTH];	// ��������ܵ�λ��

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
	}
	++SOLX_VAL;
}

// ��������
void scrleft()
{
	if(SOLX_VAL == 0){
		switchname();
	}
	--SOLX_VAL;
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

// ���þ��鷽��(16*16)��chr��ʹ�ô�spix��ʼ��4�����飬�����chr����Ϊchrix
void sptile(u8 spix, u8 chrix)
{
	sp[spix].tile = chrix;
	sp[spix+1].tile = chrix+1;
	sp[spix+2].tile = chrix+0x10;
	sp[spix+3].tile = chrix+0x11;
}

// ���þ��鷽��(16*16)�����꣬ʹ�ô�spix��ʼ��4�����飬���Ͻǵľ���λ��Ϊ(x,y)
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

// ʹ�þ�����ʾ����
u8 scores[5] = {1,2,3,4,5};
void spnums(u8 spix, u8 x, u8 y)
{
	u8 i;
	for(i=0; i<sizeof(scores); ++i){
		sp[spix+i].x = x;
		sp[spix+i].y = y;
		sp[spix+i].tile = '0'+scores[i];
		x += 8;
	}
}

// �����
const u8 rnds[] = {0xfe, 0xc9, 0xd4, 0x45, 0xd8, 0xf6, 0x4e, 0x89, 0x3a, 0x1b, 0x36, 0x16, 0xbd, 0xc5, 0xc1, 0x73, 0xdb, 0xb2, 0xda, 0xcd, 0x3a, 0x7e, 0x97, 0x64, 0x22, 0x01, 0x1f, 0x87, 0x48, 0xb5, 0xc7, 0xdd, 0x90, 0xe4, 0xc4, 0x6f, 0xe1, 0x95, 0x8f, 0x29, 0xbd, 0x6b, 0x49, 0x8d, 0x7c, 0xc0, 0x62, 0x38, 0x87, 0x7d, 0xbd, 0xdc, 0x0f, 0xbe, 0x54, 0x43, 0x18, 0xbf, 0x7e, 0x8e, 0xfb, 0x65, 0xa9, 0xde, 0xc8, 0x85, 0xb5, 0x43, 0xd1, 0x23, 0xeb, 0x28, 0x03, 0x9e, 0x08, 0xb5, 0x53, 0x57, 0x57, 0x86, 0xf7, 0x32, 0xf9, 0xa3, 0x38, 0xa6, 0xbf, 0xf9, 0x07, 0x2c, 0xa4, 0x78, 0x81, 0x17, 0xaa, 0x6c, 0x00, 0x5b, 0x29, 0x48, 0x27, 0x0c, 0xa5, 0x51, 0x15, 0x01, 0xc8, 0x95, 0x5e, 0xb5, 0x70, 0x23, 0x91, 0xfc, 0x08, 0x33, 0x51, 0x68, 0x19, 0x25, 0x90, 0x94, 0xc1, 0xa5, 0x57, 0xd7, 0x52, 0xdf, 0x88, 0xd7, 0x5d, 0x3b, 0xe6, 0xc1, 0x92, 0x0d, 0x94, 0x91, 0xfb, 0x3d, 0x26, 0x43, 0xfa, 0x4d, 0xb6, 0x51, 0x0d, 0xac, 0x6f, 0x3d, 0x32, 0xb7, 0x1d, 0x25, 0x18, 0x82, 0x0e, 0x3e, 0x03, 0x0e, 0x9c, 0x1c, 0x27, 0x13, 0x40, 0xeb, 0xa7, 0x50, 0x53, 0x29, 0x58, 0x7f, 0xaa, 0xf2, 0x2f, 0x3c, 0x1a, 0xe4, 0x61, 0xe0, 0xef, 0x86, 0x09, 0xba, 0x4c, 0xfd, 0xc5, 0xbf, 0x71, 0x75, 0x3e, 0x0e, 0x82, 0xc2, 0xa5, 0xe7, 0x5a, 0x34, 0x32, 0xd0, 0xec, 0x54, 0xe7, 0x66, 0x5b, 0x3e, 0xf5, 0xc6, 0x35, 0x74, 0x2c, 0xba, 0x89, 0x49, 0xbd, 0xca, 0x80, 0xb1, 0x7e, 0x0d, 0x60, 0xec, 0x1e, 0xf7, 0xd4, 0x3d, 0x88, 0xfb, 0x1d, 0xbe, 0x1a, 0x58, 0x07, 0x8f, 0x23, 0x14, 0x3d, 0x47, 0x91, 0x75, 0xd9, 0x28, 0x02, 0x4f, 0xff, 0x8b, 0x94, 0x46, 0x3e, 0x14, 0x25, 0xb3, 0x31, 0xf7, 0x19, 0xb6};
// ���������
u8 rndsd = 0;
// ����һ��������below�������
u8 rnd(u8 below)
{
	static u8 ix = 0;
	++ix;
	return rnds[rndsd+ix]%below;
}

// ��Ϸ
void game()
{
	u8 spx=0xc0, ix, running=0, okey, i, n, m, loadedattr0=0;
	int acc = 0;
	int spy = 0x58;
	const u8 sps[] = {0x80, 0x82, 0x84};
	
	REG(PPU_CTRL1) = 0x00;
	REG(PPU_CTRL2) = 0x00;
	
	PC1_VAL = 0x00;
	PC2_VAL = 0x00;
	SOLX_VAL = 0x00;
	fillram(sp, 0x100, 0x00);	// ��վ���
	fillram(tasks, 0x100, 0x00);	// ���PPU����

	tasks[0].rambuf = tiles_buf[0];
	tasks[1].rambuf = tiles_buf[1];
	
	// ���ر���
	loadname0(map_nam);	// ������
	loadattr0(map_atb);	// ���Ա�
	
	loadbgpal(map_col);	// ��ɫ
	loadsppal(map_col);
	
	loadname1(map_nam+VRAM_NAME_LEN);
	loadattr1(map_atb+VRAM_ATTR_LEN);
	sptile(1, 0x80);
	splocal(1, spx, spy);
	PC2_VAL = PC2_SHOW_BG | PC2_SHOW_L8BG | PC2_SHOW_SP | PC2_SHOW_L8SP;
	NMI_ENABLE();
	
	fillram(door_buf, sizeof(door_buf), 0);
	
	for(;;){
		readjoy();
		rndsd += 3; // ���һ�� 
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
				-- rndsd; // �����һ��
				ix = 0;
				break;
			case JOY_START:
				if(okey != JOY_VAL){
					running = !running;
				}
				break;
			case JOY_SELECT:
				// ����
				return;
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
		else if(!loadedattr0){
			tasks[2].vramst = VRAM_ATTR0;
			tasks[2].rambuf = map_atb+VRAM_ATTR_LEN;
			tasks[2].length = VRAM_ATTR_LEN;
			loadedattr0 = 1;
			waitvblank();
			PC1_VAL |= PC1_INC32;
			continue;
		}
		spy += (acc/8+1);
		if(spy<SP_Y_MIN){
			spy = SP_Y_MIN;
			// acc = 0;
		}
		else if(spy>SP_Y_MAX){
			spy = SP_Y_MAX;
		}
		if(acc<SP_MAXACC)
			++acc;
		waitvblank();
		splocal(1, (u8)spx, spy);
		sptile(1,sps[(ix>>2)%3]);
		
		if(loadedattr0 && SOLX_VAL%16 == 0){
			// 16=2*8
			m = SOLX_VAL/16;
			if(m%DOOR_SPACE_BLOCK == 0){
				n = rnd(SCRREN_TITLNUM_V-DOOR_HEIGHT-1)+1;
				for(i=0; i<SCRREN_TITLNUM_V; ++i){
					if(i>n && i<(n+DOOR_HEIGHT)){
						tiles_buf[0][i] = 0x00;
						tiles_buf[1][i] = 0x00;
					}
					else{
						tiles_buf[0][i] = 0xaa;
						tiles_buf[1][i] = 0xab;
					}
				}
				tiles_buf[0][n] = 0xB8;
				tiles_buf[1][n] = 0xB9;
				
				tiles_buf[0][n+DOOR_HEIGHT] = 0xA8;
				tiles_buf[1][n+DOOR_HEIGHT] = 0xA9;
				
				// ��ǿ�϶λ��
				door_buf[m] = n;
			}
			else{
				for(i=0; i<SCRREN_TITLNUM_V; ++i){
					tiles_buf[0][i] = 0x00;
					tiles_buf[1][i] = 0x00;
				}
				
				// û�йܵ�
				door_buf[m] = 0;
			}
			if(IS_NAME0()){
				tasks[0].vramst = VRAM_NAME1+(SOLX_VAL/8);
			}
			else{
				tasks[0].vramst = VRAM_NAME0+(SOLX_VAL/8);
			}
			
			tasks[1].vramst = tasks[0].vramst+1;
			
			// already vram data
			// set length to notice nmi transfer data to vram
			tasks[1].length = tasks[0].length = SCRREN_TITLNUM_V;
		}
		
		/*
		if(loadedattr0 && SOLX_VAL%64==20){
			for(i=(sizeof(scores)-1); i; --i){
				++scores[i];
				if(scores[i]<10)
					break;
				else
					scores[i] = 0;
			}
			spnums(5,100,10);
		}
		
		*/
		
		
		n = SOLX_VAL + spx + 16; //(SOLX_VAL + spx +16 )>>4;
		m = n>>4;
		
		scores[0] = m/100;
		scores[1] = m/10 - scores[0]*10;
		scores[2] = m%10;
		
		if(door_buf[m]){
			scores[4] = door_buf[m]%10;
			scores[3] = door_buf[m]/10;
			beep();
		}
		
		spnums(5,100,10);
		// waitvblank();
	}
}

void main()
{
	for(;;){
		game();
		waitvblank();
		waitvblank();
	}
}

