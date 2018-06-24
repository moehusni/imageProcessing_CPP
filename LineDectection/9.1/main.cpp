#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int main(int argc, const char * argv[]) {
    ifstream inFile(argv[1]);
    ofstream outFile(argv[2]);
    int r,c,min,max;
    int count = 0;
    inFile>>r>>c>>min>>max;
    int arr[r][c];
    for (int i = 0; i < r; i++)
        for(int j = 0; j < c; j++){
            inFile>>arr[i][j];
            if(arr[i][j] > 0) count++;
        }
    
    outFile << count << endl;
    outFile << r << " " << c << endl;
    for (int i = 0; i < r; i++)
        for(int j = 0; j < c; j++){
            if(arr[i][j] > 0)
                outFile << i << " " << j << endl;
        }
    
    inFile.close();
    outFile.close();
    return 0;
}
