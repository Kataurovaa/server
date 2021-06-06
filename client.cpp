// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>
#define _WIN32_WINNT 0x0A00
#include <windows.h>
//индификаторы кнопок, чтобы их различать
#define ind_enter 1   //кнопка входа в систему
#define ind_signup 2	//кнопка для перехода на страницу регистрации
#define ind_username 3	//окно для ввода логина
#define ind_password 4	//окно для ввода пароля
#define ind_signup2 5	//кнопка входа в систему при регистрации
#define ind_message 6	//окно для ввода сообщения
#define ind_send 7	//кнопка для отправки сообщения

char username_temp[30];
char password[30];
char username[30];
char enter_data[15];
char signup_data[15];
char sending_message[54];
char send_data[15];

//FILE* fgenchat;
HWND enter_message;
HWND chat;

void profilePhoto(HWND hwnd, int x, int y, int w) {
    HDC dc = GetDC(hwnd);
    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, RGB(188, 210, 238));
    //SelectObject(dc, GetStockObject(DC_PEN));
    //SetDCPenColor(dc, RGB(188, 210, 238));
    Rectangle(dc, y, x, y + w, x + w);

    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, RGB(162, 181, 205));
    //SelectObject(dc, GetStockObject(DC_PEN));
    //SetDCPenColor(dc, RGB(162, 181, 205));
    Ellipse(dc, y + w / 4, x + w / 5, y + w * 3 / 4, x + w * 2 / 3);
    //Ellipse(dc, y + w / 4, x + w * 3 / 5, y + w * 3 / 4, x + w * 7 / 5);
    Ellipse(dc, y + w / 5, x + w * 2 / 3, y + w * 4 / 5, x + w * 7 / 5);

    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, RGB(255, 255, 255));
    SelectObject(dc, GetStockObject(DC_PEN));
    SetDCPenColor(dc, RGB(255, 255, 255));
    Rectangle(dc, y, x + w, y + w, x + w * 7 / 5);

    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, RGB(0, 0, 0));
    SelectObject(dc, GetStockObject(DC_PEN));
    SetDCPenColor(dc, RGB(0, 0, 0));
    Rectangle(dc, y, x + w - 1, y + w, x + w);
}
BOOL CALLBACK DestoryChildCallback(HWND hwnd, LPARAM lParam)
{
    if (hwnd != NULL) {
        DestroyWindow(hwnd);
    }
    return TRUE;
}
void clearWindow(HWND hwnd) {
    EnumChildWindows(hwnd, DestoryChildCallback, NULL);
    HDC dc = GetDC(hwnd);
    SelectObject(dc, GetStockObject(DC_BRUSH));
    SetDCBrushColor(dc, RGB(255, 255, 255));
    Rectangle(dc, 0, 0, 640, 480);
}
void SendData2Server(int count, int number)
{
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	char message[1024];
	sprintf(message, "<%d client> %s %d", number, "test", count);
	int ret = send(client, message, strlen(message), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	printf("Sent: %s\nbytes: %d\n\n", message, ret);
	ret = SOCKET_ERROR;
	int i = 0;
	while (ret == SOCKET_ERROR)
	{
		//получение ответа
		ret = recv(client, message, 1024, 0);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			//printf("Can't resieve message\n");
			/*closesocket(client);
			return;*/
			continue;
		}
		//вывод на экран количества полученных байт и сообщение
		printf("Recieve: %s\n bytes: %d\n", message, ret);
	}

	closesocket(client);
}
void sendToServer(char message[])
{
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	int ret = send(client, message, strlen(message), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	printf("Sent: %s\nbytes: %d\n\n", message, ret);
	ret = SOCKET_ERROR;
	int i = 0;
	while (ret == SOCKET_ERROR)
	{
		//получение ответа
		ret = recv(client, message, 1024, 0);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			//printf("Can't resieve message\n");
			/*closesocket(client);
			return;*/
			continue;
		}
		//вывод на экран количества полученных байт и сообщение
		printf("Recieve: %s\n bytes: %d\n", message, ret);
	}
	closesocket(client);
}
void recieveStatusFromServer(char operation_status[])
{
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	int ret = send(client, "Waiting for operation status", strlen("Waiting for operation status"), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	printf("Sent: %s\nbytes: %d\n\n", "Waiting for operation status", ret);
	ret = SOCKET_ERROR;
	while (ret == SOCKET_ERROR)
	{
		//получение ответа
		ret = recv(client, operation_status, 1024, 0);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			//printf("Can't resieve message\n");
			/*closesocket(client);
			return;*/
			continue;
		}
		//вывод на экран количества полученных байт и сообщение
		printf("Recieve: %s\n bytes: %d\n", operation_status, ret);
	}
	closesocket(client);
}
void recieveDataFromServer(char message[])
{
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	int ret = send(client, "Waiting for message", strlen("Waiting for message"), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	printf("Sent: %s\nbytes: %d\n\n", "Waiting for message", ret);
	ret = SOCKET_ERROR;
	while (ret == SOCKET_ERROR)
	{
		//получение ответа
		ret = recv(client, message, 1024, 0);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			//printf("Can't resieve message\n");
			/*closesocket(client);
			return;*/
			continue;
		}
		//вывод на экран количества полученных байт и сообщение
		printf("Recieve: %s\n bytes: %d\n", message, ret);
	}
	closesocket(client);
}
//void generalChatUpdate(HWND hwnd) {
//	char str[100] = { 0 };
//	fgets(str, 100, fgenchat);
//	while (!feof(fgenchat)) {
//		SetWindowText(chat, str);
//		fgets(str, 100, fgenchat);
//	}
//	
//}
LRESULT WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	if (message == WM_DESTROY)
		PostQuitMessage(0);
	else if (message == WM_MOUSEMOVE) {
		int xPos = LOWORD(lparam);
		int yPos = HIWORD(lparam);
	}
	else if (message == WM_COMMAND) {
		if (LOWORD(wparam) == ind_signup) {
			clearWindow(hwnd);
			HWND bt_signup;
			bt_signup = CreateWindow("button", "Зарегистрироваться", WS_VISIBLE | WS_CHILD, 120, 235, 150, 30, hwnd, (HMENU)ind_signup2, NULL, NULL);

			HDC dc = GetDC(hwnd);
			SetBkMode(dc, TRANSPARENT);
			SetTextColor(dc, RGB(0, 0, 0));
			TextOut(dc, 100, 100, "Придумайте логин", 17);
			TextOut(dc, 100, 170, "Придумайте пароль", 18);
			HWND get_username;
			get_username = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 130, 200, 20, hwnd, (HMENU)ind_username, NULL, NULL);
			HWND get_password;
			get_password = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 200, 200, 20, hwnd, (HMENU)ind_password, NULL, NULL);

		}
		else if (LOWORD(wparam) == ind_username) {
			GetWindowText((HWND)lparam, username, 30);	//принимаем информацию из окна для ввода логина в глобальную переменную
		}
		else if (LOWORD(wparam) == ind_password) {
			GetWindowText((HWND)lparam, password, 30);	//принимаем информацию из окна для ввода пароля в глобальную переменную
		}
		else if (LOWORD(wparam) == ind_message) {
			GetWindowText((HWND)lparam, sending_message, 54);
		}
		else if (LOWORD(wparam) == ind_enter) {
			strcpy_s(enter_data, "enter_data");	//передаем сообщение о том, что сейчас будут переданы данные для входа
			sendToServer(enter_data);
			strcpy_s(username_temp, username);
			sendToServer(username);	//передаем логин и пароль для входа
			sendToServer(password);
			char operation_status[20] = { 0 };
			recieveStatusFromServer(operation_status);	//получаем ответ от сервера
			if (strcmp(operation_status, "error") == 0) {	//если войти не получилось, то мы выводим сообщение об ошибке
				HDC dc = GetDC(hwnd);
				SetBkMode(dc, TRANSPARENT);
				SetTextColor(dc, RGB(255, 48, 48));
				TextOut(dc, 100, 70, "Неверный логин или пароль!", 27);
			}
			else if (strcmp(operation_status, "success") == 0) {	//если войти удалось, то мы очищаем окно
				clearWindow(hwnd);
				HDC dc = GetDC(hwnd);
				SetBkMode(dc, TRANSPARENT);
				SetTextColor(dc, RGB(0, 0, 0));
				TextOut(dc, 315, 2, "Общий чат", 10);
				
				//HWND chat;
				chat = CreateWindow("static", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 100, 20, 522, 420, hwnd, NULL, NULL, NULL);
				//HWND enter_message;
				enter_message = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 420, 442, 20, hwnd, (HMENU)ind_message, NULL, NULL);
				HWND bt_send;
				bt_send = CreateWindow("button", "Отправить", WS_VISIBLE | WS_CHILD, 542, 420, 80, 20, hwnd, (HMENU)ind_send, NULL, NULL);
			}
		}
		else if (LOWORD(wparam) == ind_signup2) {
			strcpy_s(signup_data, "signup_data");	//передаем сообщение о том, что сейчас будут переданы данные для регистрации
			sendToServer(signup_data);
			strcpy_s(username_temp, username);
			sendToServer(username);	//передаем логин и пароль для регистрации
			sendToServer(password);
			char operation_status[20] = { 0 };
			recieveStatusFromServer(operation_status);	//получаем ответ от сервера
			if (strcmp(operation_status, "error") == 0) {	//если зарегистрироваться не получилось, то мы выводим сообщение об ошибке
				HDC dc = GetDC(hwnd);
				SetBkMode(dc, TRANSPARENT);
				SetTextColor(dc, RGB(255, 48, 48));
				TextOut(dc, 100, 70, "Пользователь с таким логином уже существует!", 45);
			}
			else if (strcmp(operation_status, "success") == 0) {	//если зарегистрироваться удалось, то мы очищаем окно
				clearWindow(hwnd);
				HDC dc = GetDC(hwnd);
				SetBkMode(dc, TRANSPARENT);
				SetTextColor(dc, RGB(0, 0, 0));
				TextOut(dc, 300, 0, "Общий чат", 10);
				HWND chat;
				chat = CreateWindow("static", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL, 100, 20, 522, 420, hwnd, NULL, NULL, NULL);
				//HWND enter_message;
				enter_message = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 420, 442, 20, hwnd, (HMENU)ind_message, NULL, NULL);
				HWND bt_send;
				bt_send = CreateWindow("button", "Отправить", WS_VISIBLE | WS_CHILD, 542, 420, 80, 20, hwnd, (HMENU)ind_send, NULL, NULL);
				
			}
		}
		else if (LOWORD(wparam) == ind_send) {
			strcpy_s(send_data, "send_data");	
			sendToServer(send_data);
			char temp1[30] = { 0 };
			strcpy_s(temp1, username_temp);
			char temp2[30] = { 0 };
			strcpy_s(temp2, sending_message);
			sendToServer(username_temp);
			strcpy_s(sending_message, temp2);
			sendToServer(sending_message);
			strcpy_s(username_temp, temp1);
			strcpy_s(sending_message, temp2);
			//SetWindowText(enter_message, " ");
		}
	}
	//generalChatUpdate(hwnd);
	return DefWindowProcA(hwnd, message, wparam, lparam);
}
int main()
{
	char* locale;
	locale = setlocale(LC_ALL, "RUS");
	//fgenchat = fopen("fgenchat.txt", "rt");
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("Can't connect to socket lib");
		return 1;
	}
    WNDCLASSA wcl;
    memset(&wcl, 0, sizeof(WNDCLASSA));
    wcl.lpszClassName = "my Window";
    wcl.lpfnWndProc = (WNDPROC)WndProc;
    RegisterClassA(&wcl);

    HWND hwnd;
    hwnd = CreateWindow("my Window", "Window Name", WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, NULL, NULL, NULL, NULL);
    ShowWindow(hwnd, SW_SHOWNORMAL);	//создаем наше собственное окно


    HWND bt_enter;
    bt_enter = CreateWindow("button", "Войти", WS_VISIBLE | WS_CHILD, 155, 235, 80, 30, hwnd, (HMENU)ind_enter, NULL, NULL);
    HWND bt_signup;
    bt_signup = CreateWindow("button", "Зарегистрироваться", WS_VISIBLE | WS_CHILD, 120, 300, 150, 30, hwnd, (HMENU)ind_signup, NULL, NULL);

    HDC dc = GetDC(hwnd);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(0, 0, 0));
    TextOut(dc, 100, 100, "Введите логин", 14);
    TextOut(dc, 100, 170, "Введите пароль", 14);
    /*HWND enter_username;
    enter_username = CreateWindow("static", "Введите логин", WS_VISIBLE | WS_CHILD, 100, 100, 110, 20, hwnd, NULL, NULL, NULL);
    HWND enter_password;
    enter_password = CreateWindow("static", "Введите пароль", WS_VISIBLE | WS_CHILD, 100, 170, 110, 20, hwnd, NULL, NULL, NULL);*/
    HWND get_username;
    get_username = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 130, 200, 20, hwnd, (HMENU)ind_username, NULL, NULL);
    HWND get_password;
    get_password = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD, 100, 200, 200, 20, hwnd, (HMENU)ind_password, NULL, NULL);
    //profilePhoto(hwnd, 10, 10, 90);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	//int i = 0;
	//srand(time(0));
	//rand();
	//int number = rand();
	//while (i < 100)
	//{
	//	SendData2Server(++i, number);
	//	Sleep(rand() % 10);
	//}
	//printf("Session is closed\n");
	//Sleep(1000);
	//fclose(fgenchat);
	return 0;
}