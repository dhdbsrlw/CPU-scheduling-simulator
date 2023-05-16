#ifndef __PROCESS__
#define __PROCESS__

// 헤더 파일 선언
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 프로그램에서 사용하는 기호상수 정의
#define ID_LEN 20
#define TRUE 1
#define FALSE 0


// 프로세스 구조체 (random data 부여 필요)
typedef struce _process {

    char pid[ID_LEN];    // 프로세스 ID
    int CPU_btime;       // 실행 시간
    int IO_btime;       // 입출력 시간
    int arrival_time;    // 도착 시간
    int return_time;     // (작업 처리 도중) 반환 시간
    int priority;        // 우선 순위
    int completed;       // 완료 상태 (TRUE 1 또는 FALSE 0)

} Process;


typedef int Quantum; // 시간 할당량 Quantum 키워드 사용 (이해 필요)


// 프로세스 초기화 (p[]: 프로세스 구조체 배열, p_cnt: 프로세스 개수)
void process_init(Process p[], int p_cnt) {

    for (int i = 0; i < p_cnt; i++){

        process[i].p_id = (rand() % 9000) + 1000; // 4자리로 생성
        process[i].CPU_btime = (rand() % 10) + 1;
        process[i].IO_btime = (rand() % 10) + 1;
        process[i].arrival_time = (rand() % 10) + 1;
        process[i].priority = (rnad() % p_cnt) + 1;
        p[i].completed = FALSE;
}


    }

#endif