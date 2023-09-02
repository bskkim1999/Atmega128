#define P_GAIN	4
#define	I_GAIN	0.001
#define D_GAIN	1.18
#define TIME	0.01

#include <stdio.h>

unsigned int target = 2014;  //고정된 값이다.
unsigned int current = 0;    //실시간 값이 변동된다.

float pControl = 0;
float iControl = 0;
float dControl = 0;
float pidControl = 0;
float realError = 0;
float accError = 0;
float errorGap = 0;

int tmp = 0;
void calculateErrors(void) {
	errorGap = target - current - realError;
	realError = target - current;	// 실시간 에러는 단순히 목표값 - 현재값을 의미합니다.
	accError += realError;
	/*
		누적 에러는 실시간 에러를 계속해서 더한 값입니다.
		I 제어를 위해 사용됩니다.
	*/


}

void pControlSystem(void) {

	pControl = P_GAIN * realError;

}

void iControlSystem(void) {

	iControl = I_GAIN * (accError );

}

void dControlSystem(void) {

	dControl = D_GAIN * (errorGap );

}

void pidControlSystem(void) {

	pidControl = pControl + iControl + dControl;

}


int main() {

	float x;
	pidControl = 5;   //속도

	while (1) {

		

		calculateErrors();
		pControlSystem();
		iControlSystem();
		dControlSystem();
		pidControlSystem();

		//속도는 한계가 있다.
		/*
		if (pidControl >=100) {
			pidControl = 100;
		}
		*/
		
		x = pidControl;   //속도

		current = current + (50 + tmp);  //위치는 속도에 따라서 변동된다.
		
		tmp = tmp + 2;

		printf("pControl : %f \n", pControl);
		printf("iControl : %f \n", iControl);
		printf("dControl : %f \n", dControl);
		printf("pidControl : %f \n", pidControl);

		printf("realerror : %f \n", realError);
		printf("errorgap : %f \n", errorGap);
		printf("current : %d \n", current);

		if (current >= target) {
			break;
		}


		//printf("1");
		//printf("current : %d \n", current);
		printf("======================== \n");
		

		
	}
	




	


	return 0;
}







