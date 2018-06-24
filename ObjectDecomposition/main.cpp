#include <iostream>
#include <fstream>
using namespace std;

class Expansion {
private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
public:
    bool changeFlag;
    int cycleCount;
    int** objectAry;
    int** firstAry;
    int** secondAry;
    
    Expansion(int r, int c, int min, int max) : numRows(r),numCols(c),minVal(min),maxVal(max){
        changeFlag = true;
        firstAry = new int*[numRows+2];
        secondAry = new int*[numRows+2];
        objectAry = new int*[numRows+2];
        for(int i = 0; i < numRows + 2; i++){
            firstAry[i] = new int[numCols+2];
            secondAry[i] = new int[numCols+2];
            objectAry[i] = new int[numCols+2];
            this->cycleCount = 0;

        }
    }
    
    void zeroFramed(int** arr){
        for (int i = 0; i < numRows + 2; i++) {
            arr[i][0] = 0;
            arr[i][numRows + 1] = 0;
        }
        
        for(int j = 0; j < numCols + 2; j++){
            arr[0][j] = 0;
            arr[numRows + 1][j] = 0;
        }
    }
    
    void loadImage(ifstream& inFile, int** arr){ // load objectAry && first Ary
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                inFile>>arr[i][j];
    }
    
    void copyAry(int** ary1, int** ary2){
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                ary1[i][j] = ary2[i][j];
    }
    
    void DoExpansion(int r, int c, int label){
        if(objectAry[r][c] > 0){
            secondAry[r][c] = label;
            changeFlag = true;
        }
    }
    
    void NorthExpansion(){
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                if(firstAry[i][j] > 0 && firstAry[i-1][j] == 0)
                    DoExpansion(i-1, j, firstAry[i][j]);
    }
    
    void SorthExpansion(){
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                if(firstAry[i][j] > 0 && firstAry[i+1][j] == 0)
                    DoExpansion(i+1, j, firstAry[i][j]);
    }
    void WestExpansion(){
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                if(firstAry[i][j] > 0 && firstAry[i][j-1] == 0)
                    DoExpansion(i, j-1, firstAry[i][j]);
    }
    void EastExpansion(){
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                if(firstAry[i][j] > 0 && firstAry[i][j+1] == 0)
                    DoExpansion(i, j+1, firstAry[i][j]);
    }
    
    void prettyPrint(ofstream& outFile2, int** arr){
        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j <= numCols; j++)
                if(arr[i][j] > 0) outFile2<<arr[i][j]<<" ";
                else outFile2<<"  ";
            
            outFile2<<endl;
        }
    }
    
    void finalImage(ofstream& outFile1){
        outFile1<<numRows<<" "<<numCols<<" "<<minVal<<" "<<maxVal<<endl;
        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j <= numCols; j++)
                outFile1<<firstAry[i][j]<<" ";
            outFile1<<endl;
        }
    }
    
};



int main(int argc, const char * argv[]) {
    
    ifstream inFile1(argv[1]);
    ifstream inFile2(argv[2]);
    ofstream outFile1(argv[3]);
    ofstream outFile2(argv[4]);
    
    // step 0
    int r, c, min,max;
    inFile1>>r>>c>>min>>max;
    inFile2>>r>>c>>min>>max; // update min & max
    Expansion ex(r,c,min,max);
    
    // step 1
    ex.zeroFramed(ex.objectAry);
    ex.zeroFramed(ex.firstAry);
    ex.zeroFramed(ex.secondAry);

    // step 2
    ex.loadImage(inFile1, ex.objectAry);
    ex.loadImage(inFile2, ex.firstAry);
    ex.copyAry(ex.secondAry, ex.firstAry);
    // step 3
    ex.cycleCount = 0;
    outFile2<<"Pretty print Object Image:" <<endl;
    ex.prettyPrint(outFile2,ex.objectAry);
    outFile2<<endl;
    
    while(ex.changeFlag){
        // step 4
        if(ex.cycleCount == 0 || ex.cycleCount == 3 || ex.cycleCount == 5){
            outFile2<<"Expanding in " << ex.cycleCount <<" th cycle"<<endl;
            ex.prettyPrint(outFile2, ex.firstAry); // prettyPrint firstAry to Output-2
            outFile2<<endl;
        }
    
        // step 5
        ex.changeFlag = false;
        ex.cycleCount++;
        
        // step 6
        ex.NorthExpansion();
        ex.copyAry(ex.firstAry,ex.secondAry);
        
        // step 7
        ex.SorthExpansion();
        ex.copyAry(ex.firstAry,ex.secondAry);
        
        // step 8
        ex.WestExpansion();
        ex.copyAry(ex.firstAry,ex.secondAry);
       
        // step 9
        ex.EastExpansion();
        ex.copyAry(ex.firstAry,ex.secondAry);
        
    }// step 10: repeat step 4 - 9 while changeFlag is true
    
    // step 11
    outFile2<<"Final result:"<<endl;
    ex.prettyPrint(outFile2, ex.firstAry);
    
    // step 12: write image header to Output-1 and copy firstAry from [1][1]
    ex.finalImage(outFile1);
    
    // step 13: close all files
    inFile1.close();
    inFile2.close();
    outFile1.close();
    outFile2.close();
    return 0;
}
