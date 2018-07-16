
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]){
    ofstream file;

    file.open(argv[1]);

    // Model
    file << 0.5 << " " << 1.5 << endl;
    // CellularSpace
    file << 50 << " " << 100 << endl;
    // Attribute
    file << 1 << " " << 1 << endl;
    // Cell
    srand(time(NULL));
    for(int i = 0; i < atoi(argv[2]); i++){
        int i_cell = rand() % 17;
        switch (i_cell){
            case 0: file << 0 << " " << 0 << " " << 0.1 << endl; break;
            case 1: file << 0 << " " << 99 << " " << 0.1 << endl; break;
            case 2: file << 30 << " " << 0 << " " << 0.1 << endl; break;
            case 3: file << 30 << " " << 50 << " " << 0.1 << endl; break;
            case 4: file << 30 << " " << 99 << " " << 0.1 << endl; break;
            case 5: file << 49 << " " << 0 << " " << 0.1 << endl; break;
            case 6: file << 49 << " " << 50 << " " << 0.1 << endl; break;
            case 7: file << 49 << " " << 99 << " " << 0.1 << endl; break;
            case 8: file << 70 << " " << 0 << " " << 0.1 << endl; break;
            case 9: file << 70 << " " << 50 << " " << 0.1 << endl; break;
            case 10: file << 70 << " " << 99 << " " << 0.1 << endl; break;
            case 11: file << 99 << " " << 0 << " " << 0.1 << endl; break;
            case 12: file << 99 << " " << 50 << " " << 0.1 << endl; break;
            case 13: file << 99 << " " << 99 << " " << 0.1 << endl; break;
            case 14: file << 120 << " " << 0 << " " << 0.1 << endl; break;
            case 15: file << 120 << " " << 50 << " " << 0.1 << endl; break;
            case 16: file << 120 << " " << 99 << " " << 0.1 << endl; break;
        }
    }

    file.close();
}
