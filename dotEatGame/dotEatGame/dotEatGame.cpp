//----------------------------------------------------------------------
//			dotEatGame ver 0.001
//			Copyright (C) 2014 by N.Tsuda
//			Description: ドットを食べるゲーム
//			License: CDDL 1.0 (http://opensource.org/licenses/CDDL-1.0)
//----------------------------------------------------------------------
//		謝辞：
//			本プログラムで使用している mp3 ファイルは
//			「フリー効果音素材 くらげ工匠」様のものを
//			ありがた～く使用させていただいています。感謝でござるぞ。
//			http://www.kurage-kosho.info/index.html
//----------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include "consoleUtil.h"
#include "DblBuffer.h"

#pragma comment (lib, "winmm.lib")

typedef unsigned char byte;

#define		DBL_BUFFER		1

typedef std::pair<int, int> Vec2;		//	(x, y) ベクター
bool operator==(const Vec2 &a, const Vec2 &b)
{
	return a.first == b.first && a.second == b.second;
}
Vec2 operator+(const Vec2 &a, const Vec2 &b)
{
	return Vec2(a.first + b.first, a.second + b.second);
}
Vec2 operator-(const Vec2 &a, const Vec2 &b)
{
	return Vec2(a.first - b.first, a.second - b.second);
}
//	x' 	(cosθ - sinθ)  x
//	y'	(sinθ + cosθ) y
Vec2 rot_right90(const Vec2 v)
{
	return Vec2(-v.second, v.first);
}
Vec2 rot_left90(const Vec2 v)
{
	return Vec2(v.second, -v.first);
}

//----------------------------------------------------------------------
struct Car
{
public:
	Car(int x = 0, int y = 0, int dx = 0, int dy = 0, int lane = 0)
		: m_pos(x, y), m_v(dx, dy)
		//: m_x(x), m_y(y), m_dx(dx), m_dy(dy)
		, m_lane(lane)
		, m_laneChanged(false)
		{}
	//Car(const Car &x)
	//	: m_x(x.m_x), m_y(x.m_y), m_dx(x.m_dx), m_dy(x.m_dy), m_lane(x.m_lane)
	//	, m_laneChanged(false)
	//	{}
public:
#if	1
	Vec2	m_pos;		//	位置
	Vec2	m_v;			//	速度
#else
	int	m_x;		//	位置
	int	m_y;
	int	m_dx;	//	速度
	int	m_dy;
#endif
	int	m_lane;		//	一番外側が0
	bool	m_laneChanged;		//	レーンチェンジした直後かどうか
};
//----------------------------------------------------------------------

#define		CONS_WD		80
#define		CONS_HT		25
#define		MAP_WD		(CONS_WD/2)
#define		MAP_HT			(CONS_HT-1)
#define		SCORE_X		11*2
#define		SCORE_Y		11
#define		LOOP_INTERVAL		10

int	g_score;	//	スコア
Car	g_car;		//	自機
int	g_nEnemy;	//	敵機数
std::vector<Car>	g_enemy;		//	敵機
char	g_map[MAP_HT][MAP_WD];	//	マップ
DblBuffer	g_db;		//	ちらつき防止表示用ダブルバッファ

