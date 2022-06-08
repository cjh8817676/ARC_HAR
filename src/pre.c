#include "stdio"
#define data_len 300
int main(){

	float filtered[data_len];
	///////////////////////////enlarging
	for(int i = 0; i < data_len; i ++){
		filtered[i] = filtered[i] / 10;
		filtered[i] = filtered[i] * filtered[i];
	}
	//////////////////////////normalize
	float min = 9999999;
	float max = 0;
	for(int i = 0; i < data_len; i ++){
		if (min > filtered[i]){
			min = filtered[i];
		}
		if (max < filtered[i]){
			max = filtered[i];
		}
	}
	max = max - min;
	for(int i = 0; i < data_len; i ++){
		
		filtered[i] = filtered[i] - min;
		filtered[i] = filtered[i] / max;
	}
	////////////////////////diff and ^2
	float diff[data_len - 1];
	for(int i = 0; i < data_len - 1; i ++){
		diff[i] = filtered[i + 1] - filtered[i];
		diff[i] = diff[i] * diff[i];
	}
	////////////////////////ma
	float ma[data_len - 1];
	float mm = 0;
	for(int i = 0; i < data_len - 1; i ++){
		mm = 0;
		for(int m = 0; m < 8; m ++){
			if((i + m -4) > 0 && (i + m -4) < data_len - 1){
				mm = diff[i + m - 4] + mm;
			}
		}
		mm = mm / 8;
		ma[i] = mm;
	}
	
}