#ifndef TRIANGULATION_H
#define TRIANGULATION_H
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <vector>


using namespace std;
class Beacon{

private:
    int _id;
    int _posx;
    int _posy;
    std::string _color;

public:
    inline Beacon(){}
    inline Beacon(int id, int posx, int posy, std::string color){
        _id=id;
        _posx=posx;
        _posy=posy;
        _color=color;
    }
    inline int getId(){
        return _id;
    }
    inline int getPosx(){
        return _posx;
    }
    inline int getPosy(){
        return _posy;
    }
    inline std::string getUnit(){
        return "beacons position unit is meter";
    }
    inline std::string getColor(){
        string s = "color is " + _color;
        return s;
    }
    inline float distSquareTo(float pos[3]){
        return pos[0]*pos[0]+pos[1]*pos[1];
    }

};

void beaconLocaliseWith3_caller(Beacon* beacon, float* alpha_rad, bool* alpha_bool, float* new_pos);
void beaconLocaliseWith3_totalAlgo(Beacon* beacon, float* alpha_rad, float* new_pos);
void filter_forgetting(std::vector<float> data, float lambda, int nb_to_keep, float* filtered);


#endif //TRIANGULATION_H