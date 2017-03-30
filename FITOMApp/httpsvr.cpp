#include <stdio.h>
#include <sys/types.h>
#include <winsock.h>

int
httpsvr()
{
	SOCKET sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	SOCKET sock;
	int yes = 1;

	char buf[2048];
	char inbuf[2048];

	sock0 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock0 < 0) {
		perror("socket");
		return 1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(sock0,
		SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

	if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		perror("bind");
		return 1;
	}

	if (listen(sock0, 5) != 0) {
		perror("listen");
		return 1;
	}

	// �����pHTTP���b�Z�[�W�쐬
	memset(buf, 0, sizeof(buf));
	_snprintf(buf, sizeof(buf),
		"HTTP/1.0 200 OK\r\n"
		"Content-Length: 20\r\n"
		"Content-Type: text/html\r\n"
		"\r\n"
		"HELLO\r\n");

	while (1) {
		len = sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &len);
		if (sock < 0) {
			perror("accept");
			break;
		}

		memset(inbuf, 0, sizeof(inbuf));
		recv(sock, inbuf, sizeof(inbuf), 0);
		// �{���Ȃ�΃N���C�A���g����̗v�����e���p�[�X���ׂ��ł�
		printf("%s", inbuf);

		// ���肪�����������ƃ_�~�[HTTP���b�Z�[�W���M
		send(sock, buf, (int)strlen(buf), 0);

		closesocket(sock);
	}

	closesocket(sock0);

	return 0;
}
