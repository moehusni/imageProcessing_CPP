#include <iostream> // std::cout, std::endl
#include <fstream>  // std::ifstream, std::ofstream
#include <iomanip>  // std::stew
#include <cmath>    // std::abs
#include <sstream>  // stringstream
using namespace std;

class Image {
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    
public:
    int** mirrorFramedAry;
    
    Image(int r, int c, int min, int max) : numRows(r), numCols(c), minVal(min), maxVal(max) {
        mirrorFramedAry = new int*[r + 2];
        for(int i = 0; i < r + 2; i++)
            mirrorFramedAry[i] = new int[c + 2];
    }
    
    void loadImage(ifstream &infile){
        for(int i = 1; i <= numRows; i++)
            for(int j = 1; j <= numCols; j++)
                infile>>mirrorFramedAry[i][j];
    }
    
    void mirrorFramed(){
        for(int j = 1; j <= numCols; j++){
            mirrorFramedAry[0][j] = mirrorFramedAry[1][j];
            mirrorFramedAry[numRows+1][j] = mirrorFramedAry[numRows][j];
        }
        
        for (int i = 0; i <= numRows+1; i++) {
            mirrorFramedAry[i][0] = mirrorFramedAry[i][1];
            mirrorFramedAry[i][numCols+1] = mirrorFramedAry[i][numCols];
        }
    }
    
    void prettyPrint(ofstream &output, int** arr){
        for (int i= 1; i <= numRows; i++) {
            for (int j = 1; j <= numCols; j++)
                if(arr[i][j] > 0) output<<setw(4)<<arr[i][j];
                else output<<setw(4)<<" ";
            output<<endl;
        }
    }
};

