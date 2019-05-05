#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <fileapi.h>

WSADATA wsa;
SOCKET server;
SOCKADDR_IN serveraddr, clientaddr;

int main()
{
	WSAStartup(MAKEWORD(2, 2), &wsa);
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	serveraddr.sin_addr.s_addr = htonl(ADDR_ANY);
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_family = AF_INET;
	bind(server, (SOCKADDR*)& serveraddr, sizeof(serveraddr));
	listen(server, 5);

	char buff[1024], request[5], link[128], http11[6];
	int clientaddr_length = sizeof(clientaddr);
	int ret = 0;
	while (true) {
		SOCKET client = accept(server, NULL, NULL);
		printf("New client accepted: %d\n", client);
		ret = recv(client, buff, sizeof(buff), 0);
		if (ret > 0) {
			buff[ret] = 0;
			sscanf(buff, "%s %s %s", request, link, http11);
			char  dir[128] = "C:";
			// hanlde request
			if (strcmp(link, "/") == 0) {
				//convert
				for (int i = 0; i < strlen(link); i++) {
					if (link[i] == '/') link[i] = '\\';
				}
				// send list dir and file

				char html[2048] = "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=utf-8\n\n\r\n\r\n<html><body>";
				WIN32_FIND_DATAA DATA;
				HANDLE h = FindFirstFileA("C:\\*.*", &DATA);
				do {
					if ((DATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (strcmp(DATA.cFileName, ".") != 0)) {
						strcat(html, "<a href=\"");
						strcat(html, DATA.cFileName);
						strcat(html, "\">");
						strcat(html, DATA.cFileName);
						strcat(html, "</a></br>");
					}
					else {
						INT64 size = (DATA.nFileSizeHigh * (MAXDWORD + 1)) + DATA.nFileSizeLow;
						strcat(html, "<a href=\"");
						strcat(html, DATA.cFileName);
						strcat(html, "\"><i>");
						strcat(html, DATA.cFileName);
						strcat(html, "<i/></a></br>");
					}
				} while (FindNextFileA(h, &DATA));
				strcat(html, "</body></html>");
				send(client, html, strlen(html), 0);

			}
		}
		else continue;
		closesocket(client);
	}

	closesocket(server);
	WSACleanup();
}
