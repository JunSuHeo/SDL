#define _CRT_SECURE_NO_WARNINGS

#include "Read_CSV.h"
#include "Util.h"
#include "Play_Video.h"
#include "Message.h"

column data;

void Play()   //main Thread
{
	FILE *fp = NULL;
	char arr[MAXLENGTH];

	HANDLE printimage = NULL;
	HANDLE keythread = NULL;
	HANDLE videothread = NULL;

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096))
	{
		fprintf(stderr, "SDL_Mixer Error: %s\n", Mix_GetError());
		exit(1);
	}
	
	Mix_Chunk* ding = Mix_LoadWAV("Sound/ding.wav");
	Mix_Chunk* beep = Mix_LoadWAV("Sound/beep.wav");

	//엑셀파일 열기
	fp = fopen("excel.csv", "r");
	if (fp == NULL)
		printf("Do not Open File!\n");

	else {
		fgets(arr, MAXLENGTH, fp);
		memset(arr, NULL, MAXLENGTH);

		videothread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))PlayVideo, NULL, 0, NULL);
		Sleep(10);
		keythread = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))keyboard, NULL, 0, NULL);
		Sleep(10);
		printimage = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))PrintImage, NULL, 0, NULL);
		Sleep(10);
		Sleep(1000);

		while (!feof(fp)) {
			fgets(arr, MAXLENGTH, fp);

			if (strcmp(data.fcw_sound, "beep") == 0)
			{
				if (Mix_PlayChannel(-1, beep, 0) < 0)
				{
					fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
				}
			}

			//ldw가 생겼을때 소리나오게 하기
			if (strcmp(data.ldw_sound, "ding") == 0)
			{
				if (Mix_PlayChannel(-1, beep, 0) < 0)
				{
					fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
				}
			}
			//0.2초 단위로 행 출력 (데이터 확인하기 위해)
			//printf("%s\n", arr);

			//문자열 분리
			//data에다가 엑셀에 해당하는 값을 넣는다.
			SeperateString(arr);

			SDL_Delay(200);

		}
	}

	//마무리작업 : 쓰레드 종료, 효과음 종료, SDL, TTF 종료
	SuspendThread(keythread);
	SuspendThread(videothread);
	SuspendThread(PrintImage);
	Mix_FreeChunk(ding);
	Mix_FreeChunk(beep);

	TTF_Quit();
	SDL_Quit();
}

