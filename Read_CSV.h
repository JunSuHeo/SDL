#ifndef __READ_CSV_H__
#define __READ_CSV_H__

#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>

#pragma comment (lib, "SDL2_image.lib")

#define MAXLENGTH 256

typedef struct column {
	int frame;
	int sec;
	int min;
	int secc;
	char speed[4];		//현재속도
	char speed_limit[3];	//제한속도
	int speed_sound; //???
	int fcws;
	char fcw_sound[5];	//beep or NULL
	int ldws;
	char ldw_sound[5]; //ding or NULL
	int ldwsx;
	int ldwsy;
	char tbt_now[3]; //r or rx or s or l or lx ..?
	char* dist_tbtnow;
	int properlanes;	//??????
	int lane_animation;	//??????
	char navi_sound[5]; //NULL or ???
	char tbt_next[3];	 //r or rx or s or l or lx ..?
	char dist_tbt_next[7]; // 남은거리
	int acc;	//????
	int brake;	//?????
	int bsal;	//1 or NULL
	int ellx;	//?????
	int elly;	//???
	int bsar;	//1 or NULL
	int erlx;	//???
	int erly;	//???
	int center_x;
	int center_y;
	int ADAS;	//????
	char S_camera[9];	//S_Camera or S_Merge or S_Curves
	char direction[3];
	char check[2];		//0 or 1 : 메세지가 왔는지 안왔는지
}column;

void Play();
unsigned __stdcall PrintImage();

extern column data;

#endif