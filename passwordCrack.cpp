#include "server.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <string.h>
#include <string>
#include <condition_variable>

using namespace std;

const int PASSSIZE_MAX = 10;
const char Chars[93] = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=[];',./\\!@#$%^&*()_+{}|:<>? " };
//using stopper = atomic<bool>;
string Cracker(string PassToFind, int * foundPass);
void CrackerLimited(string PassToFind, int* foundPass, int* endPass, int foundPassSizeStart, int foundPassSizeEnd, bool& passwordFound, condition_variable& cv);
string GetTestPass(int size, int * foundPass);
int IncrementByOne(int foundPassSize, int * foundPass);

int main(){

	int foundPass[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    	int foundPass2[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    	int foundPass21[PASSSIZE_MAX] = { 0, 0, 0, 0, 92, 0, 0, 0, 0, 0 };
    	int foundPass22[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 92, 0, 0, 0, 0 };
    	int foundPass31[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 92, 0, 0, 0, 0 };
    	int foundPass32[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 0, 92, 0, 0, 0 };
    	int foundPass41[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 0, 92, 0, 0, 0 };
    	int foundPass42[PASSSIZE_MAX] = { 0, 0, 0, 0, 0, 0, 0, 92, 0, 0 };
//    	int PASS_MAX[PASSSIZE_MAX] = { 92, 92, 92, 92, 92, 92, 92, 92, 92 };
	int works = runServer();
	if(works >= 0){
		cout << "Server Got Password" << endl;
	}else{
		cout << "Server Could Not Get Password\n";
		return 0;
	}

	ifstream testfile;
    	testfile.open("totally_secure_password_storage.txt");
    	string StringToFind;
    if (testfile.is_open()) {
	getline(testfile, StringToFind);
    }
    //Run password cracker and record time taken to find password
    bool passwordFound = false;
    condition_variable cv;
    
    auto begin = chrono::high_resolution_clock::now(); //Start measuring time to find the password
    cout << "Starting tests..." << endl;
    
    thread t1(CrackerLimited, StringToFind, foundPass, foundPass2, 1, 5, ref(passwordFound), ref(cv));
    thread t2(CrackerLimited, StringToFind, foundPass21, foundPass22, 5, 7, ref(passwordFound),  ref(cv));
    thread t3(CrackerLimited, StringToFind, foundPass31, foundPass32, 7, 9, ref(passwordFound), ref(cv));
    thread t4(CrackerLimited, StringToFind, foundPass41, foundPass42, 9, 10, ref(passwordFound), ref(cv));
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    auto end = chrono::high_resolution_clock::now(); //Stop the timer
    cout << chrono::duration_cast<chrono::milliseconds>(end - begin).count() << "ms" << endl; //Time to complete, either finding the password or returning a failure

	return 0;
}

void CrackerLimited(string PassToFind, int* foundPass, int* endPass, int foundPassSizeStart, int foundPassSizeEnd, bool& passwordFound, condition_variable& cv)
{
    int foundPassSize = foundPassSizeStart;
    int numAttempts = 1;
    string totalString;
    //cout << "Starting Tests..." << endl;
    while (!passwordFound) // try a password based on numAttempts
    {
        string tryString = GetTestPass(foundPassSize, foundPass);
        if (tryString == PassToFind) {
            passwordFound = true;;
            totalString = tryString;
    	    cout << "Found the password: " << totalString << endl;
	    cout << "Number of Attempts This Thread: " << numAttempts << endl;
	    cv.notify_all();
	    return;
        }
        else {
            foundPassSize = IncrementByOne(foundPassSize, foundPass);
            numAttempts += 1;
            if (foundPass == endPass || foundPassSize >= foundPassSizeEnd) {
                passwordFound = false;
                totalString = "Failed to find within given limits";
		cv.notify_all();
		return;
            }
        }
        //cout << tryString << endl;
    }
    //cout << "Number of Attempts: " << numAttempts << endl;
}

string GetTestPass(int size, int* foundPass)
{
    string tryString = "";
    for (int i = 0; i < size; i++)
    {
        tryString += Chars[foundPass[i]];
    }
    return tryString;
}

int IncrementByOne(int foundPassSize, int * foundPass )
{
    bool Carry = true;
    int i = 0;
    while (i < foundPassSize) {
        if (Carry) {
            if (foundPass[i] < 91) {
                foundPass[i] += 1;
                Carry = false;
            }
            else {
                foundPass[i] = 0;
                if (foundPassSize == (i + 1)) {
                    foundPassSize += 1;
                    foundPass[i + 1] -= 1;
                    //cout << "Password Size increased to: " << foundPassSize << endl;
                }
                Carry = true;
            }
        }
        i++;
    }
    return foundPassSize;

}
