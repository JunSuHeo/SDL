#define _CRT_SECURE_NO_WARNINGS
#include "Message.h"
#include "Read_CSV.h"
#include "Util.h"

unsigned __stdcall keyboard()
{
	bool quit = false;
	bool print_msg = false;
	bool get_msg = false;
	char* message_path = NULL;

	SDL_Event event1;
	//SDL ���� �ʱ�ȭ   
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	SDL_Window *window1 = SDL_CreateWindow("message window", 860, 350, 400, 225, SDL_WINDOW_BORDERLESS);
	SDL_Renderer * renderer = SDL_CreateRenderer(window1, -1, 0);

	SDL_Color color = { 255, 255, 255 };

	//message ������ ��¿� �ʿ��� ������
	SDL_Surface * icon_surface = NULL;
	SDL_Texture * icon_texture = NULL;
	SDL_Rect image_dstrect = { 55, 148, 120, 70 };

	//message ���� ��¿� �ʿ��� ������
	SDL_Surface *msg_surface = NULL;
	SDL_Texture *msg_texture = NULL;
	SDL_Rect msg_dstrect = { 45, 95, 270, 135 };

	//����� mixer �ʱ�ȭ
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 4096))
	{
		fprintf(stderr, "SDL_Mixer Error: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_Chunk* warning = Mix_LoadWAV("Sound/warning.wav");
	Mix_Chunk* getmsg = Mix_LoadWAV("Sound/getmsg.wav");

	//Make transparency window
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderClear(renderer);
	MakeWindowTransparent(window1, RGB(255, 0, 255));

	while (!quit)
	{
		if (SDL_PollEvent(&event1) != 0) {
			if (event1.window.event == SDL_WINDOWEVENT_CLOSE) {
				quit = true;
				break;
			}
		}

		//������â ����
		if (data.frame == 13 || data.frame == 14)
			SDL_RaiseWindow(window1);

		//����������� �����
		SDL_DestroyRenderer(renderer);
		renderer = SDL_CreateRenderer(window1, -1, 0);
		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);

		//ALT + N ������ �������
		if (keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_N]) {
			Mix_PlayChannel(-1, warning, 0);
		}

		//�޼��� ����
		if (strcmp(data.check, "0") != 0) {

			if (get_msg == false)
			{
				Mix_PlayChannel(-1, getmsg, 0);
				get_msg = true;
			}

			//�޼��� ������ �ȶ�������� Ű���� �Է� �޴´�.
			if (print_msg == false) {

				if (icon_surface != NULL) {
					SDL_FreeSurface(icon_surface);
					SDL_DestroyTexture(icon_texture);
					icon_surface = NULL;
					icon_texture = NULL;
				}

				//message ��� ����
				if (message_path != NULL)
				{
					free(message_path);
					message_path = NULL;
				}
				
				message_path = (char*)malloc(sizeof(char) * 26);
				strcpy(message_path, "Image/message/msg_in");
				strcat(message_path, data.check);
				strcat(message_path, ".png");
				icon_surface = IMG_Load(message_path); // �޼��� ������
				icon_texture = SDL_CreateTextureFromSurface(renderer, icon_surface);
				SDL_RenderCopy(renderer, icon_texture, NULL, &image_dstrect);

				//ALT + G or ALT + V�� ������ �޼����� ���´�.
				if ((keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_G]) || (keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_V])) {
					print_msg = true;
					//�̹��� ���
					SDL_DestroyRenderer(renderer);
					renderer = SDL_CreateRenderer(window1, -1, 0);
					SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
					SDL_RenderClear(renderer);

					if (msg_surface != NULL)
					{
						SDL_FreeSurface(msg_surface);
						SDL_DestroyTexture(msg_texture);
						msg_surface = NULL;
					}

					if (message_path != NULL)
					{
						free(message_path);
						message_path = NULL;
					}

					message_path = (char*)malloc(sizeof(char) * 28);
					strcpy(message_path, "Image/message/msg_read");
					strcat(message_path, data.check);
					strcat(message_path, ".png");
					msg_surface = IMG_Load(message_path); // �޼��� ����
					msg_texture = SDL_CreateTextureFromSurface(renderer, msg_surface);
					SDL_RenderCopy(renderer, msg_texture, NULL, &msg_dstrect);
				}
			}

			//�޼��� ������ ���������
			else {

				if (msg_surface != NULL)
				{
					SDL_FreeSurface(msg_surface);
					SDL_DestroyTexture(msg_texture);
					msg_surface = NULL;
					msg_texture = NULL;
				}

				msg_surface = IMG_Load(message_path); // �޼��� ����
				msg_texture = SDL_CreateTextureFromSurface(renderer, msg_surface);
				SDL_RenderCopy(renderer, msg_texture, NULL, &msg_dstrect);

				//ALT + D or ALT + E�� ������ �޼����� �����ȴ�.
				if ((keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_D]) || (keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_E])) {
					print_msg = false;
					//�̹��� ����
					SDL_DestroyRenderer(renderer);
					renderer = SDL_CreateRenderer(window1, -1, 0);
					SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
					SDL_RenderClear(renderer);
					SDL_DestroyTexture(msg_texture);
					SDL_FreeSurface(msg_surface);
					msg_texture = NULL;
					msg_surface = NULL;
					get_msg = false;
				}
				
			}
		}

		//�޼��� ������ ���� �ʾ�����
		else {
			get_msg = false;
			//�޼��� ��»����̸�
			if (print_msg == true)
			{

				if (msg_surface != NULL)
				{
					SDL_FreeSurface(msg_surface);
					SDL_DestroyTexture(msg_texture);
					msg_surface = NULL;
					msg_texture = NULL;
				}

				msg_surface = IMG_Load(message_path);
				msg_texture = SDL_CreateTextureFromSurface(renderer, msg_surface);
				SDL_RenderCopy(renderer, msg_texture, NULL, &msg_dstrect);

				//ALT + D or ALT + E�� ������ �޼����� �����ȴ�.
				if ((keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_D]) || (keystate[SDL_SCANCODE_LALT] && keystate[SDL_SCANCODE_E])) {
					print_msg = false;
					//�̹��� ����
					SDL_DestroyRenderer(renderer);
					renderer = SDL_CreateRenderer(window1, -1, 0);
					SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
					SDL_RenderClear(renderer);
					SDL_DestroyTexture(msg_texture);
					SDL_FreeSurface(msg_surface);
					msg_texture = NULL;
					msg_surface = NULL;
					
				}
				
			}

		}
		
		SDL_RenderPresent(renderer);
		SDL_Delay(200);
	}

	//���� sdl ����
	SDL_DestroyTexture(icon_texture);
	SDL_FreeSurface(icon_surface);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window1);

	return 0;
}