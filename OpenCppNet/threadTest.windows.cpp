#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
HANDLE thread;

DWORD WINAPI (*routine)(void *arg) = [](void *arg){
    printf("%s - is value of (%p)\n", arg, arg);
    return (DWORD)(0);
};

int main(int argc, char **argv){
    thread = CreateThread(NULL, 0, routine, argv[0], 0, 0);
    system("pause");
}