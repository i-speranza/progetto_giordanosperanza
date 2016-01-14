#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <vector>
#include <cmath>

#include "timing.h"
#include "sparse_matrix.h"
#include "dense_matrix.h"
//#include "read_file.h"

using namespace std;

unsigned int read_file(vector<unsigned int> &t, sparse_matrix &red, sparse_matrix &blu){
    ifstream fin("enorme.csv");
    if(fin.is_open()){
        unsigned int num_cars=0; //this variable will count the number of cars in the matrix

        //gestisco prima riga in cui sono elencati gli step in cui stampare su file
        unsigned int time;
        string stringa;
        getline(fin,stringa); //importa prima riga coi tempi in una stringa
        stringstream ss(stringa); //trasforma str in uno stream
        while(ss>>time){
            t.push_back(time);
            if(ss.peek()==','){
                ss.ignore();
            }
        }

        char *pch;

        unsigned int row=1;
        unsigned int col_old;
        unsigned int col_new=0;

        //leggo prima riga della matrice a parte per inizializzare col_new
        getline(fin,stringa);

        char *str= new char[stringa.size()+1];
        copy(stringa.begin(), stringa.end(), str);
        str[stringa.size()]='\0';
        pch= strtok(str,",");

        while(pch!=NULL){
            ++col_new;
            if(pch[1]!='\0'){
                cout<<"Error: invalid character ("<< pch << ") detected in row "<<row<<endl;
                return 0;
            }
            if(*pch=='1'){
                blu.set_matrix(col_new,row);
                num_cars++;
            }
            if(*pch=='2'){
                red.set_matrix(row,col_new);
                num_cars++;
            }
            pch = strtok(NULL,",");
        }
        //leggo il resto del file
        while(getline(fin, stringa)){
            str= &stringa[0];
            ++row;
            col_old=col_new;
            col_new=0;
            pch = strtok (str, ",");

            while(pch!=NULL){
                ++col_new;
                if(pch[1]!='\0'){
                    cout<<"Error: invalid character ("<< pch << ") detected in row "<<row<<endl;
                    return 0;
                }
                if(*pch=='1'){
                    blu.set_matrix(col_new,row);
                    num_cars++;
                }
                if(*pch=='2'){
                    red.set_matrix(row,col_new);
                    num_cars++;
                }
                pch= strtok(NULL,",");
            }
            if(col_old!=col_new){
                cout<<"Error: row "<<row<<" has different length"<<endl;
                return 0;
            }
        }

        blu.set_dimension(col_new);
        red.set_dimension(row);
        fin.close();
        if(num_cars==0){
            cout<<"No cars in the matrix."<<endl;
        }
        return num_cars;
    }
    else{
        cout << "error: I can't read the file." << endl;
        return 0;
    }
}

void print_file_sparse(sparse_matrix &red, sparse_matrix &blu, const unsigned int &n, const unsigned int &row, const unsigned int &col){
    std::ostringstream filename;
    filename<<n<<".csv";

    std::ofstream output(filename.str().c_str());

    if(output.is_open()){
        for (int i=0; i<row; i++){
            for(int j=0; j<col-1; j++){
                if(red.busy(j,i)){
                    output<<'2'<<',';
                }
                else{
                    if(blu.busy(i,j)){
                        output<<'1'<<',';
                    }
                    else{
                        output<<'0'<<',';
                    }
                }
            }
            if(red.busy(col-1,i)){
                output<<'2'<<std::endl;
            }
            else{
                if(blu.busy(i,col-1)){
                    output<<'1'<<std::endl;
                }
                else{
                    output<<'0'<<std::endl;
                }
            }
        }
        output.close();
    }
    else{
        cout<<"unable to open the file"<<endl;
    }
}

void sparse_implementation(sparse_matrix &blu, sparse_matrix &red, vector<unsigned int> &print_steps, const unsigned int rows, const unsigned int cols){
    vector<unsigned int>::iterator it_time=print_steps.begin();
    vector<unsigned int>::iterator it_end_time=print_steps.end();

    unsigned int n_steps=0;
    unsigned int blu_moved, red_moved; //tengono traccia di eventuale ingorgo e bloccano ciclo

    while (it_time!=it_end_time){
        n_steps++;
        blu_moved=blu.move_cars(red);
        if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
            print_file_sparse(red,blu,n_steps,rows,cols);
            it_time++;
        }
        if(it_time!=it_end_time){   //se non faccio questo controllo, dopo chiederei di accedere a *it_time che potrebbe non esistere
            n_steps++;
            red_moved=red.move_cars(blu);
            if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
                print_file_sparse(red,blu,n_steps,rows,cols);
                it_time++;
            }
        }
        if(blu_moved==0 && red_moved==0){
            cout<<"Neither blu nor red cars moved. Iteration number: "<<n_steps<<endl<<"Final situation in file step_"<<n_steps<<".csv"<<endl;
            print_file_sparse(red,blu,n_steps,rows,cols);
            break;
        }
    }
}

