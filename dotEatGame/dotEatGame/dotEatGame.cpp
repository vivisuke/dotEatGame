//----------------------------------------------------------------------
//			dotEatGame ver 0.001
//			Copyright (C) 2014 by N.Tsuda
//			Description: �h�b�g��H�ׂ�Q�[��
//			License: CDDL 1.0 (http://opensource.org/licenses/CDDL-1.0)
//----------------------------------------------------------------------
//		�ӎ��F
//			�{�v���O�����Ŏg�p���Ă��� mp3 �t�@�C����
//			�u�t���[���ʉ��f�� ���炰�H���v�l�̂��̂�
//			���肪���`���g�p�����Ă��������Ă��܂��B���ӂł����邼�B
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

typedef std::pair<int, int> Vec2;		//	(x, y) �x�N�^�[

//----------------------------------------------------------------------
struct Car
{
public:
	Car(int x = 0, int y = 0, int dx = 0, int dy = 0, int lane = 0)
		: m_x(x), m_y(y), m_dx(dx), m_dy(dy), m_lane(lane)
		, m_laneChanged(false)
		{}
	//Car(const Car &x)
	//	: m_x(x.m_x), m_y(x.m_y), m_dx(x.m_dx), m_dy(x.m_dy), m_lane(x.m_lane)
	//	, m_laneChanged(false)
	//	{}
public:
	int	m_x;		//	�ʒu
	int	m_y;
	int	m_dx;	//	���x
	int	m_dy;
	int	m_lane;		//	��ԊO����0
	bool	m_laneChanged;		//	���[���`�F���W�������ォ�ǂ���
};
//----------------------------------------------------------------------

#define		CONS_WD		80
#define		CONS_HT		25
#define		MAP_WD		(CONS_WD/2)
#define		MAP_HT			(CONS_HT-1)
#define		SCORE_X		11*2
#define		SCORE_Y		11
#define		LOOP_INTERVAL		10

