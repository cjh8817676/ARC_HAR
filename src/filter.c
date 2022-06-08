#include "stdio"
#define data_len 300
int main(){

	float raw_data[data_len];
	float new_data[data_len];
	float coef[16]=[0.00885425387835770,0.0137493826554715,0.0274888535961056,0.0483032392489433,0.0728372897377215,0.0967921500066679,0.115791366464563,0.126283193657644,0.126283193657644,0.115791366464563,0.0967921500066679,0.0728372897377215,0.0483032392489433,0.0274888535961056,0.0137493826554715,0.00885425387835770];
	for(int i = 0; i < data_len; i ++){
		filter = 0;
		for(int m = 0; m < 16; m ++){
			if((i + m -8) > 0 && (i + m -8) < data_len){
				filter = raw_data[i + m - 8] * coef[m] + filter;
			}
		}
		filter = filter / 16;
		new_data[i] = filter;
	}
}