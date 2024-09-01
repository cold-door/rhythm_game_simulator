#include "8x8ascii.h"

typedef unsigned short hword;
typedef unsigned char  byte;

typedef struct{   // 点の座標ストラクタを宣言
	hword x;
	hword y;
}point;

#define VRAM 0x06000000
#define KEY  0x04000130

#define KEY_A      0x0001
#define KEY_B      0x0002
#define KEY_SELECT 0x0004
#define KEY_START  0x0008
#define KEY_RIGHT  0x0010
#define KEY_LEFT   0x0020
#define KEY_UP     0x0040
#define KEY_DOWN   0x0080
#define KEY_R      0x0100
#define KEY_L      0x0200
#define KEY_ALL    0x03FF
#define KEY_ABLR   0x0303

#define BGR(r, g, b) (r | (g << 5) | (b << 10))
#define WHITE     BGR(0x1F, 0x1F, 0x1F)
#define BLACK     BGR(0x00, 0x00, 0x00)
#define RED       BGR(0x1F, 0x00, 0x00)
#define GREEN     BGR(0x00, 0x1F, 0x00)
#define BLUE      BGR(0x00, 0x00, 0x1F)
#define YELLOW    BGR(0x1F, 0x1F, 0x00)
#define MAGENTA   BGR(0x1F, 0x00, 0x1F)
#define CYAN      BGR(0x00, 0x1F, 0x1F)

#define NUM_OF_LANE 4
#define MAX_LANE_NOTES 20
#define WIDTH_LANE 44
#define Y_START_NOTE 0
#define X_NOTE_0 25
#define X_NOTE_1 69
#define X_NOTE_2 113
#define X_NOTE_3 157
#define Y_JUDGELINE 144

#define START        0
#define TUTORIAL     1
#define OPTION       2
#define GAME         3
#define END          4

#define SPEED   0
#define STAMINA 1
#define JACK    2
#define TECH    3

hword tile_x;
hword tile_y;

hword phase;

hword speed = 1;
hword interval_note = 32;
hword pattern_note = SPEED;
hword limit_time = 3000;

hword best;
hword cool;
hword good;
hword miss;

hword is_clear;


/* メイン関数 */
int main(void){
	
	// プロトタイプ宣言
    void draw_point(hword x, hword y, hword color);
	void paint_all(hword color);
	void paint_half(hword color);
	void draw_line(point pa, point pb, hword color);
	void locate(hword x, hword y);
	void print_ascii(unsigned char ascii, hword color);
	void prints(unsigned char *str, hword color);
	void print_cursor();
	void delete_cursor();
	hword div(hword, hword);
	hword mod(hword, hword);
	void num_to_string(hword num, hword digit, hword color_num);
	void delay(hword deley_time);
	void draw_note(point p_note, hword num_note);
	void delete_note(point p_pre_note, hword num_note);
	void draw_keylane();
	void draw_judgeline(hword how_finger[]);
	void display_ready();
	void display_time(hword time);
	void display_score_set();
	void display_score(hword best, hword cool, hword good, hword miss);
	void display_button(hword x, hword y, hword how_finger[]);
	void display_best();
	void display_cool();
	void display_good();
	void display_miss();
	void display_combo(hword combo);
	hword judge_key();
	hword judge_finger0(hword pre_what_key, hword what_key);
	hword judge_finger1(hword pre_what_key, hword what_key);
	hword judge_finger2(hword pre_what_key, hword what_key);
	hword judge_finger3(hword pre_what_key, hword what_key);
	void display_title();
	void begin_start();
	void set_explain();
	void begin_tutorial();
	void begin_op();
	void begin_game();
	void paint_colorful();
	void begin_end();

	/* GBAモード設定初期化 */
	hword *ptr1;
	ptr1 = (hword*)0x04000000;	/* モード設定アドレス */
	*ptr1 = 0x0F03;		/* モード設定の初期化 */
	
    hword i = 0;
	hword j = 0;


	

	// タイマカウンタ設定レジスタ
	// 全体で0.01秒
	*((unsigned short *)0x04000100) = 0xFFC0;   // タイマ0 カウンタ設定(元クロック1/2^24秒 約60n秒)    FFC0
	*((unsigned short *)0x04000104) = 0xFFC0;	// タイマ1 カウンタ設定(元クロック1/2^16秒 約15.4u秒)  FFC0
	*((unsigned short *)0x04000108) = 0xFFD7;	// タイマ2 カウンタ設定(元クロック1/2^8秒  約3.93m秒)  FFD7
	*((unsigned short *)0x0400010C) = 0x0000;	// タイマ3 カウンタ設定(元クロック1秒)

	// タイマ制御レジスタ	
	*((unsigned short *)0x04000102) = 0x0080;	// タイマ0 制御設定（タイマON，割込みOFF，カスケードOFF，プリスケーラなし）
	*((unsigned short *)0x04000106) = 0x0084;	// タイマ1 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
	*((unsigned short *)0x0400010A) = 0x0084;	// タイマ2 制御設定（タイマON，割込みOFF，カスケードON，プリスケーラなし）
	*((unsigned short *)0x0400010E) = 0x0004;	// タイマ3 制御設定（タイマOFF，割込みOFF，カスケードON，プリスケーラなし）


	phase = START;
	
	
	while(1){
		
		if(phase == START){
			begin_start();
			phase = TUTORIAL;
		}
		
		if(phase == TUTORIAL){
			begin_tutorial();
			phase = OPTION;
		}
		
		if(phase == OPTION){
			begin_op();
			phase = GAME;
		}
		
		if(phase == GAME){
			begin_game();
			phase = END;
		}

		if(phase == END){
			begin_end();
		}
		
	}

	while(1);
	return 0;
}
    


// 引数の座標に引数の色で点を描画
void draw_point(hword x, hword y, hword color){
	hword *ptr;
    ptr = (hword*)VRAM;

    ptr += x + 240 * y;
    *ptr = color;
}

// 引数の色で画面全体を描画
void paint_all(hword color){
	int i, j;
	for(i = 0; i < 240; i++){
		for(j = 0; j < 160; j++){
			draw_point(i, j, color);
		}
    }
}

// 引数の色で画面半分を描画
void paint_half(hword color) {
	int i, j;
	for (i = 0; i < 240; i++) {
		for (j = 0; j < 80; j++) {
			draw_point(i, j, color);
		}
	}
}

// 引数の座標を結ぶ線分を描画
void draw_line(point pa, point pb, hword color){
	 point pc;
	 
	 pc.x = (pa.x + pb.x) / 2;
	 pc.y = (pa.y + pb.y) / 2;
	 
	 draw_point(pc.x, pc.y, color);
	 
	 if((pc.x - pa.x) >= 1){
	 	draw_line(pa, pc, color);
		draw_line(pc, pb, color);
	 }else{
	 	return;
	 }
}

