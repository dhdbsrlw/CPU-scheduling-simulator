#ifndef __SORTING__FUNCTION__
#define __SORTING__FUNCTION__
#include "./Process.h"


// 프로세스 반환 시간 기준 비교 함수
int compare_by_return_time(const void *a, const void *b){

	// const void 형 변수 형 변환
	Process *ptA = (Process *)a;
	Process *ptB = (Process *)b;


	// ptA의 반환 시간이 작을 경우 
	if (ptA->return_time < ptB->return_time)
		return -1;
		

	// ptA의 반환 시간이 클 경우
	else if (ptA->return_time > ptB->return_time)
		return 1;
		

	// 반환 시간이 같을 경우 (존재 X)
	else
		return 0;
}



// 반환 시간 기준 퀵소트 함수
void quick_sort_by_return_time(Process p[], int p_cnt) {
	qsort(p, p_cnt, sizeof(Process), compare_by_return_time);
}



// (프로세스 재배열 목적의) 머지소트 함수
void merge_sort(Process p_arr[], int left, int mid, int right) {
    // 배열 인덱스 선언
	int fIdx = left;
	int rIdx = mid + 1;
	int i;

    // 정렬된 배열을 저장할 배열 동작 할당
	Process *sort_arr = (Process *)malloc(sizeof(Process) * (right + 1));
	int sIdx = left;


	// left부터 mid까지의 블록과 mid + 1부터 right까지의 블록을 비교하는 부분
	while (fIdx <= mid && rIdx <= right)
	{
		// left의 도칙시간이 right보다 작을 경우
		if (p_arr[fIdx].arrive_time <= p_arr[rIdx].arrive_time)
			// left의 데이터를 결과 배열에 복사
            sort_arr[sIdx] = p_arr[fIdx++];
			
		// 그렇지 않을 경우
		else
			// right의 데이터를 결과 배열에 복사
			sort_arr[sIdx] = p_arr[rIdx++];

		// 결과 배열의 인덱스 증가
		sIdx++;
	}


	// 오른쪽 배열의 데이터가 남아있는 경우
	if (fIdx > mid)
	{
		// 오른쪽 배열의 남은 데이터 만큼 반복
		for (i = rIdx; i <= right; i++, sIdx++)
			// 오른쪽 배열의 데이터를 결과 배열에 복사
			sort_arr[sIdx] = p_arr[i];
	}

	// 왼쪽 배열의 데이터가 남아있는 경우
	else
	{
		// 왼쪽 배열의 남은 데이터 만큼 반복
		for (i = fIdx; i <= mid; i++, sIdx++)
			// 왼쪽 배열의 데이터를 결과 배열에 복사
			sort_arr[sIdx] = p_arr[i];
	}


	// 배열의 데이터 개수만큼 반복
	for (i = left; i <= right; i++)
		// 원본 배열에 복사
        p_arr[i] = sort_arr[i];

	free(sort_arr);
	// 배열 메모리 할당 해제
}



// 도착 시간 기준 머지소트 함수
void merge_sort_by_arrive_time(Process p_arr[], int left, int right) {
	// 배열의 중간 지점 인덱스
	int mid;

	if (left < right)
	{
		// 중간 지점 계산
		mid = (left + right) / 2;

		// 둘로 나누어 각각 정렬
		merge_sort_by_arrive_time(p_arr, left, mid);
		merge_sort_by_arrive_time(p_arr, mid + 1, right);

		// 정렬된 두 배열 병합
		merge_sort(p_arr, left, mid, right);
	}
}


#endif