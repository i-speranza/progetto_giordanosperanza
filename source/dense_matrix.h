#ifndef DENSE_MATRIX_H_INCLUDED
#define DENSE_MATRIX_H_INCLUDED


#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

class sparse_matrix;

class dense_matrix{
    unsigned int row,col;
    char *p;

    private:
    void insert_cars(sparse_matrix &SM, char color_car);
    inline bool is_busy(const unsigned int x, const unsigned int y){return (p[index(x,y)]!='0');};
    void blu_car_advance(const unsigned int x, const unsigned int y);
    void red_car_advance(const unsigned int x, const unsigned int y);

    public:
    //CONSTRUCTOR
    dense_matrix(unsigned int row, unsigned int col, sparse_matrix &red, sparse_matrix &blu);
    //DESTRUCTOR
    ~dense_matrix(){};

    inline unsigned int index(const unsigned int x, const unsigned int y){return y+col*x;};
    inline void destroy(){delete [] p;};

    bool move_blu_cars();
    bool move_red_cars();

    void print_screen();
    void print_file(unsigned int n);
};



dense_matrix::dense_matrix(unsigned int row, unsigned int col, sparse_matrix &red, sparse_matrix &blu):row(row),col(col){
    p=new char [row*col];
    const char color_blu='1';
    const char color_red='2';
    insert_cars(blu, color_blu);
    insert_cars(red, color_red);
    for (int i=0; i<row*col; i++){
        if(p[i]!=color_blu && p[i]!=color_red){
            p[i]='0';
        }
    }
}

void dense_matrix::insert_cars(sparse_matrix &SM, char color_car){
    unsigned int coord;
    const char blu='1';
    const char red='2';
    for(int i=0; i<SM.get_size(); i++){
        for(int j=0; j<SM.get_vect_length(i); j++){
            coord=SM.get_value(i,j);
            if(color_car==blu){
                p[index(coord-1,i)]=blu;
            }
            else{
                p[index(i,coord-1)]=red;
            }
        }
        SM.clear_vect(i);
    }
}

void dense_matrix::blu_car_advance(const unsigned int x, const unsigned int y){
    const char blu='1';

    p[index(x,y)]='0'; //libero la attuale casella
    if(x!= row-1){
        p[index(x+1,y)]=blu;
    }
    else{
        p[index(0,y)]=blu;
    }

}

void dense_matrix::red_car_advance(const unsigned int x, const unsigned int y){
    const char red='2';

    p[index(x,y)]='0'; //libero la attuale casella
    if(y!= col-1){
        p[index(x,y+1)]=red;
    }
    else{
        p[index(x,0)]=red;
    }

}
bool dense_matrix::move_blu_cars(){
    const char blu='1';

    if(row>1){
        bool first_car_moved;
        bool cars_advanced;
        unsigned int exit_value;

        for(int j=0; j<col; ++j){
            first_car_moved=false; //gestiscono inizio e fine ciclo for modificati solo se c'è macchina blu che si muove in prima pos
            //gestisco primo elemento. mi interessa se si è spostato
            if(p[index(0,j)]==blu && !is_busy(1,j)){
                    blu_car_advance(0,j);
                    cars_advanced=true;
                    first_car_moved=true;
            }

            //gestisco elementi
            for(int i=1+first_car_moved; i<row-1; ++i){ //mi fermo sempre alla penultima
                if(p[index(i,j)]==blu && !is_busy((i+1),j)){ //mi fermo solo se trovo macchina blu
                        blu_car_advance(i,j);
                        cars_advanced=true;
                        ++i; //se muovo una macchina nella riga in cui l'ho mossa c'era uno zero e quindi non devo più controllare quella riga
                }
                exit_value=i;
            }
            //controllo macchina in ultima riga solo se la prima e la penultima non si sono mosse
            if(!first_car_moved && exit_value==row-2 && p[index(row-1,j)]==blu && !is_busy(0,j)){
                blu_car_advance(row-1,j);
                cars_advanced=true;
            }
        }
        return cars_advanced;
    }
    return false;
}

bool dense_matrix::move_red_cars(){
    const char red='2';

    if(col>1){

        bool first_car_moved;
        bool cars_advanced;
        unsigned int exit_value;

        for(int i=0; i<row; ++i){
            first_car_moved=false;

            //gestisco primo elemento. mi interessa se si è spostato
            if(p[index(i,0)]==red && !is_busy(i,1)){
                    red_car_advance(i,0);
                    first_car_moved=true;
                    cars_advanced=true;
            }
            //gestisco elementi


            for(int j=1+first_car_moved; j<col-1; ++j){
                if(p[index(i,j)]==red && !is_busy(i,j+1)){ //mi fermo solo se trovo macchina rossa
                    red_car_advance(i,j);
                    cars_advanced=true;
                    ++j;
                }
                exit_value=j;
            }
            //controllo macchina in ultima riga solo se la prima e la penultima non si sono mosse
            if(!first_car_moved && exit_value==col-2 && p[index(i,col-1)==red && !is_busy(i,0)]){
                red_car_advance(i,col-1);
                cars_advanced=true;
            }
        }
        return cars_advanced;
    }
    return false;
}

void dense_matrix::print_screen(){
    for(int i=0; i<row; i++){
        for(int j=0; j<col-1; j++){
            std::cout<<p[index(i,j)]<<',';
        }
        std::cout<<p[index(i,col-1)]<<std::endl;
    }
}

void dense_matrix::print_file(unsigned int n){
    std::ostringstream filename;
    filename<<n<<".csv";

    std::ofstream output(filename.str().c_str());
    for(int i=0; i<row; i++){
        for(int j=0; j<col-1; j++){
            output<<p[index(i,j)]<<',';
        }
        output<<p[index(i,col-1)]<<std::endl;
    }
    output.close();
}
#endif
