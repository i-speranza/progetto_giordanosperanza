#ifndef SPARSE_MATRIX_H_INCLUDED
#define SPARSE_MATRIX_H_INCLUDED


#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

//using Int = unsigned int;
//using intMat = std::vector<vector<Int> >;


class sparse_matrix{
    std::vector<std::vector<unsigned int> > pos;
    unsigned int dim; //colonne del file per le blu, righe per le rosse

    public:
    //CONSTRUCTORS
    sparse_matrix(): pos(),dim(0){};

    //DISTRUCTOR
    ~sparse_matrix(){};


    inline void set_dimension(unsigned int n){dim=n;};
    inline unsigned int get_dim(){return dim;};         //ritorna il numero di righe/colonne del file
    inline unsigned int get_size(){return pos.size();}; //ritorna numero effettivo di colonne/righe delle due matrici sparse (<= di dimension)
    inline unsigned int get_vect_length(unsigned int i){return pos[i].size();};
    inline unsigned int get_value(unsigned int i, unsigned int j){return pos[i][j];};
    inline void clear_vect(unsigned int i){pos[i].clear();};

    void set_matrix(unsigned int stat, unsigned int dyn);
    void print();
    void car_advance(const unsigned int stat, const unsigned int dyn, const unsigned int value, const unsigned int capolinea);
    bool busy (const unsigned int dyn, const unsigned int stat);
    bool blocked_by_same_color_car (const unsigned int stat, const int position, const unsigned int value, const unsigned int capolinea);
    bool move_cars(sparse_matrix &other);
};

void sparse_matrix::set_matrix(unsigned int stat, unsigned int dyn){
    if(stat>pos.size()){
        pos.resize(stat);
    }
    pos[stat-1].push_back(dyn);
}

void sparse_matrix::print(){
        int i=0;
        for (std::vector<std::vector<unsigned int> >::iterator it = pos.begin(); it != pos.end(); ++it, ++i){
            for(std::vector<unsigned int>::iterator it2 = pos[i].begin(); it2!=pos[i].end(); ++it2){
                std::cout<<*it2<<' ';
            }
            std::cout<<std::endl;
        }
}

void sparse_matrix::car_advance(const unsigned int stat, const unsigned int dyn, const unsigned int value, const unsigned int capolinea){
    unsigned int new_value=value+1;
    if(value==capolinea){
        new_value=1;
    }
    pos[stat][dyn]=new_value;
}

bool sparse_matrix::busy (const unsigned int dyn, const unsigned int stat){
    unsigned int vect_to_look_in=stat;
    if(stat==get_dim()){
        vect_to_look_in=0;
    }
    if(vect_to_look_in<get_size()){
        for(unsigned int i=0; i<get_vect_length(vect_to_look_in); ++i){
            if(pos[vect_to_look_in][i]==dyn+1){
                return true;
            }
        }
    }
return false;
}

bool sparse_matrix::blocked_by_same_color_car (const unsigned int stat, const int position, const unsigned int value, const unsigned int capolinea){
    unsigned int value_to_find=value+1;
    unsigned int position_to_look=position+1;
    if (value==capolinea){
        value_to_find=1;
    }
return pos[stat][position_to_look]==value_to_find;
}

bool sparse_matrix::move_cars(sparse_matrix &other){
        bool car_moved=false; //conta quante macchine si sono mosse. resta 0 se nessuna si è mossa
        bool first_car_advanced;
        const unsigned int capolinea=other.get_dim(); //per le blu mi dice num righe, per rosse numero colonne
        const unsigned int end_loop_value=get_size(); //numero effettivo di righe per le rosse, colonne per le blu
        unsigned int vect_length; //conterrà la lunghezza di ciascun vettore
        unsigned int position; //contiene per le blu la riga in cui si trovano, per le rosse la colonna
        unsigned int last_position; //serve per gestire ultima macchina

        for(unsigned int i=0; i<end_loop_value; i++){ //ciclo su attributo statico
            vect_length=get_vect_length(i);
            first_car_advanced=false;
            if(vect_length>0){ //faccio qualcosa solo se dentro c'è almeno una macchina
                position=pos[i][0];
                if(vect_length==1){//se c'è una sola macchina devo soltanto controllare altra matrice
                    if(!other.busy(i,position)){
                        car_advance(i,0,position,capolinea);
                        car_moved=true;
                    }
                }
                else{ //c'è più di una macchina
                    //gestisco a parte prima macchina
                    if(!blocked_by_same_color_car(i,0,position,capolinea) && !other.busy(i,position)){
                        car_advance(i,0,position,capolinea);
                        car_moved=true;
                        first_car_advanced=true;
                    }
                    //gestisco macchine dalla seconda alla penultima
                    for(unsigned int j=1; j<vect_length-1; ++j){
                        position=pos[i][j];
                        if(!blocked_by_same_color_car(i,j,position,capolinea) && !other.busy(i,position)){
                            car_advance(i,j,position,capolinea);
                            car_moved=true;
                        }
                    }
                    //gestisco ultima macchina
                    position=pos[i][vect_length-1];
                    last_position=position;
                    if(first_car_advanced){ //se la prima macchina si è mossa faccio virtualmente muovere in avanti anche questa per controllare se si può muovere
                        last_position=position%capolinea+1;
                    }
                    if(!blocked_by_same_color_car(i,-1,last_position,capolinea) && !other.busy(i,position)){
                        car_advance(i,vect_length-1,position,capolinea);
                        car_moved=true;
                    }
                }
            }
        }

return car_moved;
}


#endif

