#include "triangulation.h"



/** beacon localization : call the total algo 4 times with 3 diff
    input :
        Beacon* beacon      : list of 3 beacons
        float*  alpha_rad   : list of angle in rad to the beacon in the same order of the beacon
                                in the fixed reference frame (origin in middle of map)
        bool*   alpha_bool  : list of boolean to express the fact that the angle is useful for the algo
    output :
        float*  new_pos     : list of the estimated position x, y, angle_rad
*/
void beaconLocaliseWith3_caller(Beacon* beacon, float* alpha_rad, bool* alpha_bool, float* new_pos){
    int i=0, nb_cas=4, nb_case_working=0, index0, index1, index2;
    Beacon temp_beac[3];
    float temp_alpha_rad[3], temp_new_pos[3];
    new_pos[0]=0;
    new_pos[1]=0;

    cout << "total algo" << endl;
    for(i=0;i<nb_cas;i++){
        index0=i%nb_cas;
        index1=(i+1)%nb_cas;
        index2=(i+2)%nb_cas;
        if(alpha_bool[index0] && alpha_bool[index1] && alpha_bool[index2]){
            nb_case_working++;
            temp_beac[0]=beacon[index0];
            temp_beac[1]=beacon[index1];
            temp_beac[2]=beacon[index2];
            temp_alpha_rad[0]=alpha_rad[index0];
            temp_alpha_rad[1]=alpha_rad[index1];
            temp_alpha_rad[2]=alpha_rad[index2];
            beaconLocaliseWith3_totalAlgo(temp_beac, temp_alpha_rad, temp_new_pos);
            new_pos[0]+=temp_new_pos[0];
            new_pos[1]+=temp_new_pos[1];
        }
    }
    new_pos[0]=new_pos[0]/nb_case_working;
    new_pos[1]=new_pos[1]/nb_case_working;
    return;
}

/** beacon localization using the total algorithm http://www.telecom.ulg.ac.be/triangulation/
    input :
        Beacon* beacon      : list of 3 beacons
        float*  alpha_rad   : list of angle in rad to the beacon in the same order of the beacon
                                in the fixed reference frame (origin in middle of map)
    output :
        float* new_pos      : list of the estimated position x, y, angle_rad
*/
void beaconLocaliseWith3_totalAlgo(Beacon* beacon, float* alpha_rad, float* new_pos){
    float alpha_1 = alpha_rad[0];
    float alpha_2 = alpha_rad[1];
    float alpha_3 = alpha_rad[2];

    float x_beac_1 = beacon[0].getPosx();
    float x_beac_2 = beacon[1].getPosx();
    float x_beac_3 = beacon[2].getPosx();
    float y_beac_1 = beacon[0].getPosy();
    float y_beac_2 = beacon[1].getPosy();
    float y_beac_3 = beacon[2].getPosy();

    //compute the modified beacon coordinates
	float x1_p = x_beac_1 - x_beac_2;
	float y1_p = y_beac_1 - y_beac_2;
	float x3_p = x_beac_3 - x_beac_2;
	float y3_p = y_beac_3 - y_beac_2;

	//compute the three cot();
	float T12 = 1 / (tan(alpha_2 - alpha_1));
	float T23 = 1 / (tan(alpha_3 - alpha_2));
	float T31 = (1 - T12*T23) / (T12 + T23);

	//compute the modified circle center coordinates
	float x12_p = x1_p + T12*y1_p, y12_p = y1_p - T12*x1_p;
	float x23_p = x3_p - T23*y3_p, y23_p = y3_p + T23*x3_p;
	float x31_p = x3_p + x1_p + T31*(y3_p - y1_p);
	float y31_p = y3_p + y1_p - T31*(x3_p - x1_p);

	//compute k31_p
	float k31_p = x1_p*x3_p + y1_p*y3_p + T31*(x1_p*y3_p - x3_p*y1_p);

	//compute D
	float D = (x12_p - x23_p)*(y23_p - y31_p) - (y12_p - y23_p)*(x23_p - x31_p);
	if (D == 0)
	{
		printf("ERROR D=0");
		return;
	}

	//compute the robot position
	new_pos[0] = (x_beac_2 + k31_p*(y12_p - y23_p) / D);
	new_pos[1] = (y_beac_2 + k31_p*(x23_p - x12_p) / D);
	return ;
}

/** filtering using a forgetting factor vector based on lambda
    input :
        vector<float>   data        : the data vector (1D) to filter (coded for any length of data)
        float           lambda      : the forgetting factor (0.9-0.99)
        int             nb_to_keep  : the maximum number of previous data to take into account (5-10)
    output :
        float*          filtered    : the current filtered value of the data
*/
void filter_forgetting(vector<float> data, float lambda, int nb_to_keep, float* filtered){

    // get the length of the data
    int length_data=data.size();

    // get the number of value to use
    int length_lambda = min(nb_to_keep, length_data);

    // initialize
    *filtered = 0;
    float lambda_vec[length_lambda], sum=0;

    // compute the forgetting vector [lambda^1, lambda^2 ... lambda^length_lambda] and the normalizing factor
    for (int i=0;i<length_lambda;i++){
        lambda_vec[i]=pow(lambda,i+1);
        sum+=lambda_vec[i];
    }

    // compute the filtered value : most recent data * lambda_vec / sum
    for (int i=0;i<length_lambda;i++){
        (*filtered)+=data.at(length_data-i-1)*lambda_vec[i];
    }
    (*filtered)=(*filtered)/sum;

    return;
}
