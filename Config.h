#ifndef __CONFIG__
#define __CONFIG__
#include <unistd.h>
#include "./Process.h"
#include "./Sorting.h"

// Ready Queue, Waiting(Device) Queue 선언 (프로세스 최대 개수 5)
Process ready_queue[10];
Process waiting_queue[10]; 

/* 
언제 waiting queue로? - I/O 오퍼레이션 대기
*/