int	g_score;	//	�X�R�A
Car	g_car;		//	���@
int	g_nEnemy;	//	�G�@��
std::vector<Car>	g_enemy;		//	�G�@
char	g_map[MAP_HT][MAP_WD];	//	�}�b�v
DblBuffer	g_db;		//	������h�~�\���p�_�u���o�b�t�@

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
	g_enemy.push_back(Car(9, MAP_HT / 2 + 1, -1, 0, /*lane:*/4));
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
					g_db.write("��");
					break;
				case '|':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("��");
					break;
				case '/':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("��");
					break;
				case '#':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("��");
					break;
				case 'L':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("��");
					break;
				case 'J':
					g_db.setColor(DblBuffer::GRAY, DblBuffer::BLACK);
					g_db.write("��");
					break;
				case '.':
					g_db.setColor(DblBuffer::YELLOW, DblBuffer::BLACK);
					g_db.write("�E");
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
	g_db.setCursorPos(g_car.m_x*2, g_car.m_y);
	g_db.setColor(DblBuffer::GREEN, DblBuffer::BLACK);
	g_db.write("��");
}
void draw_enemy()
{
	g_db.setColor(DblBuffer::RED, DblBuffer::BLACK);
	for (int i = 0; i < (int)g_enemy.size(); ++i) {
		g_db.setCursorPos(g_enemy[i].m_x*2, g_enemy[i].m_y);
		g_db.write("��");
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
//	x' 	(cos�� - sin��)
//	y'	(sin�� + cos��)
Vec2 rot_right90(const Vec2 v)
{
	return Vec2(-v.second, v.first);
}
Vec2 rot_left90(const Vec2 v)
{
	return Vec2(v.second, -v.first);
}
#if	0
void rot_right90(int &x, int &y)
{
	//	c90 = 0, s90 = 1
	int x2 = -y;
	int y2 = x;
	x = x2;
	y = y2;
}
void rot_left90(int &x, int &y)
{
	//	c(-90) = 0, s(-90) = -1
	int x2 = y;
	int y2 = -x;
	x = x2;
	y = y2;
}
#endif
//	�w��ʒu�ɕǂ��������ǂ������`�F�b�N
bool can_move_to(int x, int y)
{
	return g_map[y][x] == ' ' || g_map[y][x] == '.';
}
void move_car(Car &car)
{
	int x = car.m_x + car.m_dx;
	int y = car.m_y + car.m_dy;
	if( !can_move_to(x, y) ) {
		if( car.m_dy == 0 ) {		//	���������Ɉړ����Ă���ꍇ
			if( can_move_to(car.m_x, car.m_y + 1) )
				car.m_dy = 1;
			else
				car.m_dy = -1;
			car.m_dx = 0;
		} else {			//	���������Ɉړ����Ă���ꍇ
			if( can_move_to(car.m_x + 1, car.m_y) )
				car.m_dx = 1;
			else
				car.m_dx = -1;
			car.m_dy = 0;
		}
		x = car.m_x + car.m_dx;
		y = car.m_y + car.m_dy;
	}
	car.m_x = x;
	car.m_y = y;
}
void change_lane(Car &car, int &key)
{
	int x = car.m_x;
	int y = car.m_y;
	if( can_move_to(x, y) && can_move_to(x+car.m_dx, y+car.m_dy) )
	{		//	�s���~�܂�łȂ��ꍇ
		if( car.m_dy == 0 ) {		//	���������Ɉړ����Ă���ꍇ
			if( key == VK_UP && can_move_to(x, y-1)) {
				y-=2;
				if( y < MAP_HT/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			} else if( key == VK_DOWN && can_move_to(x, y+1)) {
				y+=2;
				if( y > MAP_HT/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			}
		} else {			//	���������Ɉړ����Ă���ꍇ
			if( key == VK_LEFT && can_move_to(x-1, y)) {
				x-=2;
				if( x < MAP_WD/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			} else if( key == VK_RIGHT && can_move_to(x+1, y)) {
				x+=2;
				if( x > MAP_WD/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			}
		}
	}
	car.m_x = x;
	car.m_y = y;
}
void move_car(Car &car, int &key)
{
	int x = car.m_x + car.m_dx;
	int y = car.m_y + car.m_dy;
	if( !can_move_to(x, y) ) {
		if( car.m_dy == 0 ) {		//	���������Ɉړ����Ă���ꍇ
			if( can_move_to(car.m_x, car.m_y + 1) )
				car.m_dy = 1;
			else
				car.m_dy = -1;
			car.m_dx = 0;
		} else {			//	���������Ɉړ����Ă���ꍇ
			if( can_move_to(car.m_x + 1, car.m_y) )
				car.m_dx = 1;
			else
				car.m_dx = -1;
			car.m_dy = 0;
		}
		x = car.m_x + car.m_dx;
		y = car.m_y + car.m_dy;
	} else if( can_move_to(x, y) && can_move_to(x+car.m_dx, y+car.m_dy) )
	{		//	�s���~�܂�łȂ��ꍇ
		if( car.m_dy == 0 ) {		//	���������Ɉړ����Ă���ꍇ
			if( key == VK_UP && can_move_to(x, y-1)) {
				y-=2;
				if( y < MAP_HT/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			} else if( key == VK_DOWN && can_move_to(x, y+1)) {
				y+=2;
				if( y > MAP_HT/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			}
		} else {			//	���������Ɉړ����Ă���ꍇ
			if( key == VK_LEFT && can_move_to(x-1, y)) {
				x-=2;
				if( x < MAP_WD/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			} else if( key == VK_RIGHT && can_move_to(x+1, y)) {
				x+=2;
				if( x > MAP_WD/2 )
					--car.m_lane;		//	�O���̃��[���Ɉړ�
				else
					++car.m_lane;
				key = 0;
			}
		}
	}
	car.m_x = x;
	car.m_y = y;
}
//	�G�@�����@�̕��Ƀ��[���ύX
//		��ԊO���� ���[���O�C��ԓ����� ���[���S
void change_lane(Car &car)
{
	if( car.m_laneChanged ||		//	�Q��A�����[���`�F���W�s��
		car.m_lane == g_car.m_lane ||
		!can_move_to(car.m_x + car.m_dx, car.m_y + car.m_dy) )		//	�s���~�܂�̏ꍇ
	{
		car.m_laneChanged = false;
		return;
	}
	if( car.m_lane > g_car.m_lane ) {		//	�O���̃��[���Ɉړ�
		if( car.m_dy == 0 ) {		//	���������Ɉړ����Ă���ꍇ
			if( car.m_y < MAP_HT/2 ) {
				if( can_move_to(car.m_x, car.m_y-1) ) {
					car.m_y -= 2;
					--car.m_lane;
					car.m_laneChanged = true;
				}
			} else {
				if( can_move_to(car.m_x, car.m_y+1) ) {
					car.m_y += 2;
					--car.m_lane;
					car.m_laneChanged = true;
				}
			}
		} else {
		}
	} else {		//	�����̃��[���Ɉړ�
		if( car.m_dy == 0 ) {		//	���������Ɉړ����Ă���ꍇ
			if( car.m_y < MAP_HT/2 ) {
				if( can_move_to(car.m_x, car.m_y+1) ) {
					car.m_y += 2;
					++car.m_lane;
					car.m_laneChanged = true;
				}
			} else {
				if( can_move_to(car.m_x, car.m_y-1) ) {
					car.m_y -= 2;
					++car.m_lane;
					car.m_laneChanged = true;
				}
			}
		} else {
		}
	}
	assert(car.m_lane >= 0 && car.m_lane < 5);
}
//	����������
void accel_decel(int &key, int &iv)
{
    if( g_car.m_dx > 0 ) {
    	if( key == VK_RIGHT ) {
    		iv = 5;
    		key = 0;
    	} else if( key == VK_LEFT ) {
    		iv = 20;
    		key = 0;
    	}
    } else if( g_car.m_dx < 0 ) {
    	if( key == VK_RIGHT ) {
    		iv = 20;
    		key = 0;
    	} else if( key == VK_LEFT ) {
    		iv = 5;
    		key = 0;
    	}
    } else if( g_car.m_dy > 0 ) {
    	if( key == VK_DOWN ) {
    		iv = 5;
    		key = 0;
    	} else if( key == VK_UP ) {
    		iv = 20;
    		key = 0;
    	}
    } else if( g_car.m_dy < 0 ) {
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
	if( g_map[g_car.m_y][g_car.m_x] == '.' ) {
		g_map[g_car.m_y][g_car.m_x] = ' ';
		g_score += 10;
		mciSendString(TEXT("play button57.mp3"), NULL, 0, NULL);
		//std::cout << (char)0x07;			//	�r�[�v��
	}
}
bool check_crash()
{
	for (int i = 0; i < (int)g_enemy.size(); ++i) {
		const Car &c = g_enemy[i];
		if( c.m_x == g_car.m_x && c.m_y == g_car.m_y )
			return true;
		if( c.m_x == g_car.m_x - g_car.m_dx && c.m_y == g_car.m_y - g_car.m_dy)
			return true;		//	���蔲�����ꍇ
	}
	return false;
}
bool game()
{
	g_score = 0;
	g_nEnemy = 1;
	init();
	int key = 0;   // �������ꂽ�L�[
    int keyDown = 0;   // ������Ԃ̃L�[
    bool update = true;
    int iv = 10;
    for (int cnt=1;;++cnt) {
    	if( update ) {
			int nDot = draw_map();
			draw_enemy();
			draw_car();
	        draw_score();
			g_db.swap();
			if( !nDot ) {		//	�h�b�g�S����
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
		if( !keyDown ) {     // �L�[�������󂯕t���Ă��Ȃ��ꍇ
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
            if( !isKeyPressed(keyDown) )// �����ꂽ�L�[�������ꂽ
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
	g_nEnemy = 1;	//	�G�@���F1
	init();
	draw_map();
	draw_car();
	draw_enemy();
	draw_score();
	g_db.swap();		//	�\���o�b�t�@�؂�ւ�
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
			Sleep(LOOP_INTERVAL);		//	10�~���b�E�F�C�g
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
�� �_�u���o�b�t�@
�� �L�[�ŃR�[�X�ύX
�� �L�[�ŉ����E����
�� �h�b�g����������
�� �h�b�g�������F�X�R�A�{�A�r�[�v���H
�� �X�R�A�\��
�� �h�b�g��S�ď�������N���A
�� �h�b�g��S�ď�������G�@���ЂƂ��₷
�� �G�@����
�� �G�@�͎��@�̃��[�������ɂP�����ړ�
�� ���F���܂�180�x�����]�����Ă��܂����Ƃ�����
�� ���F�G�@�����蔲���Ă��܂����Ƃ�����
�� ���F�ăQ�[�����̏��������s�\��
�� ���F�N���b�V���������ɁA�X�R�A�A���@���\������Ȃ�
*/