const char *mapData[] = {
	"/--------------------------------------#",
	"| . . . . . . . . .  . . . . . . . . . |",
	"|./----------------  ----------------#.|",
	"| | . . . . . . . .  . . . . . . . . | |",
	"|.|./--------------  --------------#.|.|",
	"| | | . . . . . . .  . . . . . . . | | |",
	"|.|.|./------------  ------------#.|.|.|",
	"| | | | . . . . . .  . . . . . . | | | |",
	"|.|.|.|./----------  ----------#.|.|.|.|",
	"| | | | | . . . . .  . . . . . | | | | |",
	"|.|.|.|.|./------------------#.|.|.|.|.|",
	"|         |                  |         |",
	"|         |                  |         |",
	"|.|.|.|.|.L------------------J.|.|.|.|.|",
	"| | | | | . . . . .  . . . . . | | | | |",
	"|.|.|.|.L----------  ----------J.|.|.|.|",
	"| | | | . . . . . .  . . . . . . | | | |",
	"|.|.|.L------------  ------------J.|.|.|",
	"| | | . . . . . . .  . . . . . . . | | |",
	"|.|.L--------------  --------------J.|.|",
	"| | . . . . . . . .  . . . . . . . . | |",
	"|.L----------------  ----------------J.|",
	"| . . . . . . . . .  . . . . . . . . . |",
	"L--------------------------------------J",
	0,
};
void init_map()
{
	for (int y = 0; mapData[y] != 0; ++y) {
		for (int x = 0; x < MAP_WD; ++x) {
			g_map[y][x] = mapData[y][x];
		}
	}
}
void add_enemy()
{
	g_enemy.clear();
	g_enemy.push_back(Car(MAP_WD / 2 + 1, 1, 1, 0));
	if( g_nEnemy == 1 ) return;
	g_enemy.push_back(Car(MAP_WD / 2 + 1, MAP_HT - 2, -1, 0));
	if( g_nEnemy == 2 ) return;
	g_enemy.push_back(Car(9, MAP_HT / 2 + 1, 0, -1, /*lane:*/4));
}
void init()
{
	g_car = Car(MAP_WD / 2 - 1, 1, -1, 0);
	add_enemy();
	init_map();
}
int draw_map()
{
	int nDot = 0;
	for (int y = 0; y < MAP_HT; ++y) {
		g_db.setCursorPos(0, y);
		const char *ptr = g_map[y];
		for (int x = 0; x < MAP_WD; ++x) {
			switch (*ptr++) {
				case '-':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("━");
					break;
				case '|':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("┃");
					break;
				case '/':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("┏");
					break;
				case '#':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("┓");
					break;
				case 'L':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("┗");
					break;
				case 'J':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("┛");
					break;
				case '.':
					g_db.setColor(DblBuffer::YELLOW, DblBuffer::BLACK);
					g_db.write("・");
					++nDot;
					break;
				default:
					g_db.setColor(DblBuffer::BLACK, DblBuffer::BLACK);
					g_db.write("  ");
					break;
			}
		}
	}
	return nDot;
}
void draw_car()
{
	g_db.setCursorPos(g_car.m_pos.first*2, g_car.m_pos.second);
	g_db.setColor(DblBuffer::GREEN, DblBuffer::BLACK);
	g_db.write("＠");
}
void draw_enemy()
{
	g_db.setColor(DblBuffer::RED, DblBuffer::BLACK);
	for (int i = 0; i < (int)g_enemy.size(); ++i) {
		g_db.setCursorPos(g_enemy[i].m_pos.first*2, g_enemy[i].m_pos.second);
		g_db.write("◆");
	}
}
std::string to_string(int v)
{
	if( !v ) return std::string("0");
	std::string str;
	while( v ) {
		str = std::string(1, '0' + v % 10) + str;
		v /= 10;
	}
	return str;
}
void draw_score()
{
	g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
	g_db.setCursorPos(SCORE_X, SCORE_Y);
	g_db.write("SCORE:");
	std::string str = to_string(g_score);
	while( str.size() < 6 )
		str = "0" + str;
	g_db.setCursorPos(SCORE_X, SCORE_Y + 1);
	g_db.write(str);
}
//	指定位置に壁が無いかどうかをチェック
bool can_move_to(int x, int y)
{
	return g_map[y][x] == ' ' || g_map[y][x] == '.';
}
bool can_move_to(const Vec2 &pos)
{
	const int x = pos.first;
	const int y = pos.second;
	return g_map[y][x] == ' ' || g_map[y][x] == '.';
}
void move_car(Car &car)
{
	Vec2 p = car.m_pos + car.m_v;		//	次の位置
	if( !can_move_to(p) ) {		//	壁にぶつかった場合
		Vec2 v = rot_right90(car.m_v);	//	速度ベクターを90度回転
		p = car.m_pos + v;
		if( can_move_to(p) )
			car.m_v = v;
		else {
			//	行き止まりは無いと仮定
			car.m_v = rot_left90(car.m_v);	//	速度ベクターを90度回転
			p = car.m_pos + car.m_v;
		}
	}
	car.m_pos = p;
}
void change_lane(Car &car, int &key)
{
	Vec2 p = car.m_pos;
	if( can_move_to(p) && can_move_to(p + car.m_v) )
	{		//	行き止まりでない場合
		if( car.m_v.second == 0 ) {		//	水平方向に移動している場合
			if( key == VK_UP && can_move_to(p - Vec2(0, 1)) ) {
				if( (p.second -= 2) < MAP_HT/2 )
					--car.m_lane;		//	外側のレーンに移動
				else
					++car.m_lane;
				key = 0;
			} else if( key == VK_DOWN && can_move_to(p + Vec2(0, 1)) ) {
				if( (p.second += 2) > MAP_HT/2 )
					--car.m_lane;		//	外側のレーンに移動
				else
					++car.m_lane;
				key = 0;
			}
		} else {			//	垂直方向に移動している場合
			if( key == VK_LEFT && can_move_to(p - Vec2(1, 0)) ) {
				if( (p.first -= 2) < MAP_WD/2 )
					--car.m_lane;		//	外側のレーンに移動
				else
					++car.m_lane;
				key = 0;
			} else if( key == VK_RIGHT && can_move_to(p + Vec2(1, 0)) ) {
				if( (p.first += 2) > MAP_WD/2 )
					--car.m_lane;		//	外側のレーンに移動
				else
					++car.m_lane;
				key = 0;
			}
		}
	}
	car.m_pos = p;
}
//	敵機を自機の方にレーン変更
//		一番外側が レーン０，一番内側が レーン４
void change_lane(Car &car)
{
	if( car.m_laneChanged ||		//	２回連続レーンチェンジ不可
		car.m_lane == g_car.m_lane ||
		!can_move_to(car.m_pos + car.m_v) )		//	行き止まりの場合
	{
		car.m_laneChanged = false;
		return;
	}
	if( car.m_lane > g_car.m_lane ) {		//	外側のレーンに移動
		if( car.m_v.second == 0 ) {		//	水平方向に移動している場合
			if( car.m_pos.second < MAP_HT/2 ) {
				if( can_move_to(car.m_pos - Vec2(0, 1)) ) {
					car.m_pos.second -= 2;
					--car.m_lane;
					car.m_laneChanged = true;
				}
			} else {
				if( can_move_to(car.m_pos + Vec2(0, 1)) ) {
					car.m_pos.second += 2;
					--car.m_lane;
					car.m_laneChanged = true;
				}
			}
		} else {
		}
	} else {		//	内側のレーンに移動
		if( car.m_v.second == 0 ) {		//	水平方向に移動している場合
			if( car.m_pos.second < MAP_HT/2 ) {
				if( can_move_to(car.m_pos + Vec2(0, 1)) ) {
					car.m_pos.second += 2;
					++car.m_lane;
					car.m_laneChanged = true;
				}
			} else {
				if( can_move_to(car.m_pos - Vec2(0, 1)) ) {
					car.m_pos.second -= 2;
					++car.m_lane;
					car.m_laneChanged = true;
				}
			}
		} else {
		}
	}
	assert(car.m_lane >= 0 && car.m_lane < 5);
}
//	加減速処理
void accel_decel(int &key, int &iv)
{
    if( g_car.m_v.first > 0 ) {
    	if( key == VK_RIGHT ) {
    		iv = 5;
    		key = 0;
    	} else if( key == VK_LEFT ) {
    		iv = 20;
    		key = 0;
    	}
    } else if( g_car.m_v.first < 0 ) {
    	if( key == VK_RIGHT ) {
    		iv = 20;
    		key = 0;
    	} else if( key == VK_LEFT ) {
    		iv = 5;
    		key = 0;
    	}
    } else if( g_car.m_v.second > 0 ) {
    	if( key == VK_DOWN ) {
    		iv = 5;
    		key = 0;
    	} else if( key == VK_UP ) {
    		iv = 20;
    		key = 0;
    	}
    } else if( g_car.m_v.second < 0 ) {
    	if( key == VK_DOWN ) {
    		iv = 20;
    		key = 0;
    	} else if( key == VK_UP ) {
    		iv = 5;
    		key = 0;
    	}
    }
}
void eat_dot()
{
	if( g_map[g_car.m_pos.second][g_car.m_pos.first] == '.' ) {
		g_map[g_car.m_pos.second][g_car.m_pos.first] = ' ';
		g_score += 10;
		mciSendString(TEXT("play button57.mp3"), NULL, 0, NULL);
		//std::cout << (char)0x07;			//	ビープ音
	}
}
bool check_crash()
{
	for (int i = 0; i < (int)g_enemy.size(); ++i) {
		const Car &c = g_enemy[i];
		if( c.m_pos == g_car.m_pos )
			return true;
		if( c.m_pos == g_car.m_pos - g_car.m_v )
			return true;		//	すり抜けた場合
	}
	return false;
}
bool game()
{
	g_score = 0;
	g_nEnemy = 1;
	init();
	int key = 0;   // 押下されたキー
    int keyDown = 0;   // 押下状態のキー
    bool update = true;
    int iv = 10;
    for (int cnt=1;;++cnt) {
    	if( update ) {
			int nDot = draw_map();
			draw_enemy();
			draw_car();
	        draw_score();
			g_db.swap();
			if( !nDot ) {		//	ドット全消去
				mciSendString(TEXT("play one23.mp3"), NULL, 0, NULL);
				Sleep(1000);
				g_score += g_nEnemy * 1000;
				++g_nEnemy;
				init();
				continue;
				//return true;
			}
			if( check_crash() ) {
				mciSendString(TEXT("play s-burst01.mp3"), NULL, 0, NULL);
				return false;
			}
    	}
		Sleep(10);
		update = false;
		if( !keyDown ) {     // キー押下を受け付けていない場合
            if( isKeyPressed(VK_LEFT) ) {
                key = keyDown = VK_LEFT;
            } else if( isKeyPressed(VK_RIGHT) ) {
                key = keyDown = VK_RIGHT;
            } else if( isKeyPressed(VK_UP) ) {
                key = keyDown = VK_UP;
            } else if( isKeyPressed(VK_DOWN) ) {
                key = keyDown = VK_DOWN;
            }
        } else {
            if( !isKeyPressed(keyDown) )// 押されたキーが離された
                keyDown = 0;
        }
        if( cnt % 10 == 0 ) {
        	for (int i = 0; i < (int)g_enemy.size(); ++i) {
		        move_car(g_enemy[i]);
		        change_lane(g_enemy[i]);
        	}
	        update = true;
        }
        accel_decel(key, iv);
        if( cnt % iv == 0 ) {
	        move_car(g_car);
	        change_lane(g_car, key);
	        //move_car(g_car, key);
	        eat_dot();
	        update = true;
	        iv = 10;
        }
	}
}
#if	0
int main()
{
	g_nEnemy = 1;	//	敵機数：1
	init();
	draw_map();
	draw_car();
	draw_enemy();
	draw_score();
	g_db.swap();		//	表示バッファ切り替え
	getchar();
	return 0;
}
#else
int main()
{
	//mciSendString(TEXT("open button57.mp3"), NULL, 0, NULL);
	//mciSendString(TEXT("open s-burst01.mp3"), NULL, 0, NULL);
	//mciSendString(TEXT("open one23.mp3"), NULL, 0, NULL);
	for (;;) {
		bool rc = game();
		g_db.setCursorPos(0, CONS_HT - 1);
		g_db.setColor(COL_GRAY, COL_BLACK);
		//if( rc ) {
		//	g_db.write("!!! GOOD JOB !!!");
		//} else {
			g_db.write("GAME OVER.");
		//}
		g_db.write(" Try Again ? [y/n] ");
		draw_map();
		draw_enemy();
		draw_car();
		draw_score();
		g_db.swap();
		for (;;) {
			if( isKeyPressed('N') )
				return 0;
			if( isKeyPressed('Y') )
				break;
			Sleep(LOOP_INTERVAL);		//	10ミリ秒ウェイト
		}
		g_db.setCursorPos(0, CONS_HT - 1);
		for (int i = 0; i < CONS_WD - 1; ++i) {
			g_db.write(" ");
		}
		g_db.swap();
		g_db.setCursorPos(0, CONS_HT - 1);
		for (int i = 0; i < CONS_WD - 1; ++i) {
			g_db.write(" ");
		}
	}
	return 0;
}
#endif
/*
◎ ダブルバッファ
◎ キーでコース変更
◎ キーで加速・減速
◎ ドットを消す処理
◎ ドット消去時：スコア＋、ビープ音？
◎ スコア表示
◎ ドットを全て消したらクリア
◎ ドットを全て消したら敵機をひとつ増やす
◎ 敵機実装
◎ 敵機は自機のレーン方向に１つだけ移動
◎ 問題：たまに180度方向転換してしまうことがある
◎ 問題：敵機をすり抜けてしまうことがある
◎ 問題：再ゲーム時の初期化が不十分
◎ 問題：クラッシュした時に、スコア、自機が表示されない
*/
