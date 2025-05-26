#include <string.h>
#include <stdio.h>
#include "mbed.h"
#include "L3_host.h"

// 외부 선언된 Serial 객체 참조 
extern Serial pc;

void printRoleFromInput(const char* input) {
    if (strcmp(input, "0") == 0) {
        pc.printf("역할은 호스트\n");
    } else {
        pc.printf("역할은 플레이어\n");
    }
}