/*********************************************
 * Operating System Term project             *
 * CPU Scheduling Simulator                  *
 *                                           *
 * worked by 2020120036 경영학과 오윤진       *
 *                                           *
 * Last update: 05/08                        *
*********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <Process.h>

int main(void) {

Process * process;

// 프로세스 개수 랜덤 생성 (최대 5개)
srand(time(NULL));
int p_cnt = (rand() % 5) + 1;       

// 개수 만큼의 프로세스 생성
// 메모리 할당

// 프로세스 환경 랜덤 생성
for (int i; i < p_cnt; i++){
    p_id = (rand() % 9000) + 1000; // 4자리로 생성

}




return 0;
}
