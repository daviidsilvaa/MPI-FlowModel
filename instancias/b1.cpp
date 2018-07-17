
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
    file << 40 << " " << 20 << endl;
    // Attribute
    file << 1 << " " << 1 << endl;
    // Cell
    srand(time(NULL));
    for(int i = 0; i < atoi(argv[2]); i++){
        int i_cell = rand() % 48;
        switch (i_cell){
            case 0: file << 0 << " " << 0 << " " << 0.1 << endl; break;
            case 1: file << 0 << " " << 9 << " " << 0.1 << endl; break;
            case 2: file << 4 << " " << 0 << " " << 0.1 << endl; break;
            case 4: file << 4 << " " << 5 << " " << 0.1 << endl; break;
            case 5: file << 4 << " " << 9 << " " << 0.1 << endl; break;
            case 6: file << 9 << " " << 0 << " " << 0.1 << endl; break;
            case 7: file << 9 << " " << 5 << " " << 0.1 << endl; break;
            case 8: file << 9 << " " << 9 << " " << 0.1 << endl; break;
            case 9: file << 12 << " " << 0 << " " << 0.1 << endl; break;
            case 10: file << 12 << " " << 5 << " " << 0.1 << endl; break;
            case 11: file << 12 << " " << 9 << " " << 0.1 << endl; break;
            case 12: file << 19 << " " << 0 << " " << 0.1 << endl; break;
            case 13: file << 19 << " " << 5 << " " << 0.1 << endl; break;
            case 14: file << 19 << " " << 9 << " " << 0.1 << endl; break;
            case 15: file << 24 << " " << 0 << " " << 0.1 << endl; break;
            // case 16: file << 24 << " " << 5 << " " << 0.1 << endl; break;
            case 17: file << 24 << " " << 9 << " " << 0.1 << endl; break;
            case 18: file << 29 << " " << 0 << " " << 0.1 << endl; break;
            case 19: file << 29 << " " << 5 << " " << 0.1 << endl; break;
            case 20: file << 29 << " " << 9 << " " << 0.1 << endl; break;
            // case 21: file << 34 << " " << 0 << " " << 0.1 << endl; break;
            case 22: file << 34 << " " << 5 << " " << 0.1 << endl; break;
            case 23: file << 34 << " " << 9 << " " << 0.1 << endl; break;
            case 24: file << 39 << " " << 0 << " " << 0.1 << endl; break;
            case 25: file << 39 << " " << 5 << " " << 0.1 << endl; break;
            case 26: file << 39 << " " << 9 << " " << 0.1 << endl; break;
            // case 27: file << 44 << " " << 0 << " " << 0.1 << endl; break;
            // case 28: file << 44 << " " << 5 << " " << 0.1 << endl; break;
            // case 29: file << 44 << " " << 9 << " " << 0.1 << endl; break;
            // case 30: file << 49 << " " << 0 << " " << 0.1 << endl; break;
            // case 31: file << 49 << " " << 5 << " " << 0.1 << endl; break;
            case 32: file << 49 << " " << 9 << " " << 0.1 << endl; break;
            case 33: file << 54 << " " << 0 << " " << 0.1 << endl; break;
            case 34: file << 54 << " " << 5 << " " << 0.1 << endl; break;
            case 35: file << 54 << " " << 9 << " " << 0.1 << endl; break;
            case 36: file << 59 << " " << 0 << " " << 0.1 << endl; break;
            case 37: file << 59 << " " << 5 << " " << 0.1 << endl; break;
            case 38: file << 59 << " " << 9 << " " << 0.1 << endl; break;
            // case 39: file << 64 << " " << 0 << " " << 0.1 << endl; break;
            // case 40: file << 64 << " " << 5 << " " << 0.1 << endl; break;
            // case 41: file << 64 << " " << 9 << " " << 0.1 << endl; break;
            // case 42: file << 69 << " " << 0 << " " << 0.1 << endl; break;
            // case 43: file << 69 << " " << 5 << " " << 0.1 << endl; break;
            case 44: file << 69 << " " << 9 << " " << 0.1 << endl; break;
            // case 45: file << 74 << " " << 0 << " " << 0.1 << endl; break;
            case 46: file << 74 << " " << 5 << " " << 0.1 << endl; break;
            case 47: file << 74 << " " << 9 << " " << 0.1 << endl; break;
            default: break;
        }
    }

    file.close();
}
