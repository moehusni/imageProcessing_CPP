
#include <iostream>
#include <fstream>
using namespace std;

class CornerPreseve{
private:
    int numRows, numCols, minVal, maxVal;
    int newMin, newMax;
    int** mirrorFramedAry;
    int** tempAry;
    int neighborARY[9];
    double neighborAVG[8];
public:
    CornerPreseve(int row, int col, int min, int max){
        numRows = row;
        numCols = col;
        minVal = min;
        maxVal = max;
        newMax = -999999;
        newMin =  999999;
        mirrorFramedAry = new int*[row+4];
        tempAry = new int*[row+4];
        for(int i=0; i< row+4; i++){
            mirrorFramedAry[i] = new int[col+4];
            tempAry[i] = new int[col+4];
        }
    }
    
    void mirrorFramed(){
        
        for(int i =0; i < numRows+4; i++){
            mirrorFramedAry[i][0] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][1] = mirrorFramedAry[i][2];
            mirrorFramedAry[i][numCols+2] = mirrorFramedAry[i][numCols+1];
            mirrorFramedAry[i][numCols+3] = mirrorFramedAry[i][numCols+1];
        }
        
        for(int j=0; j < numCols+4; j++){
            mirrorFramedAry[0][j] = mirrorFramedAry[2][j];
            mirrorFramedAry[1][j] = mirrorFramedAry[2][j];
            mirrorFramedAry[numRows+2][j] = mirrorFramedAry[numRows+1][j];
            mirrorFramedAry[numRows+3][j] = mirrorFramedAry[numRows+1][j];
        }
    }
    
    void loadImage(ifstream &input){
        for(int i = 2;i< numRows+2;i++)
            for(int j = 2;j< numCols+2; j++)
                input>>mirrorFramedAry[i][j];
    }
    
    void loadNeighbors(int which, int i, int j){
        int n=0;
        switch(which){
            case 0:
                for(int r=-2; r<=0; r++)
                    for (int c=-2; c<=0; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
            
            case 1:
                for(int r=-2; r<=0; r++)
                    for(int c=0; c<=2; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
                
            case 2:
                for(int r=0; r<=2; r++)
                    for(int c=-2; c<=0; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
                
            case 3:
                for(int r=0; r<=2; r++)
                    for(int c=0; c<=2; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
                
            case 4:
                for(int r=-2; r<=2; r++)
                    for(int c=-2; c<=r && c <= -r; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
            
            case 5:
                for(int r=-2; r<=2; r++)
                    for(int c=2; c >= -r && c >= r; c--)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
                
            case 6:
                for(int r=-2; r<=0; r++)
                    for(int c=r; c <= -r; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
                
            case 7:
                for(int r=0; r<=2; r++)
                    for(int c=-r; c<= r; c++)
                        neighborARY[n++] = mirrorFramedAry[i+r][j+c];
                break;
        }
    }
    
    double computeAVG(){
        double sum =0;
        for(int i =0; i< 9; i++)
            sum += neighborARY[i];
        return sum/9;
    }
    
    int minAVG(){
        int min = neighborAVG[0];
        for(int i=1 ; i< 8; i++)
            if(neighborAVG[i] < min)
                min = neighborAVG[i];
        
        return min;
    }
    
    void findNewMinMax(){
        newMax = -1;
        newMin = 9999;
        for(int i= 2; i< numRows+2;i++){
            for (int j=2; j< numCols+2; j++) {
                if(tempAry[i][j] > newMax) newMax = tempAry[i][j];
                if(tempAry[i][j] < newMin) newMin = tempAry[i][j];
            }
        }
    }
    
    void outPutImage(ofstream &output){
        output<<numRows<<" "<<numCols<<" "<<newMin<<" "<<newMax<<endl;
        for(int i=2; i<numRows+2; i++){
            for(int j=2; j<numCols+2; j++){
                output<<tempAry[i][j]<<" ";
            }
            output<<endl;
        }
    }
    
    void processPixels(){
        for(int i=2;i<numRows+2;i++){
            for(int j=2; j< numCols+2; j++){
                int which = 0; // step 4: which <- 0 // the first group
                while(which < 8){
                    loadNeighbors(which,i,j);
                    neighborAVG[which] = computeAVG(); // step 5
                    which++; // step 6: next group
                }// step 7: repeat step 5 while which < 8
                tempAry[i][j] = minAVG();// step 8
            }
        }// step 9: repeat step 3 to step 8 until all pixels are processed
    }
};

int main(int argc, const char * argv[]) {
    // step 0: open input file and output file
    ifstream input(argv[1]);
    ofstream output(argv[2]);
    if(input.is_open() && output.is_open()){
        // read the image header
        int row,col,min,max;
        input>>row;
        input>>col;
        input>>min;
        input>>max;
        CornerPreseve filter(row,col,min,max);
        // step 1: load image onto mirrorFrameAry
        filter.loadImage(input);
        // step 2: mirrorFrame the mirrorFrameAry
        filter.mirrorFramed();
        // step 3 - 9: process all the pixels,
        filter.processPixels();
        // step 10: findNewMinMax()
        filter.findNewMinMax();
        // step 11: outPutImage with new min and max
        filter.outPutImage(output);
    
    }else{
        cout<<"cannnot open files"<<endl;
    }
    
    // step 12: close all files
    input.close();
    output.close();
    return 0;
}
