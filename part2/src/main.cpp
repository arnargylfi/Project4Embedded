#include <iostream>
#include <sys/syscall.h>
#include <unistd.h>

int main() {

    int pos = syscall(SYS_get_pos);
}


