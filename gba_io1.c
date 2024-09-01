#include "gba1.h"
#include "8x8ascii.h"


/* 関数プロトタイプ宣言 */
void locate(hword, hword);
void printh(hword);
void printn(hword);
void prints(byte*);
void print_ascii(byte);
void draw_point(hword, hword, hword);
hword mod(hword dividened, hword divisor);
hword div(hword dividened, hword divisor);


/* グローバル変数定義 */
point p;				/* 座標指定用構造体 */


/* 文字座標(0, 0)～(29,19)の位置(cx, cy)を画面座標に変換 */
void locate(hword cx, hword cy){
	if (cx < LCD_CHAR_WIDTH ) {
		p.x = cx << 3;				/* x座標の指定 */
	} else {
		p.x = LCD_CHAR_WIDTH - 1;	/* xの最大値を設定 */
	}
	if (cy < LCD_CHAR_HEIGHT) {
		p.y = cy << 3;				/* y座標の指定 */
	} else {
		p.y = LCD_CHAR_HEIGHT - 1;	/* yの最大値を設定 */
	}
}


/* 引数を16進数として表示する関数（最大値16ビット）*/
void printh(hword val) {

	byte char_data[] = "0123456789ABCDEF";
	byte buf[7];
	hword tmp;
	int i;

	/* 入力値valの桁数判定 */
	tmp = val;
	for (i = 0; i < 4; i++) {
		if (tmp > 0xF){
			tmp = tmp >> 4;
		} else {
			break;
		}
	}

	/* 文字列の最後にNULLコード挿入（"0x"の先頭2文字分の値をiに加算）*/
	buf[i+3] = 0;

	/* 最下位桁の文字を挿入し，入力値を4ビットシフト（"0x"の先頭2文字分の値をiに加算）*/
	for (; i >= 0; i--) {
		buf[i+2] = char_data[val & 0xF];
		val = val >> 4;
	}

	/* 文字列先頭に16進数の"0x"を挿入 */
	buf[1] = 'x';
	buf[0] = '0';

	/* 文字列全体を表示 */
	prints(buf);
	
	return;
}


/* 引数を10進数として表示する関数（最大値16ビット）*/
void printn(hword val) {

	byte char_data[] = "0123456789";
	byte buf[6];
	hword tmp;
	int i;
	
	/* 入力値valの桁数判定 */
	tmp = val;
	for (i = 0; i < 5; i++) {
		if (tmp >= 10){
			tmp = div(tmp, 10);
		} else {
			break;
		}
	}

	/* 文字列の最後にNULLコード挿入 */
	buf[i+1] = 0;
	
	/* 最下位桁の文字（10で割った余り）を挿入し，入力値を10で割る */
	for(; i >= 0; i--) {
		buf[i] = char_data[mod(val, 10)];
		val = div(val, 10);
	}
	
	/* 文字列全体を表示 */
	prints(buf);
	
	return;
}


/* 割り算の商を求める関数*/
hword div(hword dividened, hword divisor){
	
	hword quotient = 0;			/* 商 */
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return quotient;
}


/* 割り算の余りを求める関数*/
hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			/* 商 */
	
	while(1){
		if (dividened >= divisor){
			dividened = dividened - divisor;
			quotient++;
		} else {
			break;
		}
	}
	return dividened;
}


/* 文字列表示関数 */
void prints(byte *str) {

	while(*str) {
		print_ascii(*str++);
		p.x = p.x + CHAR_WIDTH;			/* 表示位置を1文字右へ */
	}	
}


/* locateで設定された位置にASCIIコードascii_numに相当する文字を表示する関数 */
void print_ascii(byte ascii_num) {
	
	hword tx, ty;							/* 文字パターンの座標指定 */
	byte cbit;								/* ビット判定 */

	for(ty = 0; ty < 8; ty++) {				/* 文字パターンのy座標指定 */
	
		cbit = 0x80;						/* 判定ビットを8ビットの最上位にセット */
		
		for(tx = 0; tx < 8; tx++) {							/* 文字パターンのx座標指定 */
			if((char8x8[ascii_num][ty] & cbit) == cbit){			/* ビット位置の判定 */
				draw_point((p.x + tx), (p.y + ty), BGR(0x1F, 0x1F, 0x1F));	// 1なら白
			}else {
				draw_point((p.x + tx), (p.y + ty), BGR(0x00, 0x00, 0x00));	// 0なら黒
			}
			cbit = cbit >> 1;					/* 判定ビットを1ビット右へシフト */
		}
	}
}


/* 指定した座標(x, y) に指定した色colorの点を描画する関数 */
void draw_point(hword x, hword y, hword color) {

	hword *ptr;
	ptr = (hword *) VRAM;
	
	ptr = ptr + x + y * LCD_WIDTH;
	*ptr = color;

}
