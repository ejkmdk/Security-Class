#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;


int main(){
	
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1){
		return 1;
	}
	
	int port = 54000;

	string ipAddress = "127.0.0.1";

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	int connectResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connectResult == -1){
		return 1;
	}
	
	char buf[4096];
	string userInput;

	do{
		cout << "Enter your password: ";
		getline(cin, userInput);
		int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
		if (sendResult == -1){
			cout << "Did not send to server" << endl;
			continue;
		}

		memset(buf, 0, 4096);
		int bytesRecived = recv(sock, buf, 4096, 0);

		cout << "Server: " << string(buf, bytesRecived) << endl;
		break;
	}while(true);

	close(sock);

	return 0;
}

