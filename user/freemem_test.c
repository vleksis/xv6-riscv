#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    int mem = freemem();
    fprintf(1, "Free memory: %d bytes\n", mem);
    exit(0);
}