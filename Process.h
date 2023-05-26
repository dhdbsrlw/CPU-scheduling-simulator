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


// 프로세스 구조체 
typedef struct _process {

    char pid[ID_LEN];        // 프로세스 ID
    int CPU_burst_time;      // 실행 시간
    int IO_burst_time;       // 입출력 시간
    int IO_start_time;       // 입출력 시작 시간
    int arrival_time;        // 도착 시간
    int turnaround_time;     // 턴어라운드 시간
    int terminate_time;      // 종료 시간
    int response_time;       // 응답 시간
    int priority;            // 우선 순위
    int completed;           // 완료 상태 (TRUE 1 또는 FALSE 0)

} Process;


typedef int Quantum; // 시간 할당량 Quantum 키워드 사용 (이해 필요)


// 프로세스 초기화 (p[]: 프로세스 구조체 배열, len: 프로세스 개수)
void process_init(Process p[], int len) {

    for (int i = 0; i < len; i++){
        p[i].waiting_time = 0;   // 대기 시간 초기화
        p[i].completed = FALSE;  // 완료 상태 초기화
    }
}


#endif