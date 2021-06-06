// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>
#include <locale.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <winsock.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define _WIN32_WINNT 0x0A00
#include <windows.h>

pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

char recieved_message[50];	//просто глоабльная переменная для сообщений, которые мы получаем от клиента
char operation_status[20];	//"error"-если войти или зарегистрироваться не удалось, "success"-если удалось
typedef struct Client {	//структура для клиента с логином и паролем
	char username[30];
	char password[30];
}CLIENT;
void clientsUpload(FILE* fclients, CLIENT clients[], int clients_count) {	//выводим в файл логины и пароли зарегистрированных пользователей
	for (int i = 0; i < clients_count; ++i) {
		fprintf(fclients, "%s\n", clients[i].username);
		fprintf(fclients, "%s\n", clients[i].password);
	}
}
void clientsDownload(FILE* fclients, CLIENT clients[], int* clients_count) {	//выводим логины и пароли из файла в массив структур
	char str[30] = { 0 };
	fgets(str, 30, fclients);
	while (!(feof(fclients))) {
		if (str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0';
		strcpy_s(clients[*clients_count].username, str);
		fgets(str, 30, fclients);
		if (str[strlen(str) - 1] == '\n')
			str[strlen(str) - 1] = '\0';
		strcpy_s(clients[*clients_count].password, str);
		printf("u: %s\np: %s\n\n", clients[*clients_count].username, clients[*clients_count].password);
		fgets(str, 30, fclients);
		*clients_count += 1;
	}
}
int usernameSearch(CLIENT clients[], int clients_count, char username[]) {	//поиск логина в базе зарегистрированных
	for (int i = 0; i < clients_count; ++i) {
		if (strcmp(clients[i].username, username) == 0)
			return i;
	}
	return -1;	//i - есть такой логин, -1 - нет такого логина
}
void* ClientStart(void* param)
{
	SOCKET client = (SOCKET)param;
	char recieve[1024], transmit[1024];
	int ret;

	ret = recv(client, recieve, 1024, 0);	//получаем сообщение от клиента
	if (!ret || ret == SOCKET_ERROR)
	{
		pthread_mutex_lock(&mutex);
		pthread_mutex_lock(&mutex_file);
		printf("Error getting data\n");
		fprintf(stdout, "test");
		pthread_mutex_unlock(&mutex_file);
		pthread_mutex_unlock(&mutex);
		return (void*)1;
	}
	recieve[ret] = '\0';
	strcpy_s(recieved_message, recieve);
	pthread_mutex_lock(&mutex);
	pthread_mutex_lock(&mutex_file);
	//fprintf(stdout, "test");
	printf("%s\n", recieve);
	pthread_mutex_unlock(&mutex_file);
	pthread_mutex_unlock(&mutex);
	if (strcmp(recieve, "Waiting for operation status") == 0) {	
		ret = send(client, operation_status, sizeof(operation_status), 0);
		//если клиент ждет статуса операции (то есть он пытается войти или зарегистрироваться), то мы отправляем статус
	}
	else {
		sprintf_s(transmit, "%s %s %s\n", "Your data", recieve, "recieved");
		//Sleep(2000);
		ret = send(client, transmit, sizeof(transmit), 0);
	}
	if (ret == SOCKET_ERROR)
	{
		pthread_mutex_lock(&mutex);
		pthread_mutex_lock(&mutex_file);
		printf("Error sending data\n");
		pthread_mutex_unlock(&mutex_file);
		pthread_mutex_unlock(&mutex);
		return (void*)2;
	}
	return (void*)0;
}
//int CreateServer(CLIENT clients[], int* clients_count, int* operation)
//{
//	SOCKET server, client;
//	sockaddr_in localaddr, clientaddr;
//	int size;
//	server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
//	if (server == INVALID_SOCKET)
//	{
//		printf("Error create server\n");
//		return 1;
//	}
//	localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	localaddr.sin_family = AF_INET;
//	localaddr.sin_port = htons(5510);//port number is for example, must be more than 1024
//	if (bind(server, (sockaddr*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR)
//	{
//		printf("Can't start server\n");
//		return 2;
//	}
//	else
//	{
//		printf("Server is started\n");
//	}
//	listen(server, 50);//50 клиентов в очереди могут стоять
//	pthread_mutex_init(&mutex, NULL);
//	pthread_mutex_init(&mutex_file, NULL);
//	while (1)
//	{
//		size = sizeof(clientaddr);
//		client = accept(server, (sockaddr*)&clientaddr, &size);
//
//		if (client == INVALID_SOCKET)
//		{
//			printf("Error accept client\n");
//			continue;
//		}
//		else
//		{
//			printf("Client is accepted\n");
//		}
//		pthread_t mythread;
//		int status = pthread_create(&mythread, NULL, ClientStart, (void*)client);
//		pthread_detach(mythread);
//		if (strcmp(recieved_message, "enter_data") == 0) {
//			printf("oh yeah");
//			*operation = 0;
//		}
//		else if (*operation == 0) {
//			strcpy_s(clients[*clients_count].username, recieved_message);
//			*operation = 1;
//		}
//		else if (*operation == 1) {
//			strcpy_s(clients[*clients_count].password, recieved_message);	
//			*clients_count += 1;
//			*operation = -1;
//		}
//		else if (strcmp(recieved_message, "signup_data") == 0)
//			*operation = 2;
//		else if (*operation == 2) {
//			strcpy_s(clients[*clients_count].username, recieved_message);
//			*operation = 3;
//		}
//		else if (*operation == 3) {
//			strcpy_s(clients[*clients_count].password, recieved_message);
//			*clients_count += 1;
//			*operation = -1;
//		}
//		printf("\n\nrecieved message: %s\n", recieved_message);
//		printf("operation: %d\n", *operation);
//		printf("%d clients exist\n\n", *clients_count);
//	}
//	pthread_mutex_destroy(&mutex_file);
//	pthread_mutex_destroy(&mutex);
//	printf("Server is stopped\n");
//	closesocket(server);
//	return 0;
//}
int main()
{
	char* locale;
	locale = setlocale(LC_ALL, "RUS");
	int operation = -1; //0 - логин для входа, 1 - пароль для входа, 2 - логин для регистрации, 3 - пароль для регистрации
	CLIENT clients[100];
	int clients_count = 0;
	FILE* fclients = fopen("fclients.txt", "rt");
	clientsDownload(fclients, clients, &clients_count);
	//printf("***%d***\n", clients_count);
	fclose(fclients);
	FILE* fgenchat = fopen("fgenchat.txt", "at");
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0)
	{
		printf("Connected to socket lib\n");
	}
	else
	{
		return 1;
	}
	//return CreateServer(clients, &clients_count, &operation);
	SOCKET server, client;
	sockaddr_in localaddr, clientaddr;
	int size;
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (server == INVALID_SOCKET)
	{
		printf("Error create server\n");
		return 1;
	}
	localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(5510);//port number is for example, must be more than 1024
	if (bind(server, (sockaddr*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR)
	{
		printf("Can't start server\n");
		return 2;
	}
	else
	{
		printf("Server is started\n");
	}
	listen(server, 50);//50 клиентов в очереди могут стоять
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_file, NULL);
	time_t time_temp = time(NULL);
	while (1)
	{
		time_t timer = time(NULL);
		if ((long int)timer - (long int)time_temp > 60 * 2)	//если сервер работает дольше какого-то времени, то он прекращает работу
			break;
		size = sizeof(clientaddr);
		client = accept(server, (sockaddr*)&clientaddr, &size);

		if (client == INVALID_SOCKET)
		{
			printf("Error accept data\n");
			continue;
		}
		else
		{
			printf("Data is accepted\n");
		}
		pthread_t mythread;
		int status = pthread_create(&mythread, NULL, ClientStart, (void*)client);
		pthread_detach(mythread);
		//здесь мы начинаем проверять полученные данные
		//значения для 'operation': 0 - логин для входа, 1 - пароль для входа, 2 - логин для регистрации, 3 - пароль для регистрации,
		//4 - логин для отправки сообщения, 5 - сообщение
		if (strcmp(recieved_message, "enter_data") == 0) {	//запрос на вход
			operation = 0;
		}
		else if (operation == 0) {
			strcpy_s(clients[clients_count].username, recieved_message);
			operation = 1;
		}
		else if (operation == 1) {
			strcpy_s(clients[clients_count].password, recieved_message);
			int i = usernameSearch(clients, clients_count, clients[clients_count].username);
			if (i == -1) {
				printf("Wrong username or password\n");
				strcpy_s(operation_status, "error");
				ClientStart;	//если такого логина нет, посылаем сообщение об ошибке
			}
			else {
				if (strcmp(clients[clients_count].password, clients[i].password) != 0) {
					printf("Wrong username or password\n");
					strcpy_s(operation_status, "error");
					ClientStart;	//если такой логин есть, но пароль неправильный, посылаем сообщение об ошибке
				}
				else {
					printf("You entered the chat\n");
					strcpy_s(operation_status, "success");
					ClientStart;	//если все ок, говорим, что все ок
				}
			}
			operation = -1;
		}
		else if (strcmp(recieved_message, "signup_data") == 0) {	//запрос на регистрацию
			operation = 2;
		}
		else if (operation == 2) {
			strcpy_s(clients[clients_count].username, recieved_message);
			operation = 3;
		}
		else if (operation == 3) {
			strcpy_s(clients[clients_count].password, recieved_message);
			if (usernameSearch(clients, clients_count, clients[clients_count].username) == -1) {
				printf("You were signed up\n");
				strcpy_s(operation_status, "success");
				ClientStart;	//если такого логина нет, то мы говорим, что все ок, и регистрируем пользователя
				clients_count += 1;
			}
			else {
				printf("This username already exists\n");
				strcpy_s(operation_status, "error");
				ClientStart;	//если логин уже существует, мы сообщаем об ошибке
			}
			operation = -1;
		}
		else if (strcmp(recieved_message, "send_data") == 0) {	//запрос на отправку сообщения в общий чат

			operation = 5;
		}
		else if (operation == 5) {
			fprintf(fgenchat, "%s: ", recieved_message);
			printf("!!!%s: ", recieved_message);
			operation = 6;
		}
		else if (operation == 6) {
			fprintf(fgenchat, "%s\n", recieved_message);
			printf("!!!%s\n", recieved_message);
			operation = -1;
		}
		printf("recieved message: %s\n", recieved_message);		//просто строчки для отслеживания работы программы
		printf("operation: %d\n", operation);
		printf("%d clients exist\n\n\n", clients_count);
	}
	pthread_mutex_destroy(&mutex_file);
	pthread_mutex_destroy(&mutex);
	printf("Server is stopped\n");
	closesocket(server);
	printf("***%d***\n", clients_count);
	fclients = fopen("fclients.txt", "wt");
	clientsUpload(fclients, clients, clients_count);
	fclose(fclients);
	fclose(fgenchat);
	return 0;
}