class Edge {
    int** imgAry;
    const int ROW;
    const int COL;
    int newMin[5] = {99,99,99,99,99};
    int newMax[5] = {0,0,0,0,0};
    
public:
    const int maskVertical[3][3]   = {{1,0,-1},{2,0,-2},{1,0,-1}};
    const int maskHorizontal[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
    const int maskRightDiag[3][3]  = {{2,1,0},{1,0,-1},{0,-1,-2}};
    const int maskLeftDiag[3][3]   = {{0,1,2},{-1,0,1},{-2,-1,0}};
    int** SobelVertical;
    int** SobelHorizontal;
    int** SobelRightDiag;
    int** SobelLeafDiag;
    int** GradiantEdge;

    Edge(int r, int c, int** ary): ROW(r), COL(c){
        SobelVertical   = new int*[r+2];
        SobelHorizontal = new int*[r+2];
        SobelRightDiag  = new int*[r+2];
        SobelLeafDiag   = new int*[r+2];
        GradiantEdge    = new int*[r+2];
        
        for(int i = 0; i < r+2; i++){
            SobelHorizontal[i] = new int[c+2];
            SobelVertical[i]   = new int[c+2];
            SobelRightDiag[i]  = new int[c+2];
            SobelLeafDiag[i]   = new int[c+2];
            GradiantEdge[i]    = new int[c+2];
        }
        imgAry = ary;
    }
    
    void computeNewMinMax(){
        for(int i = 1; i <= ROW; i++)
            for(int j = 1; j <= COL; j++){
                if(SobelVertical[i][j] > newMax[0]) newMax[0] = SobelVertical[i][j];
                else if(SobelVertical[i][j] < newMin[0]) newMin[0]=SobelVertical[i][j];
                if (SobelHorizontal[i][j] > newMax[1]) newMax[1]=SobelHorizontal[i][j];
                else if(SobelHorizontal[i][j]<newMin[1])newMin[1]=SobelHorizontal[i][j];
                if(SobelRightDiag[i][j] > newMax[2]) newMax[2] = SobelRightDiag[i][j];
                else if(SobelRightDiag[i][j]<newMin[2]) newMin[2]=SobelRightDiag[i][j];
                if(SobelLeafDiag[i][j] > newMax[3]) newMax[3] = SobelLeafDiag[i][j];
                else if(SobelLeafDiag[i][j]<newMin[3]) newMin[3]=SobelLeafDiag[i][j];
                if(GradiantEdge[i][j] > newMax[4]) newMax[4] = GradiantEdge[i][j];
                else if(GradiantEdge[i][j] < newMin[4]) newMin[4] = GradiantEdge[i][j];
            }
    }
    
    void convolute(int i, int j, int mask){
        int* arr;
        if(mask < 2) arr = (int*)((mask == 0) ? maskVertical : maskHorizontal);
        else arr = (int*)((mask == 2) ? maskRightDiag : maskLeftDiag);
        
        int sum = 0;
        for(int r = 0; r < 3; r++)
            for(int c = 0; c < 3; c++)
                sum += (imgAry[i+r-1][j+c-1] * *(arr+r*3+c));
        
        sum = abs(sum);
        
        switch (mask) {
            case 0:
                SobelVertical[i][j]  = sum;
                break;
            case 1:
                SobelHorizontal[i][j] = sum;
                break;
            case 2:
                SobelRightDiag[i][j] = sum;
                break;
            case 3:
                SobelLeafDiag[i][j]  = sum;
        }
    }
    
    void computeGradiant(int i ,int j){
        GradiantEdge[i][j] = abs(imgAry[i][j] - imgAry[i+1][j])
                            + abs(imgAry[i][j] - imgAry[i][j+1]);
    }
    
    void print(ofstream &out1, ofstream &out2, ofstream &out3,
               ofstream &out4, ofstream &out5){
        computeNewMinMax();
        stringstream ss[5];
        for(int i = 0; i < 5; i++)
            ss[i] << ROW << " " << COL << " " << newMin[i] << " " << newMax[i] <<endl;
     
        out1 << ss[0].str();
        out2 << ss[1].str();
        out3 << ss[2].str();
        out4 << ss[3].str();
        out5 << ss[4].str();
        
        for(int i = 1; i <= ROW; i++){
            for(int j = 1; j <= COL; j++){
                out1<< setw(4) <<SobelVertical[i][j];
                out2<< setw(4) <<SobelHorizontal[i][j];
                out3<< setw(4) <<SobelRightDiag[i][j];
                out4<< setw(4) <<SobelLeafDiag[i][j];
                out5<< setw(4) <<GradiantEdge[i][j];
            }
            out1<<endl;
            out2<<endl;
            out3<<endl;
            out4<<endl;
            out5<<endl;
        }
    }
};

int main(int argc, const char * argv[]) {
    // step 0: open image
    ifstream inFile(argv[1]);
    ofstream out1(argv[2]);
    ofstream out2(argv[3]);
    ofstream out3(argv[4]);
    ofstream out4(argv[5]);
    ofstream out5(argv[6]);
    
    if(inFile.is_open()){
        // read image header
        int r,c,min,max;
        inFile>>r>>c>>min>>max;
        Image img(r,c,min,max);
        img.loadImage(inFile); // step 1 : loadImage
        img.mirrorFramed();    // step 2 : mirrorFramed
        Edge edge(r, c, img.mirrorFramedAry);
        for(int i = 1; i < r+1; i++)            // step 3: process the mirrorFramedAry
            for (int j = 1; j < c+1; j++) {     // begin at (1,1)
                edge.convolute(i, j, 0);
                edge.convolute(i, j, 1);
                edge.convolute(i, j, 2);
                edge.convolute(i, j, 3);
                edge.computeGradiant(i, j);
            }// step 4 : repeat step 3 until all pixels are processed
        /*
         step 5: write the results of the four Sobel edge arrays to argv[2],
         argv[3], argv[4], argv[5] write the result of the gradiant edge array to argv[6]
         */
        edge.print(out1, out2, out3, out4, out5);
        
        if(argc >=7){
            ofstream out6(argv[7]);
            out6<<"SobelVertical:"<<endl;
            img.prettyPrint(out6, edge.SobelVertical);
            out6<<"\nSobelHorizontal:"<<endl;
            img.prettyPrint(out6, edge.SobelHorizontal);
            out6<<"\nSobelRightDiag:"<<endl;
            img.prettyPrint(out6, edge.SobelRightDiag);
            out6<<"\nSobelLeafDiag:"<<endl;
            img.prettyPrint(out6, edge.SobelLeafDiag);
            out6<<"\nGradiantEdge:"<<endl;
            img.prettyPrint(out6, edge.GradiantEdge);
            out6.close();
        }
    }
    // step 6: close all files
    inFile.close();
    out1.close();
    out2.close();
    out3.close();
    out4.close();
    out5.close();
    return 0;
}