unsigned __stdcall PrintImage()
{
	char *limit = NULL;
	char *tbt_now = NULL;
	int running = 1;
	int ilimit = 0;
	int spd = 0;
	int check_red = 0;
	int check_ldw = 0;
	unsigned int old_frame = 0;

	bool quit = false;
	SDL_Event event2;
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	//SDL_WINDOW_BORDERLESS
	SDL_Window *window = SDL_CreateWindow("SDL2 Displaying Image", 550, 350, 400, 225, SDL_WINDOW_BORDERLESS);
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);

	TTF_Font * font = TTF_OpenFont("arial.ttf", 50);
	SDL_Color color = { 255, 255, 255 };
	SDL_Color spd_color = { 255, 255, 255 };

	SDL_Surface * tbt_now_surface = NULL; // 방향표지판
	SDL_Texture * tbt_now_texture = NULL;
	SDL_Rect tbt_now_dstrect = { 150, 175, 40, 40 }; 

	SDL_Surface * speed_limit_surface = NULL;  // 제한속도
	SDL_Texture * speed_limit_texture = NULL;
	SDL_Rect speed_limit_dstrect = { 75, 175, 45, 45 };

	SDL_Surface * speed_surface = NULL; // 속도
	SDL_Texture * speed_texture = NULL;
	SDL_Rect speed_dstrect = { 0, 0, 40, 30 };

	SDL_Surface * km_per_h_surface = NULL;  // km/h
	SDL_Texture * km_per_h_texture = NULL;
	SDL_Rect km_per_h_dstrect = { 283, 195, 40, 20 };

	SDL_Surface * dist_tbt_surface = NULL;  // 남은거리
	SDL_Texture * dist_tbt_texture = NULL;
	SDL_Rect dist_tbt_dstrect = { 205, 185, 60, 35 };

	SDL_Surface * fcw_surface = NULL;	//FCW
	SDL_Texture * fcw_texture = NULL;
	SDL_Rect fcw_dstrect = { 150, 70, 100, 60 };

	SDL_Surface * ldw_surface = NULL;	//LDW
	SDL_Texture * ldw_texture = NULL;
	SDL_Rect ldw_dstrect = { 10, 80, 80, 50 };

	//오디오 초기화
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096))
	{
		fprintf(stderr, "SDL_Mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	
	Mix_Chunk* ding = Mix_LoadWAV("Sound/ding.wav");
	
	//네비소리
	Mix_Music* navi = Mix_LoadMUS("Sound/videoNavi.mp3");
	Mix_PlayMusic(navi, 0);

	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderClear(renderer);
	MakeWindowTransparent(window, RGB(255, 0, 255));

	while (running) {
		//종료버튼 눌렀을때 종료하기
		if (SDL_PollEvent(&event2) != 0) {
			if (event2.window.event == SDL_WINDOWEVENT_CLOSE) {
				running = 0;
				break;
			}
		}

		//그 전의 프레임과 같다면 넘어간다.
		if (data.frame == old_frame)
		{
			continue;
		}

		//창띄우기
		if (data.frame == 10 || data.frame == 11)
			SDL_RaiseWindow(window);

		ilimit = Array_2_Int(data.speed_limit);
		spd = Array_2_Int(data.speed);

		//제한속도 넘었을때 깜빡깜빡 & 소리나게하기
		if (spd > ilimit)
		{
			check_red++;
			if ((check_red % 4 == 0) || (check_red % 4 == 1))
			{
				spd_color.r = 255;
				spd_color.g = 0;
				spd_color.b = 0;

				if (check_red == 4) 
					check_red = 0;
				
				else
					check_red = 1;
			}
			else {
				spd_color.r = 255;
				spd_color.g = 255;
				spd_color.b = 255;
			}
		}

		//평소상태일때는 하얀색
		else
		{
			spd_color.r = 255;
			spd_color.g = 255;
			spd_color.b = 255;
		}

		//renderer 업데이트하기위해 전부 삭제
		SDL_DestroyRenderer(renderer);
		renderer = SDL_CreateRenderer(window, -1, 0);
		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);

		//제한속도 띄우기 위한 작업
		if (limit != NULL)
		{
			free(limit);
			limit = NULL;
		}
		limit = (char*)malloc(sizeof(char) * 20);
		
		strcpy(limit, "Image/Limit/");
		strcat(limit, data.speed_limit);
		strcat(limit, ".png");

		if (tbt_now != NULL)
		{
			free(tbt_now);
			tbt_now = NULL;
		}
		tbt_now = (char*)malloc(sizeof(char) * 17);
		strcpy(tbt_now, "Image/TBT/");
		strcat(tbt_now, data.tbt_now);
		strcat(tbt_now, ".png");
		
		SetPosition(&speed_dstrect, &dist_tbt_dstrect);

		font = TTF_OpenFont("arial.ttf", 50);

		//원래있던 내용들을 지우고 새롭게 생성
		if (tbt_now_surface != NULL)
		{
			SDL_FreeSurface(tbt_now_surface);
			SDL_DestroyTexture(tbt_now_texture);
			tbt_now_surface = NULL;
			tbt_now_texture = NULL;
		}
		tbt_now_surface = IMG_Load(limit);
		tbt_now_texture = SDL_CreateTextureFromSurface(renderer, tbt_now_surface);
		SDL_RenderCopy(renderer, tbt_now_texture, NULL, &speed_limit_dstrect);

		if (speed_limit_surface != NULL)
		{
			SDL_FreeSurface(speed_limit_surface);
			SDL_DestroyTexture(speed_limit_texture);
			speed_limit_surface = NULL;
			speed_limit_texture = NULL;
		}
		speed_limit_surface = IMG_Load(tbt_now);
		speed_limit_texture = SDL_CreateTextureFromSurface(renderer, speed_limit_surface);
		SDL_RenderCopy(renderer, speed_limit_texture, NULL, &tbt_now_dstrect);

		if (speed_surface != NULL)
		{
			SDL_FreeSurface(speed_surface);
			SDL_DestroyTexture(speed_texture);
			speed_surface = NULL;
			speed_texture = NULL;
		}
		speed_surface = TTF_RenderText_Solid(font, data.speed, spd_color);
		speed_texture = SDL_CreateTextureFromSurface(renderer, speed_surface);
		SDL_RenderCopy(renderer, speed_texture, NULL, &speed_dstrect);

		if (km_per_h_surface != NULL)
		{
			SDL_FreeSurface(km_per_h_surface);
			SDL_DestroyTexture(km_per_h_texture);
			km_per_h_surface = NULL;
			km_per_h_texture = NULL;
		}
		km_per_h_surface = TTF_RenderText_Solid(font, "km/h", color);
		km_per_h_texture = SDL_CreateTextureFromSurface(renderer, km_per_h_surface);
		SDL_RenderCopy(renderer, km_per_h_texture, NULL, &km_per_h_dstrect);

		if (dist_tbt_surface != NULL)
		{
			SDL_FreeSurface(dist_tbt_surface);
			SDL_DestroyTexture(dist_tbt_texture);
			dist_tbt_surface = NULL;
			dist_tbt_texture = NULL;
		}
		dist_tbt_surface = TTF_RenderText_Solid(font, data.dist_tbtnow, color);
		dist_tbt_texture = SDL_CreateTextureFromSurface(renderer, dist_tbt_surface);
		SDL_RenderCopy(renderer, dist_tbt_texture, NULL, &dist_tbt_dstrect);


		switch (data.fcws) {
			if (fcw_texture != NULL) {
				SDL_DestroyTexture(fcw_texture);
				SDL_FreeSurface(fcw_surface);
				fcw_texture = NULL;
				fcw_surface = NULL;
			}
		case 1:
			fcw_surface = IMG_Load("Image/FCW/FCW1.png");
			fcw_texture = SDL_CreateTextureFromSurface(renderer, fcw_surface);
			SDL_RenderCopy(renderer, fcw_texture, NULL, &fcw_dstrect);
			break;
		case 2:
			fcw_surface = IMG_Load("Image/FCW/FCW2.png");
			fcw_texture = SDL_CreateTextureFromSurface(renderer, fcw_surface);
			SDL_RenderCopy(renderer, fcw_texture, NULL, &fcw_dstrect);
			break;
		case 3:
			fcw_surface = IMG_Load("Image/FCW/FCW3.png");
			fcw_texture = SDL_CreateTextureFromSurface(renderer, fcw_surface);
			SDL_RenderCopy(renderer, fcw_texture, NULL, &fcw_dstrect);
			break;
		default:
			break;
		}

		switch (data.ldws) {
			if (ldw_texture != NULL) {
				SDL_DestroyTexture(ldw_texture);
				SDL_FreeSurface(ldw_surface);
				ldw_texture = NULL;
				ldw_surface = NULL;
			}
		case 1:
			ldw_dstrect.x = 50 + check_ldw * 5;
			ldw_surface = IMG_Load("Image/LDW/ldwl1.png");
			ldw_texture = SDL_CreateTextureFromSurface(renderer, ldw_surface);
			SDL_RenderCopy(renderer, ldw_texture, NULL, &ldw_dstrect);

			check_ldw++;
			
			if (check_ldw == 3)
				check_ldw = 0;
			
			break;
		case 2:
			ldw_dstrect.x = 300 - check_ldw * 5;
			ldw_surface = IMG_Load("Image/LDW/ldwr1.png");
			ldw_texture = SDL_CreateTextureFromSurface(renderer, ldw_surface);
			SDL_RenderCopy(renderer, ldw_texture, NULL, &ldw_dstrect);

			check_ldw++;
			if (check_ldw == 3)
				check_ldw = 0;
			
			break;

		default:
			break;
		}

		SDL_RenderPresent(renderer);

		TTF_CloseFont(font);
		old_frame = data.frame;
	}

	//끝났을때 해야될 작업들
	Mix_FreeChunk(ding);
	Mix_FreeMusic(navi);

	SDL_DestroyTexture(tbt_now_texture);
	SDL_FreeSurface(tbt_now_surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}