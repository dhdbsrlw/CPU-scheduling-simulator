#ifndef __FIRST__COME__FIRST__SERVED__
#define __FIRST__COME__FIRST__SERVED__

// 사용자 정의 헤더
#include "./Process.h"
#include "./Sorting.h"


/*Context Switch 로 인한 delay 시간 미고려*/

// Gantt Chart 출력 (대대적인 알고리즘 수정 필요)
void fcfs_print_gantt_chart(Process p[], int len) {
    
    print("\t ");

    // 상단 바 출력 (burst 시간에 비례하여 출력 필요)
    for (int i = 0; i < len; i++) {
        

    }

}


// FCFS Algorithm 
void FCFS(p[], int len) {

    // 프로세스 초기화
    process_init(p, len);

    // 도착시간을 기준으로 Ready Queue 에 프로세스 정렬
    merge_sort_by_arrive_time(p, 0, len);

    // 성능 평가 목적의 변수
    int tot_waiting_time = 0, tot_turnaround_time = 0, tot_response_time = 0;
    

    // IO 연산으로 인해 뒷순서 멤버의 waiting_time에 미치는 영향
    int effected_wait[10] = { 0, };
    // IO 연산으로 인해 뒷순서 멤버의 response_time에 미치는 영향 
    int effected_res[10] = { 0, };
    // I/O 시작(까지 소요) 시간 랜덤 생성 (response 이후 1초 후 시작)
    int io_start_time = 1;


    for (int i = 0; i < len, i++) {
        
        // p[i] 는 I/O 로 인한 영향 없음
        p[i].response_time = effected_res[i]; // 완료
        p[i].waiting_time = p[i].response_time - p[i].arrival_time + effected_wait[i]; // 완료
        p[i].return_time = p[i].response_time + p[i].CPU_burst_time + p[i].IO_burst_time + effected_wait[i]; // 각 프로세스마다의 종료시각
        p[i].turnaround_time = p[i].return_time - p[i].arrival_time; // 완료


        /* I/O 가 발생하는 프로세스인 경우 */
        if (p[i].IO_burst_time != 0) { 

            // p[i+1] I/O 로 인한 영향 있음  
            int left = p[i].IO_burst_time;

            for (int j = i + 1; left > 0; j++) {

                // 이전에 도착한 프로세스가 존재할 경우
                if (p[j].arrival_time < p[i].response_time + 1) {
        
                    // 종료 조건
                    if (p[j].CPU_burst_time >= left) { 

                        // 영향받은 요소
                        effected_res[j] = p[i].response_time + io_start_time;   // p[j]의 response time 화
                        effected_wait[j] = p[i].CPU_burst_time - 1;             // p[j]의 waiting time 에서 덧셈 
                        break;
                    }

                    // 반복 조건 (IO burst time 구간에 2개 이상의 프로세스 실행)
                    else {
                        effected_res[j] = p[i].response_time + io_start_time; // p[j]의 response time 화
                        left =- p[j].CPU_burst_time;
                        io_start_time += p[j].CPU_burst_time;
                    }
                }


                // I/O burst 도중에 CPU 할당할 프로세스가 생긴 경우
                else if (/* condition */) {
                    /* code */
                }

                // I/O burst 동안 CPU 할당할 타 프로세스가 없는 경우
                else {
                    // 무無 (추후 수정)

                }

            }
        }
    }
        
        // 실행된 프로세스만큼 증가
        tot_response_time += p[i].response_time; // 완료
        tot_waiting_time += p[i].waiting_time; // 완료
        tot_turnaround_time += p[i].turnaround_time; // 완료
}


#endif