// 座標を30×20のタイルに変換
void locate(hword x, hword y) {
	tile_x = 8 * x;
	tile_y = 8 * y;
}

// 文字表示
void print_ascii(unsigned char ascii, hword color) {
	hword tx = 0;
	hword ty = 0;
	unsigned char cbit;


	for (ty = 0; ty < 8; ty++) {
		cbit = 0x80;
			for (tx = 0; tx < 8; tx++) {
				if ((cbit & char8x8[ascii][ty]) == cbit) {
				draw_point(tile_x + tx, tile_y + ty, color); 
			} else {
				draw_point(tile_x + tx, tile_y + ty, BLACK);
			}
				cbit = cbit >> 1;
		}
	}
}

// 文字列表示
void prints(unsigned char *str, hword color) {
	
	hword store_x = tile_x;

	while (*str != 0x00) {
		print_ascii(*str, color);
		tile_x += 8;
		str++;
	}
	
	tile_x = store_x;
}

// カーソル表示
void print_cursor(){
	
	hword i;
	hword x = tile_x;
	hword y = tile_y + 7;
	
	
	for(i = 0; i < 8; i++){
		draw_point(x + i, y, RED);
	}
}

// カーソル消去
void delete_cursor(){
	
	hword i;
	hword x = tile_x;
	hword y = tile_y + 7;
	
	
	for(i = 0; i < 8; i++){
		draw_point(x + i, y, BLACK);
	}
}

