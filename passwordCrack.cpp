#include "server.h"
#include <iostream>

using namespace std;

int main(){

	int works = runServer();
	if(works >= 0){
		cout << "Server Got Password";
	}

	return 0;
}
