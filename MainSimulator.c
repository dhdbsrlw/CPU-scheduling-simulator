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
#include <unistd.h>
#include <Process.h>

int main(void) {

// 프로세스 개수 랜덤 생성 (최대 5개)
srand(time(NULL));
int p_cnt = (rand() % 5) + 1;       


// 개수 만큼의 프로세스 생성 (동적 메모리 할당)
struct Process * process = (struct Process *)malloc(sizeof(struct Process) * p_cnt);


// 프로세스 환경 초기화 (랜덤 생성)
for (int i = 0; i < len; i++){

    process[i].p_id = (rand() % 9000) + 1000;        // 4자리 생성
    process[i].CPU_burst_time = (rand() % 10) + 1;   // 최대 시간 10 (최소 설정 필요)
    process[i].IO_burst_time = (rand() % 10) + 1;    // 최대 시간 10 (확률로 발생빈도 저하 필요)

    if (i == 0) {
        process[i].arrival_time = 0;
    }
    else {
        process[i].arrival_time = (rand() % 11)};     // 최대 시간 10

    process[i].priority = (rnad() % len) + 1;
}


free(process);
return 0;

}