// 割り算
hword div(hword dividened, hword divisor){
	
	hword quotient = 0;		
	
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


// 余りを求める
hword mod(hword dividened, hword divisor){
	
	hword quotient = 0;			
	
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


// 数字を文字列に変換して表示
void num_to_string(hword num, hword digit, hword color_num){

	int i;

	byte char_data[] = "0123456789";
	byte buff[digit + 1];
	
	buff[digit] = '\0';

	for(i = digit - 1; i >= 0; i--){
		buff[i] = char_data[mod(num, 10)];
		num = div(num, 10);
	}

	prints(buff, color_num);

}

//	遅延を生成
void delay(hword delay_time){
	hword i;
	
	for(i = 0; i < delay_time; i++){
	   locate(25,19);
		num_to_string(i, 5, BLACK);
	}
	
}


// 引数の座標を中心としてノーツを表示
void draw_note(point p_note, hword num_note){
	int i;
	int j;

	hword half_length = 19;
	hword half_width  = 1;

	hword color_center;
	hword color_aronud;

	switch(num_note){
		case 0:
			color_center = WHITE;
			color_aronud = CYAN;
			break;

		case 1:
			color_center = CYAN;
			color_aronud = WHITE;
			break;

		case 2:
			color_center = WHITE;
			color_aronud = CYAN;
			break;

		case 3:
			color_center = CYAN;
			color_aronud = WHITE;
			break;

	}

	for(i = 0; i < half_length; i++){
		draw_point(p_note.x + i, p_note.y, color_center);
		draw_point(p_note.x - i, p_note.y, color_center);
	}

	for(i = 1; i <= half_width; i++){
		for(j = 0; j < half_length; j++){
			draw_point(p_note.x + j, p_note.y + i, color_aronud);
			draw_point(p_note.x - j, p_note.y + i, color_aronud);
			draw_point(p_note.x + j, p_note.y - i, color_aronud);
			draw_point(p_note.x - j, p_note.y - i, color_aronud);
		}
	}

}

// ノーツを削除
void delete_note(point p_pre_note, hword num_note){
	int i;
	int j;

	hword half_length = 19;
	hword half_width  = 1;

	hword color_center = BLACK;
	hword color_aronud = BLACK;

	for(i = 0; i < half_length; i++){
		draw_point(p_pre_note.x + i, p_pre_note.y, color_center);
		draw_point(p_pre_note.x - i, p_pre_note.y, color_center);
	}

	for(i = 1; i <= half_width; i++){
		for(j = 0; j < half_length; j++){
			draw_point(p_pre_note.x + j, p_pre_note.y + i, color_aronud);
			draw_point(p_pre_note.x - j, p_pre_note.y + i, color_aronud);
			draw_point(p_pre_note.x + j, p_pre_note.y - i, color_aronud);
			draw_point(p_pre_note.x - j, p_pre_note.y - i, color_aronud);
		}
	}
}

// レーンを表示
void draw_keylane(){
	hword i;
	hword j;

	for(i = 0; i < 5; i++){
		for(j = 0; j < 160; j++){
			draw_point(3 + i * 44, j, WHITE);
		}
	}
	
}

// 判定線を表示
void draw_judgeline(hword how_finger[]){
	hword i;
	hword j;

	for(i = 0; i < 4; i++){

		if(how_finger[i] == 0){
			for(j = 4 + 44 * i; j < 4 + 44 * (i + 1) - 1; j++){
				draw_point(j, Y_JUDGELINE - 1, BLACK);
				draw_point(j, Y_JUDGELINE, YELLOW);
				draw_point(j, Y_JUDGELINE + 1, BLACK);
			}

		} else {
			for(j = 4 + 44 * i; j < 4 + 44 * (i + 1) - 1; j++){
				draw_point(j, Y_JUDGELINE - 1, RED);
				draw_point(j, Y_JUDGELINE, RED);
				draw_point(j, Y_JUDGELINE + 1, RED);
			}
		}
	}
}

// Readyを表示
void display_ready(){
	hword time;
	*((unsigned short *)0x0400010E) = 0x0084;     // タイムスイッチON
	

	locate(9, 7);
	
	while(1){
		time = *((unsigned short *)0x0400010C); 

		if(time > 600){
			prints("     ", RED);
			break;
		} else if(time > 500){
			prints("  1  ", RED);
		} else if(time > 400){
			prints("  2  ", RED);
		} else if(time > 300){
			prints("  3  ", RED);
		} else if(time > 0){
			prints("ready", RED);
		}
	}

	*((unsigned short *)0x0400010E) = 0x0004;     // タイムスイッチOFF
	*((unsigned short *)0x0400010C) = 0;
	
}

// 時間を表示
void display_time(hword time){

	hword dsec = mod(time, 100);
	hword sec = mod(div(time, 100), 60);
	hword min = mod(div(time, 6000), 60);

	hword x_time = 23;
	hword y_time = 0;

	locate(x_time, y_time);
	num_to_string(min, 1, WHITE);

	locate(x_time + 1, y_time);
	prints(":", WHITE);

	locate(x_time + 2, y_time);
	num_to_string(sec, 2, WHITE);

	locate(x_time + 4, y_time);
	prints(":", WHITE);

	locate(x_time + 5, y_time);
	num_to_string(dsec, 2, WHITE);

}

// スコアセットを表示
void display_score_set(){
	hword x_score = 23;
	hword y_score = 2;

	locate(x_score, y_score);
	prints("SC:", WHITE);

	locate(x_score, y_score + 2);
	prints("BE:", RED);

	locate(x_score, y_score + 4);
	prints("CL:", YELLOW);

	locate(x_score, y_score + 6);
	prints("GD:", GREEN);

	locate(x_score, y_score + 8);
	prints("MS:", BLUE);
}

// スコアを表示
void display_score(hword best, hword cool, hword good, hword miss){
	hword sum_note = best + cool + good + miss;
	hword score    = 10 * best + 7 * cool + 3 * good + 0 * miss;
	hword x_score = 23;
	hword y_score = 2;

	locate(x_score + 2, y_score + 1);
	num_to_string(score, 5, WHITE);

	locate(x_score + 3, y_score + 3);
	num_to_string(best, 4, RED);

	locate(x_score + 3, y_score + 5);
	num_to_string(cool, 4, YELLOW);

	locate(x_score + 3, y_score + 7);
	num_to_string(good, 4, GREEN);

	locate(x_score + 3, y_score + 9);
	num_to_string(miss, 4, BLUE);
}

// 押したボタンを表示
void display_button(hword x, hword y, hword how_finger[]){
	int i;
	hword color_button;

	for(i = 0; i < 4; i++){
		locate(x + i, y);

		switch(how_finger[i]){
			case 0: color_button = WHITE; break;
			case 1: color_button = RED; break;
			case 2: color_button = RED; break;
		}

		switch(i){
			case 0: prints("D", color_button); break;
			case 1: prints("F", color_button); break;
			case 2: prints("J", color_button); break;
			case 3: prints("K", color_button); break;
		}
	}

}

// BEST判定を表示
void display_best(){
	locate(9, 6);
	prints("BE", RED);
	locate(12, 6);
	prints("ST", RED);
}

// COOL判定を表示
void display_cool(){
	locate(9, 6);
	prints("CO", YELLOW);
	locate(12, 6);
	prints("OL", YELLOW);
}

// GOOD判定を表示
void display_good(){
	locate(9, 6);
	prints("GO", GREEN);
	locate(12, 6);
	prints("OD", GREEN);
}

// MISS判定を表示
void display_miss(){
	locate(9, 6);
	prints("MI", BLUE);
	locate(12, 6);
	prints("SS", BLUE);
}

// COMBOを表示
void display_combo(hword combo){
	locate(9, 8);
	num_to_string(combo, 5, WHITE);

}

// ライフを表示
void display_life(hword life){
	hword xi;
	hword yi;

	hword right_life = 225;
	hword left_life = 200;
	hword bottom_life = 135;
	hword top_life = 100;
	hword rest_life = 135 - life;

	for(yi = top_life; yi <= bottom_life + 1; yi++){
		draw_point((left_life  - 1), yi, WHITE);
		draw_point((right_life + 1), yi, WHITE);
		if(yi == bottom_life + 1){
			for(xi = left_life; xi <= right_life; xi++){
				draw_point(xi, yi, WHITE);
			}
		}
	}

	for(yi = top_life; yi <= bottom_life; yi++){
		for(xi = left_life; xi <= right_life; xi++){
			if(yi < rest_life){
				draw_point(xi, yi, BLACK);
			} else {
				draw_point(xi, yi, MAGENTA);
			}
		}
	}
}


// キー入力を判断
hword judge_key(){

	hword *ptr_key;
	hword state_key;
	hword what_key;

	ptr_key = (hword*)KEY;
	state_key = *ptr_key;
	what_key = ~state_key & KEY_ALL;

	return what_key;
}


// 鍵盤0の状態を012で評価
hword judge_finger0(hword pre_what_key, hword what_key){

	
	if((pre_what_key & KEY_L) != KEY_L && (what_key & KEY_L) == KEY_L){
		return 1;

	} else if((pre_what_key & KEY_L) == KEY_L && (what_key & KEY_L) == KEY_L){
		return 2;

	} else {
		return 0;
	}

}

// 鍵盤1の状態を012で評価
hword judge_finger1(hword pre_what_key, hword what_key){

	
	if((pre_what_key & KEY_A) != KEY_A && (what_key & KEY_A) == KEY_A){
		return 1;

	} else if((pre_what_key & KEY_A) == KEY_A && (what_key & KEY_A) == KEY_A){
		return 2;

	} else {
		return 0;
	}

}

// 鍵盤2の状態を012で評価
hword judge_finger2(hword pre_what_key, hword what_key){

	
	if((pre_what_key & KEY_B) != KEY_B && (what_key & KEY_B) == KEY_B){
		return 1;

	} else if((pre_what_key & KEY_B) == KEY_B && (what_key & KEY_B) == KEY_B){
		return 2;

	} else {
		return 0;
	}

}

// 鍵盤3の状態を012で評価
hword judge_finger3(hword pre_what_key, hword what_key){

	
	if((pre_what_key & KEY_R) != KEY_R && (what_key & KEY_R) == KEY_R){
		return 1;

	} else if((pre_what_key & KEY_R) == KEY_R && (what_key & KEY_R) == KEY_R){
		return 2;

	} else {
		return 0;
	}

}

// タイトル画面を表示
void display_title(){
	hword i;

	paint_all(BLACK);

	locate(0, 0); prints("#include", GREEN); locate(9, 0); prints("<cold.io>", WHITE);
	locate(0, 1); prints("#define", GREEN); locate(8, 1); prints("ID door", WHITE);

	locate(0, 3); prints("int     (){", WHITE); locate(4, 3); prints("main", RED); 
	locate(2, 4); prints("welcome", CYAN); locate(9, 4); prints("(!      ()){", WHITE); locate(11, 4); prints("mygame", MAGENTA);
	locate(4, 5); prints("for", CYAN); locate(7, 5); prints("(your : playing){", WHITE);

	locate(2, 7); prints("!!!", RED);
	locate(6, 7); prints("Please", YELLOW); locate(12, 7); prints(".", WHITE); locate(13, 7); prints("change", MAGENTA);
											locate(19, 7); prints("(      );", WHITE); locate(20, 7); prints("config", GREEN);
	locate(6, 8); prints("from", CYAN); locate(10, 8); prints("(       (      )){", WHITE); locate(11, 8); prints("Options", MAGENTA);
																							locate(19, 8); prints("joypad", GREEN);
	
	locate(8, 10); prints("key.   >>  key. ;", WHITE); locate(12, 10); prints("A", GREEN); locate(23, 10); prints("F", GREEN);
	locate(8, 11); prints("key.   >>  key. ;", WHITE); locate(12, 11); prints("B", GREEN); locate(23, 11); prints("J", GREEN);
	locate(8, 12); prints("key.   >>  key. ;", WHITE); locate(12, 12); prints("L", GREEN); locate(23, 12); prints("D", GREEN);
	locate(8, 13); prints("key.   >>  key. ;", WHITE); locate(12, 13); prints("R", GREEN); locate(23, 13); prints("K", GREEN);

	locate(6, 14); prints("}", WHITE);
	locate(4, 15); prints("}", WHITE);
	locate(2, 16); prints("}", WHITE);
	locate(2, 17); prints("return", CYAN); locate(15, 17); prints(".          ;", WHITE); 
	locate(0, 18); prints("}", WHITE); locate(16, 18); prints("//(LABR)", GREEN);

}

// START画面を始める
void begin_start(){
	
	hword what_key;          // キー入力
	hword pre_what_key;      // 一周前のキー入力
	hword how_finger[4] = {0, 0, 0, 0};    // それぞれの鍵盤の状態、押してない０、押す１、長押し２

	hword color_push;

	hword time_note;

	display_title();

	while(1){

		pre_what_key = what_key;
		what_key = judge_key();

		// 鍵盤入力の状態を、押してない0, 押した1, 長押し2で判断
		how_finger[0] = judge_finger0(pre_what_key, what_key);
		how_finger[1] = judge_finger1(pre_what_key, what_key);
		how_finger[2] = judge_finger2(pre_what_key, what_key);
		how_finger[3] = judge_finger3(pre_what_key, what_key);

		if(time_note < 100){
			color_push = WHITE;
		} else {
			color_push = RED;
		}

		time_note ++;
		if(time_note == 200){
			time_note = 0;
		}

		locate(9, 17);
		prints("PUSH ALL [", color_push);
		locate(23, 17);
		prints("]", color_push);
		display_button(19, 17, how_finger);

		if((what_key & (KEY_ABLR)) == KEY_ABLR){
			break;
		}

	}
}

// TUTORIAL画面を始める
void begin_tutorial(){
	hword i;
	
	hword pre_what_key;
	hword what_key;

	hword color_yes;
	hword color_no;
	hword state_select = 1;

	point p_note;
	p_note.x = 30;
	p_note.y = 10;


	paint_all(BLACK);
	delay(800);

	locate(2, 4);
	prints(" Config is OK. Thank you!", YELLOW);
	delay(500);
	
	locate(2, 6);
	prints("     This is like a      ", YELLOW);
	delay(500);

	locate(2, 8);
	prints("       RHYTHM GAME       ", CYAN);
	delay(500);
	
	locate(2, 10);
	prints("     Play a TUTORIAL?    ", GREEN);
	delay(500);

	while(1){
		
		pre_what_key = what_key;
		what_key = judge_key();
		
		if((pre_what_key & KEY_LEFT) != KEY_LEFT && (what_key & KEY_LEFT) == KEY_LEFT){
			state_select = ~state_select;
		} else if ((pre_what_key & KEY_RIGHT) != KEY_RIGHT && (what_key & KEY_RIGHT) == KEY_RIGHT){
			state_select = ~state_select;
		}

		if(state_select == 1){
			color_yes = RED;
			color_no = WHITE;
		} else {
			color_yes = WHITE;
			color_no = RED;
		}

		locate(9, 12); prints("YES", color_yes);
		locate(18, 12); prints("NO", color_no);

		if((what_key & KEY_START) == KEY_START){
			what_key = 0;
			break;
		}
	}

	if(state_select == 1){
		paint_all(BLACK);
		delay(800);

		locate(1, 4);
		prints("Sorry.The colddoor has been ", YELLOW);
		delay(500);
	
		locate(1, 6);
		prints("lazy and the TUTORIAL is not", YELLOW);
		delay(500);

		locate(1, 8);
		prints("         ready yet!        ", YELLOW);
		delay(500);
	
		locate(1, 10);
		prints("  It's a simple game with  ", YELLOW);
		delay(500);

		locate(1, 12);
		prints("    only D,F,J,L button.   ", YELLOW);
		delay(500);

		locate(1, 14);
		prints("   I hope you understand.  ", YELLOW);
		delay(500);


		while(1){
			pre_what_key = 0;
			what_key = 0;

			pre_what_key = what_key;
			what_key = judge_key();
		
			locate(22, 16);
			prints("next>>", WHITE);

			if(pre_what_key != what_key){
				break;
			}
		}
		
	}
}

// helpを表示
void display_help(){
	hword i;
	hword xi;
	hword yi;
	
	
	hword pre_what_key = 0;
	hword what_key = 0;
	
	
	hword left  = 25;
	hword right = 214;
	hword up    = 25;
	hword down  = 134;

	hword page = 0;
	point ex_note;
	ex_note.x = 50;
	ex_note.y = 108;

	
	for(yi = up; yi <= down; yi++){
		draw_point(left, yi, WHITE);
		for(xi = (left + 1); xi <= (right - 1); xi++){
			if(yi == up || yi == down){
				draw_point(xi, yi, WHITE);
			} else {
				draw_point(xi, yi, BLACK);
			}
		}
		draw_point(right, yi, WHITE);
	}
	locate(4, 3);
	prints("HELP(exit with D or K)", WHITE);

	locate(22, 16);
	prints("next>>", WHITE);

	locate(3, 16);
	prints("<<prev", WHITE);

	while(1){
		pre_what_key = what_key;
		what_key = judge_key();

		if((pre_what_key & KEY_LEFT) != KEY_LEFT && (what_key & KEY_LEFT) == KEY_LEFT){
			if(page == 0){
				page = 3;
			} else {
				page --;
			}

		} else if((pre_what_key & KEY_RIGHT) != KEY_RIGHT && (what_key & KEY_RIGHT) == KEY_RIGHT){
			if(page == 3){
				page = 0;
			} else {
				page ++;
			}
		}

		
		if(page == 0){
			
			locate(5, 4);
			prints("     <PATTERN>       ", YELLOW);
			locate(5, 6);
			prints("STAIRS               ", CYAN);
			locate(5, 7);
			prints("NOTE come like stairs", WHITE);
			locate(5, 8);
			prints("                     ", GREEN);
			locate(5, 9);
			prints("OTHERS               ", CYAN);;
			locate(5, 10);
			prints("Coming soon          ", WHITE);
			locate(5, 11);
			prints("                     ", WHITE);

			draw_note(ex_note, 0);
			locate(9, 13);
			print_ascii(27, WHITE);
			locate(10, 13);
			prints("This is NOTE.", WHITE);
			locate(10, 14);
			prints("Press the button", WHITE);
			locate(10, 15);
			prints("for this.", WHITE);

		} else if(page == 1){
			
			locate(5, 4);
			prints("      <SPEED>        ", YELLOW);
			locate(5, 6);
			prints("You can set the speed", GREEN);
			locate(5, 7);
			prints(" at which NOTE move. ", GREEN);
			locate(5, 8);
			prints("                     ", GREEN);
			locate(5, 9);
			prints("  Recommended SPEED  ", WHITE);
			locate(5, 10);
			prints("        is 4         ", WHITE);
			locate(5, 11);
			prints("                     ", WHITE);
			
		} else if(page == 2){
			
			locate(5, 4);
			prints("     <INTERVAL>      ", YELLOW);
			locate(5, 6);
			prints("You can set interval ", GREEN);
			locate(5, 7);
			prints(" at which NOTE come. ", GREEN);
			locate(5, 8);
			prints("                     ", GREEN);
			locate(5, 9);
			prints("Recommended INTERVAL ", WHITE);
			locate(5, 10);
			prints("       is 1/6        ", WHITE);
			locate(5, 11);
			prints("                     ", WHITE);

		} else if(page == 3){
			
			locate(5, 4);
			prints("       <TIME>        ", YELLOW);
			locate(5, 6);
			prints("GAME time can be set.", GREEN);
			locate(5, 7);
			prints("If you survive TIME, ", GREEN);
			locate(5, 8);
			prints("you will GAME CLEAR. ", GREEN);
			locate(5, 9);
			prints("                     ", WHITE);
			locate(5, 10);
			prints("  Recommended TIME   ", WHITE);
			locate(5, 11);
			prints("       is 30s        ", WHITE);
		}
		
		if((what_key & KEY_L) == KEY_L || (what_key & KEY_R) == KEY_R){
			break;
		}
	}

	left  = 12;
	right = 227;
	up    = 12;
	down  = 147;

	paint_all(BLUE);
	for(yi = up; yi <= down; yi++){
		draw_point(left, yi, WHITE);
		for(xi = (left + 1); xi <= (right - 1); xi++){
			if(yi == up || yi == down){
				draw_point(xi, yi, WHITE);
			} else {
				draw_point(xi, yi, BLACK);
			}
		}
		draw_point(right, yi, WHITE);
	}
	locate(2, 1);
	prints("OPTION", WHITE);
}

// OPTIONを始める
void begin_op(){
	
	hword i;
	hword xi;
	hword yi;
	
	
	hword pre_what_key = 0;
	hword what_key = 0;
	
	
	hword left  = 12;
	hword right = 227;
	hword up    = 12;
	hword down  = 147;

	hword ud_select = 0;
	hword lr_select[4];
	for(i = 0; i < 4; i++){
		lr_select[i] = 0;
	}

	hword color_help = BLUE;
	hword color_mod[5];
	hword color_left[5];
	hword color_right[5];
	
	for(i = 0; i < 5; i++){
		color_mod[i] = 0;
		color_left[i] = 0;
		color_right[i] = 0;
	}

	paint_all(BLUE);
	
	/*
	for(xi = left; xi <= right; xi++){
		draw_point(xi, up, WHITE);
	}

	for(yi = up; yi <= down; yi++){
		draw_point(right, yi, WHITE);
	}

	for(xi = right; xi >= left; xi--){
		draw_point(xi, down, WHITE);
	}

	for(yi = down; yi >= up; yi--){
		draw_point(left, yi, WHITE);
	}
	*/

	for(yi = up; yi <= down; yi++){
		draw_point(left, yi, WHITE);
		for(xi = (left + 1); xi <= (right - 1); xi++){
			if(yi == up || yi == down){
				draw_point(xi, yi, WHITE);
			} else {
				draw_point(xi, yi, BLACK);
			}
		}
		draw_point(right, yi, WHITE);
	}
	
	locate(2, 1);
	prints("OPTION", WHITE);

	while(1){

		pre_what_key = what_key;
		what_key = judge_key();

		color_help = BLUE;
		
		for(i = 0; i < 5; i++){
			color_mod[i] = WHITE;
			color_left[i]  = WHITE;
			color_right[i] = WHITE;
			
		}

		if((pre_what_key & KEY_UP) != KEY_UP && (what_key & KEY_UP) == KEY_UP){
			if(ud_select != 0){
				ud_select --;
			}
		}

		if((pre_what_key & KEY_DOWN) != KEY_DOWN && (what_key & KEY_DOWN) == KEY_DOWN){
			if(ud_select != 4){
				ud_select ++;
			}
		}

		if(ud_select != 4){
			color_mod[ud_select] = GREEN;
			color_left[ud_select] = GREEN;
			color_right[ud_select] = GREEN;
		} else {
			color_mod[ud_select] = RED;
			color_left[ud_select] = RED;
			color_right[ud_select] = RED;
		}

		if((pre_what_key & KEY_LEFT) != KEY_LEFT && (what_key & KEY_LEFT) == KEY_LEFT){
			switch(ud_select){
				case 0:
					if(lr_select[ud_select] == 0){
						lr_select[ud_select] = 3;
					} else {
						lr_select[ud_select] --;
					}
					break;

				case 1:
					if(lr_select[ud_select] == 0){
						lr_select[ud_select] = 6;
					} else {
						lr_select[ud_select] --;
					}
					break;
					
				case 2:
					if(lr_select[ud_select] == 0){
						lr_select[ud_select] = 8;
					} else {
						lr_select[ud_select] --;
					}
					break;

				case 3:
					if(lr_select[ud_select] == 0){
						lr_select[ud_select] = 4;
					} else {
						lr_select[ud_select] --;
					}
					break;
			}
			
		}

		if((what_key & KEY_LEFT) == KEY_LEFT){
			color_left[ud_select] = RED;
		}

		if((pre_what_key & KEY_RIGHT) != KEY_RIGHT && (what_key & KEY_RIGHT) == KEY_RIGHT){
			switch(ud_select){
				case 0:
					if(lr_select[ud_select] == 3){
						lr_select[ud_select] = 0;
					} else {
						lr_select[ud_select] ++;
					}
					break;

				case 1:
					if(lr_select[ud_select] == 6){
						lr_select[ud_select] = 0;
					} else {
						lr_select[ud_select] ++;
					}
					break;

				case 2:
					if(lr_select[ud_select] == 8){
						lr_select[ud_select] = 0;
					} else {
						lr_select[ud_select] ++;
					}
					break;

				case 3:
					if(lr_select[ud_select] == 4){
						lr_select[ud_select] = 0;
					} else {
						lr_select[ud_select] ++;
					}
					break;
			}
			
		}

		if((what_key & KEY_RIGHT) == KEY_RIGHT){
			color_right[ud_select] = RED;
		}

		locate(4, 3);
		prints("Push F or J to see HELP", color_help);

		locate(4, 6);
		prints("PATTERN", color_mod[0]);
		locate(15, 6);
		prints("<", color_left[0]);
		locate(24, 6);
		prints(">", color_right[0]);
		switch(lr_select[0]){
			case 0:
				locate(16, 6);
				prints(" STAIRS ", color_mod[0]);
				pattern_note = SPEED;
				break;
			case 1:
				locate(16, 6);
				prints(" STREAM ", color_mod[0]);
				pattern_note = STAMINA;
				break;
			case 2:
				locate(16, 6);
				prints("  JACK  ", color_mod[0]);
				pattern_note = JACK;
				break;
			case 3:
				locate(16, 6);
				prints("  TECH  ", color_mod[0]);
				pattern_note = TECH;
				break;
		}

		locate(4, 8);
		prints("SPEED", color_mod[1]);
		locate(15, 8);
		prints("<", color_left[1]);
		locate(23, 8);
		prints(">", color_right[1]);
		switch(lr_select[1]){
			case 0:
				locate(16, 8);
				prints("   1   ", color_mod[1]);
				speed = 1;
				break;
			case 1:
				locate(16, 8);
				prints("   2   ", color_mod[1]);
				speed = 3;
				break;
			case 2:
				locate(16, 8);
				prints("   3   ", color_mod[1]);
				speed = 3;
				break;
			case 3:
				locate(16, 8);
				prints("   4   ", color_mod[1]);
				speed = 4;
				break;
			case 4:
				locate(16, 8);
				prints("   6   ", color_mod[1]);
				speed = 6;
				break;
			case 5:
				locate(16, 8);
				prints("   8   ", color_mod[1]);
				speed = 8;
				break;
			case 6:
				locate(16, 8);
				prints("   9   ", color_mod[1]);
				speed = 9;
				break;
		}

		locate(4, 10);
		prints("INTERVAL", color_mod[2]);
		locate(15, 10);
		prints("<", color_left[2]);
		locate(23, 10);
		prints(">", color_right[2]);
		switch(lr_select[2]){
			case 0:
				locate(16, 10);
				prints("  1/2  ", color_mod[2]);
				interval_note = 32;
				break;
			case 1:
				locate(16, 10);
				prints("  1/4  ", color_mod[2]);
				interval_note = 16;
				break;
			case 2:
				locate(16, 10);
				prints("  1/6  ", color_mod[2]);
				interval_note = 12;
				break;
			case 3:
				locate(16, 10);
				prints("  1/8  ", color_mod[2]);
				interval_note = 8;
				break;
			case 4:
				locate(16, 10);
				prints(" 1/12  ", color_mod[2]);
				interval_note = 6;
				break;
			case 5:
				locate(16, 10);
				prints(" 1/16  ", color_mod[2]);
				interval_note = 4;
				break;
			case 6:
				locate(16, 10);
				prints(" 1/24  ", color_mod[2]);
				interval_note = 3;
				break;
			case 7:
				locate(16, 10);
				prints(" 1/32  ", color_mod[2]);
				interval_note = 2;
				break;
			case 8:
				locate(16, 10);
				prints(" 1/64  ", color_mod[2]);
				interval_note = 1;
				break;
		}

		locate(4, 12);
		prints("TIME", color_mod[3]);
		locate(15, 12);
		prints("<", color_left[3]);
		locate(23, 12);
		prints(">", color_right[3]);
		switch(lr_select[3]){
			case 0:
				locate(16, 12);
				prints("  30s  ", color_mod[3]);
				limit_time = 3000;
				break;
			case 1:
				locate(16, 12);
				prints("  60s  ", color_mod[3]);
				limit_time = 6000;
				break;
			case 2:
				locate(16, 12);
				prints("  90s  ", color_mod[3]);
				limit_time = 9000;
				break;
			case 3:
				locate(16, 12);
				prints(" 120s  ", color_mod[3]);
				limit_time = 12000;
				break;
			case 4:
				locate(16, 12);
				prints("endless", color_mod[3]);
				limit_time = 0;
				break;
		}

		locate(21, 16);
		prints("START!", color_mod[4]);

		if((what_key & KEY_A) == KEY_A || (what_key & KEY_B) == KEY_B){
			display_help();
		}

		
		if((ud_select == 4) && ((what_key & KEY_START) == KEY_START)){
			if(pattern_note == STAMINA){
				locate(3, 14);
				prints("Error:", RED);
				locate(3, 15);
				prints("PATTERN STREAM", YELLOW);
				locate(4, 16);
				prints("is coming soon!", YELLOW);
			} else if(pattern_note == JACK){
				locate(3, 14);
				prints("Error:", RED);
				locate(3, 15);
				prints("PATTERN JACK   ", YELLOW);
				locate(4, 16);
				prints("is coming soon!", YELLOW);
			} else if(pattern_note == TECH){
				locate(3, 14);
				prints("Error:", RED);
				locate(3, 15);
				prints("PATTERN TECH   ", YELLOW);
				locate(4, 16);
				prints("is coming soon!", YELLOW);
			} else {
				break;
			}
			
		}
		
	}
	
}


// GAMEを始める
void begin_game(){
	hword i;
	hword j;
	hword k;
	

	point p_note[NUM_OF_LANE][MAX_LANE_NOTES];     // ノーツの座標
	hword is_note[NUM_OF_LANE][MAX_LANE_NOTES];    // ノーツを表示するかどうか
	hword judge_note[NUM_OF_LANE][MAX_LANE_NOTES]; // ノーツの判定用
	
	// ノーツの座標を初期化
	for(i = 0; i < NUM_OF_LANE; i++){
		for(j = 0; j < MAX_LANE_NOTES; j++){
			p_note[i][j].x = X_NOTE_0 + WIDTH_LANE * i;
			p_note[i][j].y = Y_START_NOTE;
			is_note[i][j] = 0;
			judge_note[i][j] = 0;
		}
	}
	
	
	hword what_key = 0;          // キー入力
	hword pre_what_key = 0;      // 一周前のキー入力
	hword how_finger[4] = {0, 0, 0, 0};    // それぞれの鍵盤の状態、押してない０、押す１、長押し２


	// スコアセット
	best = 0;
	cool = 0;
	good = 0;
	miss = 0;

	hword time = 0;        // 時間
	hword rest_time = 0;

	hword current_lane = 0;             // 現在参照している鍵盤
	hword count_note[NUM_OF_LANE] = {0,0,0,0};  // レーン毎のノーツ番号


	hword combo = 0;                    // コンボを数える

	hword time_note = 0;          // while一周毎にインクリメントで動作の時間を計測

	hword seed = 0;               // 乱数のタネ
	hword rand = 0;               // 乱数

	hword life = 35;

	// 一度きりの表示
	paint_all(BLACK);
	draw_keylane();        // 鍵盤のレーンを表示
	display_score_set();   // スコアセットを表示

	if(limit_time != 0){
		display_time(limit_time);
	} else {
		display_time(0);
	}
	
	display_life(life);
	display_score(best, cool, good, miss);
	display_button(25, 18, how_finger);
	draw_judgeline(how_finger);
	display_ready();
	draw_keylane();        // 鍵盤のレーンを表示

	

	// GAMEフェーズ繰り返しの動作
	while(1){

		*((unsigned short *)0x0400010E) = 0x0084;     // タイムスイッチON
		
		time = *((unsigned short *)0x0400010C);       // GBA内のタイマ（0.01秒刻み）
		pre_what_key = what_key;   // 一周前のキー入力を保持
		what_key = judge_key();    // キー入力を判定
		// 乱数を生成
		// whileループ内で加算され続けるrandは鍵盤が押されると固定される
		if(what_key == pre_what_key){
			rand ++;
		}
		// locate(23, 14);
		// num_to_string(rand, 3, WHITE);
			
		// 鍵盤入力の状態を、押してない0, 押した1, 長押し2で判断
		how_finger[0] = judge_finger0(pre_what_key, what_key);
		how_finger[1] = judge_finger1(pre_what_key, what_key);
		how_finger[2] = judge_finger2(pre_what_key, what_key);
		how_finger[3] = judge_finger3(pre_what_key, what_key);
			// 判定線を表示
		draw_judgeline(how_finger);
			
		// is_noteが1のノーツを流し続ける
		for(i = 0; i < NUM_OF_LANE; i++){          // ここでiはレーンの番号
			for(j = 0; j < MAX_LANE_NOTES; j++){   // jは各レーンにおけるノーツの番号を表す
				if(is_note[i][j] == 1){             // is_noteが1なら
					delete_note(p_note[i][j], i);   // ノーツを消す
					p_note[i][j].y += speed;        // 座標を下にずらす
					draw_note(p_note[i][j], i);     // 描画
					judge_note[i][j] ++;            // 判定インクリメント
				}

				

				if(p_note[i][j].y >= 198){          // ノーツのy座標198以上で
					p_note[i][j].y = Y_START_NOTE;  // そのノーツのy座標を初期化
					is_note[i][j] = 0;              // is_noteを0にする
					judge_note[i][j] = 0;           // 判定初期化
					miss ++;                        // MISSを加算
					combo = 0;                      // コンボを0にする
					if(life == 1){
						life -= 1;
					} else {
						life -= 2;
					}
					display_miss();                 // MISSを表示
				}
			}
		}

		// 鍵盤を押したときに判定
		// 判定線をspeedで割ったもの(ノーツが判定線に到達するまでの時間)と、
		// ノーツが発生してからの時間を比較して判定
		for(i = 0; i < NUM_OF_LANE; i++){          // ここでiはレーンの番号
			for(j = 0; j < MAX_LANE_NOTES; j++){   // jは各レーンにおけるノーツの番号を表す
				if(how_finger[i] == 1){
					// 誤差±0でBEST判定
					if (div(Y_JUDGELINE, speed) - 0 <= judge_note[i][j] && judge_note[i][j] <= div(Y_JUDGELINE, speed) + 0){
						best ++;                         // bestインクリメント
						delete_note(p_note[i][j], i);    // ノーツを消す
						p_note[i][j].y = Y_START_NOTE;   // ノーツのy座標を初期化
						is_note[i][j] = 0;               // is_noteを0にする
						judge_note[i][j] = 0;            // 判定初期化
						combo ++;                        // コンボインクリメント
						if(life == 35){
							life += 0;
						} else if(life == 34){
							life += 1;
						} else {
							life += 2;
						}
						display_best();                  // BESTを表示
						break;

					// 誤差±1でCOOL判定, 動作はBESTの時と同じ
					} else if (div(Y_JUDGELINE, speed) - 2 <= judge_note[i][j] && judge_note[i][j] <= div(Y_JUDGELINE, speed) + 2){
						cool ++;
						delete_note(p_note[i][j], i);
						p_note[i][j].y = Y_START_NOTE;
						is_note[i][j] = 0;
						judge_note[i][j] = 0;
						combo ++;
						display_cool();
						break;

					// 誤差±2でGOOD判定, 動作はBESTの時と同じ
					} else if (div(Y_JUDGELINE, speed) - 4 <= judge_note[i][j] && judge_note[i][j] <= div(Y_JUDGELINE, speed) + 4){
						good ++;
						delete_note(p_note[i][j], i);
						p_note[i][j].y = Y_START_NOTE;
						is_note[i][j] = 0;
						judge_note[i][j] = 0;
						display_good();
						break;
					}
				}
			}
		}


		// 定めた間隔でノーツを生成する
		if(mod(time_note, interval_note) == 0){
			
			// パターンSPEEDここから ----------------------------------------------------------------
			if(pattern_note == SPEED){

			

				if(current_lane == 0){ // ノーツがレーン0なら次はレーン1に
					if(mod(rand, 3) == 0){         
						current_lane = 3;
					} else {
						current_lane = 1;
					}
					
				} else if(current_lane == 3){              // ノーツがレーン3なら次はレーン2に
					if(mod(rand, 3) == 0){         
						current_lane = 0;
					} else {
						current_lane = 2;
					}

				} else if(mod(rand, 2) == 0){         // ノーツがレーン1か2なら次は左右ランダムに1ずらす
					current_lane ++;
				} else if(mod(rand, 2) == 1){
					current_lane --;
				}

				is_note[current_lane][count_note[current_lane]] = 1;     // 現在参照のレーンにノーツが現れるようにする
				count_note[current_lane] ++;        // ノーツ番号インクリメント

			}

			// パターンSPEEDここまで ----------------------------------------------------------------


			
			// パターンSTAMINAここから ----------------------------------------------------------------
			if(pattern_note == STAMINA){
					
			}
			// パターンSTAMINAここまで ----------------------------------------------------------------
			
				
		}

		for(i = 0; i < NUM_OF_LANE; i++){
			if(count_note[i] == MAX_LANE_NOTES){
				count_note[i] = 0;
			}
		}
			
			

		if(limit_time != 0){
			rest_time = limit_time - time;
			display_time(rest_time);
		} else {
			display_time(time);
		}
		
		
		display_score(best, cool, good, miss);
		display_button(25, 18, how_finger);
		display_life(life);
		display_combo(combo);
		draw_judgeline(how_finger);

		time_note ++;
		// locate(23, 0);
		// num_to_string(time_note, 5, RED);

		if(life == 0){
			paint_all(BLACK);
			
			if(limit_time != 0){
				rest_time = limit_time - time;
				display_time(rest_time);
			} else {
				display_time(time);
			}
			display_score(best, cool, good, miss);
			display_button(25, 18, how_finger);
			display_life(life);
			draw_judgeline(how_finger);

			locate(5, 6);
			prints("LIFE is gone.", RED);
			locate(7, 8);
			prints("GAME OVER", RED);
			is_clear = 0;
			delay(1500);
			while(1){
				pre_what_key = 0;
				what_key = 0;

				pre_what_key = what_key;
				what_key = judge_key();
		
				locate(15, 10);
				prints("next>>", WHITE);

				if(pre_what_key != what_key){
					break;
				}
			}
			break;
		}

		if(limit_time != 0){
			if(rest_time <= 10){
				paint_all(BLACK);

				display_time(0);
				display_score(best, cool, good, miss);
				display_button(25, 18, how_finger);
				display_life(life);
				draw_judgeline(how_finger);

				locate(3, 6);
				prints("Survived in TIME.", YELLOW);
				locate(7, 8);
				prints("GAME CLEAR", YELLOW);
				is_clear = 1;
				delay(1500);

				while(1){
					pre_what_key = 0;
					what_key = 0;

					pre_what_key = what_key;
					what_key = judge_key();
		
					locate(15, 10);
					prints("next>>", WHITE);

					if(pre_what_key != what_key){
						break;
					}
				}
				break;
			}
		}

	}
		
}

// 背景をカラフルにする
void paint_colorful(){
	hword xi;
	hword yi;

	for(yi = 0; yi < 160; yi++){
		for(xi = 0; xi < 240; xi++){
			if(yi < 40){
				draw_point(xi, yi, RED);
			} else if(yi < 80){
				draw_point(xi, yi, YELLOW);
			} else if(yi < 120){
				draw_point(xi, yi, GREEN);
			} else if(yi < 160){
				draw_point(xi, yi, BLUE);
			}
		}
	}
}

// フェーズENDを開始する
void begin_end(){
	hword i;
	hword xi;
	hword yi;

	hword pre_what_key;
	hword what_key;

	hword left  = 12;
	hword right = 227;
	hword up    = 12;
	hword down  = 147;

	hword sum_note = best + cool + good + miss;
	hword score    = 10 * best + 7 * cool + 3 * good + 0 * miss;
	hword x_score = 20;
	hword y_score = 3;

	hword state_select = 0;
	hword color_select[3] = {0, 0, 0};
	
	paint_colorful();
	
	for(yi = up; yi <= down; yi++){
		draw_point(left, yi, WHITE);
		for(xi = (left + 1); xi <= (right - 1); xi++){
			if(yi == up || yi == down){
				draw_point(xi, yi, WHITE);
			} else {
				draw_point(xi, yi, BLACK);
			}
		}
		draw_point(right, yi, WHITE);
	}

	locate(2, 1);
	prints("RESULT", WHITE);

	locate(3, 3);
	prints("PATTERN:", WHITE); locate(11, 3); prints("STAIRS", CYAN);

	locate(3, 5);
	prints("SPEED:", WHITE); locate(9, 5); num_to_string(speed, 1, CYAN);

	locate(3, 7);
	prints("INTERVAL:", WHITE);
	locate(12, 7); 
	switch(interval_note){
		case 32:
			prints("1/2", CYAN);
			break;
		case 16:
			prints("1/4", CYAN);
			break;
		case 12:
			prints("1/6", CYAN);
			break;
		case 8:
			prints("1/8", CYAN);
			break;
		case 6:
			prints("1/12", CYAN);
			break;
		case 4:
			prints("1/16", CYAN);
			break;
		case 3:
			prints("1/24", CYAN);
			break;
		case 2:
			prints("1/32", CYAN);
			break;
		case 1:
			prints("1/64", CYAN);
			break;
	}

	locate(3, 9);
	prints("TIME:", WHITE);
	locate(8, 9);
	switch(limit_time){
		case 3000:
			prints("30s", CYAN);
			break;
		case 6000:
			prints("60s", CYAN);
			break;
		case 9000:
			prints("90s", CYAN);
			break;
		case 12000:
			prints("120s", CYAN);
			break;
		case 0:
			prints("endless", CYAN);
			break;
	}

	locate(x_score, y_score);
	prints("SC:", WHITE);

	locate(x_score, y_score + 2);
	prints("BE:", RED);

	locate(x_score, y_score + 4);
	prints("CL:", YELLOW);

	locate(x_score, y_score + 6);
	prints("GD:", GREEN);

	locate(x_score, y_score + 8);
	prints("MS:", BLUE);

	locate(x_score + 2, y_score + 1);
	num_to_string(score, 5, WHITE);

	locate(x_score + 3, y_score + 3);
	num_to_string(best, 4, RED);

	locate(x_score + 3, y_score + 5);
	num_to_string(cool, 4, YELLOW);

	locate(x_score + 3, y_score + 7);
	num_to_string(good, 4, GREEN);

	locate(x_score + 3, y_score + 9);
	num_to_string(miss, 4, BLUE);

	locate(6, 12);
	if(is_clear == 0){
		prints("FAILURE...", BLUE);
	} else {
		prints("CLEAR!", YELLOW);
	}

	pre_what_key = 0;
	what_key = 0;
		
	while(1){
		
		pre_what_key = what_key;
		what_key = judge_key();
		
		if((pre_what_key & KEY_LEFT) != KEY_LEFT && (what_key & KEY_LEFT) == KEY_LEFT){
			if(state_select == 0){
				state_select = 2;
			} else {
				state_select --;
			}

		} else if((pre_what_key & KEY_RIGHT) != KEY_RIGHT && (what_key & KEY_RIGHT) == KEY_RIGHT){
			if(state_select == 2){
				state_select = 0;
			} else {
				state_select ++;
			}
		}

		for(i = 0; i < 3; i++){
			color_select[i] = WHITE;
			color_select[state_select] = RED;
		}

		locate(4, 16);
		prints("TITLE", color_select[0]);

		locate(12, 16);
		prints("OPTION", color_select[1]);

		locate(21, 16);
		prints("RETRY", color_select[2]);

		if((pre_what_key & KEY_START) != KEY_START && (what_key & KEY_START) == KEY_START){
			if(state_select == 0){
				phase = START;
				break;
			} else if(state_select == 1){
				phase = OPTION;
				break;
			} else if(state_select == 2){
				phase = GAME;
				break;
			} else {
				break;
			}
		}
	}

	best = 0;
	cool = 0;
	good = 0;
	miss = 0;

	*((unsigned short *)0x0400010E) = 0x0004;     // タイムスイッチOFF
	*((unsigned short *)0x0400010C) = 0;
	
}