void dense_implementation(dense_matrix &M, vector<unsigned int> &print_steps, const unsigned int rows, const unsigned int cols){
    vector<unsigned int>::iterator it_time=print_steps.begin();
    vector<unsigned int>::iterator it_end_time=print_steps.end();

    unsigned int n_steps=0;
    bool blu_moved, red_moved;

    while (it_time!=it_end_time){
        n_steps++;
        blu_moved=M.move_blu_cars();
        if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
            M.print_file(n_steps);
            it_time++;
        }
        if(it_time!=it_end_time){   //se non faccio questo controllo, dopo chiederei di accedere a *it_time che potrebbe non esistere
            n_steps++;
            red_moved=M.move_red_cars();
            if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
                M.print_file(n_steps);
                it_time++;
            }
        }
        if(blu_moved==0 && red_moved==0){
            cout<<"Neither blu nor red cars moved. Iteration number: "<<n_steps<<endl<<"Final situation in file "<<n_steps<<".csv"<<endl;
            M.print_file(n_steps);
            break;
        }
    }
    M.destroy();
}

void dense_implementation_one_line(dense_matrix &M, vector<unsigned int> &print_steps, const unsigned int n, bool one_row){
    vector<unsigned int>::iterator it_time=print_steps.begin();
    vector<unsigned int>::iterator it_end_time=print_steps.end();

    unsigned int n_steps=0;
    bool car_moved=false;

    if(one_row){
        while (it_time!=it_end_time){
            n_steps++;
            if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
                M.print_file(n_steps);
                it_time++;
            }
            if(it_time!=it_end_time){   //se non faccio questo controllo, dopo chiederei di accedere a *it_time che potrebbe non esistere
                n_steps++;
                car_moved=M.move_red_cars();
                if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
                    M.print_file(n_steps);
                    it_time++;
                }
            }
            if(car_moved==0){
                cout<<"Only red cars could move and now they are all blocked. Iteration number: "<<n_steps<<endl<<"Final situation in file "<<n_steps<<".csv"<<endl;
                M.print_file(n_steps);
                break;
            }
        }
        M.destroy();
    }
    else{
        while (it_time!=it_end_time){
            n_steps++;

            car_moved=M.move_blu_cars();

            if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
                M.print_file(n_steps);
                it_time++;
            }
            if(it_time!=it_end_time){   //se non faccio questo controllo, dopo chiederei di accedere a *it_time che potrebbe non esistere
                n_steps++;
                if(n_steps == *it_time){ //in generale ci sarà scritto stampa_matrice complessiva
                    M.print_file(n_steps);
                    it_time++;
                }
            }
            if(car_moved==0){
                cout<<"Only blu cars could move and now they are all blocked. Iteration number: "<<n_steps<<endl<<"Final situation in file "<<n_steps<<".csv"<<endl;
                M.print_file(n_steps);
                break;
            }
        }
        M.destroy();
    }
}


int main(){
    timer Timer;
    Timer.start();
    sparse_matrix blu,red;
    vector<unsigned int> print_steps;
    unsigned int num_cars;

    const float sparsity_param=0.06;

    num_cars=read_file(print_steps, red, blu);
    if(num_cars==0){
        return 0;
    }

    const unsigned int rows=red.get_dim();
    const unsigned int cols=blu.get_dim();

    cout<<"cars in the matrix: "<<num_cars<<endl;
    cout<<"File dimensions"<<endl<<"rows: "<<rows<<", columns: "<<cols<<endl;

    float sparsity_limit=sparsity_param*rows*cols;

    if(num_cars<sparsity_limit){
        sparse_implementation(blu, red, print_steps, rows, cols);
    }
    //IMPLEMENTAZIONE DENSA
    else{
        dense_matrix M(rows, cols,red,blu); //construisco matrice densa
        if(rows==1){
            dense_implementation_one_line(M,print_steps,cols,true);      //true è il valo passato a una bool che verifica se è una sola riga o una sola colonna
        }
        else if(cols==1){
            dense_implementation_one_line(M,print_steps,rows,false);
        }
        else{
            dense_implementation(M,print_steps,rows,cols);
        }
    }

    Timer.stop();

return 0;
}
