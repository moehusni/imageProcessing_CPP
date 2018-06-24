#include <iostream>
#include <fstream>

using namespace std;

class Morphology {
public:
    int numRowsIMG;
    int numColsIMG;
    int minValIMG;
    int maxValIMG;
    
    int numRowsStrctElem;
    int numColsStrctElem;
    int minValStrctElem;
    int maxValStrctElem;
    
    int rowOrigin;
    int colOrigin;
    int rowFrameSize;
    int colFrameSize;
    
    int** imgAry;
    int** morphAry;
    int** morphAry2;
    int** structElem;
   
    Morphology(int r1, int c1, int min1, int max1,int r2, int c2, int min2, int max2)
      : numRowsIMG(r1), numColsIMG(c1), minValIMG(min1), maxValIMG(max2),
        numRowsStrctElem(r2), numColsStrctElem(c2), minValStrctElem(min2), maxValStrctElem(max2){
        
        // step 1: compute Frame Size
        computeFrameSize();
        imgAry = new int*[numRowsIMG + rowFrameSize];
        morphAry = new int*[numRowsIMG + rowFrameSize];
        morphAry2 = new int*[numRowsIMG + rowFrameSize];
        for(int i = 0; i < numRowsIMG + rowFrameSize; i++){
            imgAry[i] = new int[numColsIMG + colFrameSize];
            morphAry[i] = new int[numColsIMG + colFrameSize];
            morphAry2[i] = new int[numColsIMG + colFrameSize];
        }
            
        structElem = new int*[numRowsStrctElem];
        for(int i = 0; i < numRowsStrctElem; i++) structElem[i] = new int[numColsStrctElem];
    }

    void computeFrameSize(){
        rowFrameSize = 2 * numRowsStrctElem;
        colFrameSize = 2 * numColsStrctElem;
    }
    
    void loadImage(ifstream& input){
        for(int i = numRowsStrctElem; i < numRowsIMG + numRowsStrctElem; i++)
            for(int j = numColsStrctElem; j < numColsIMG + numColsStrctElem; j++)
                input>>imgAry[i][j];
    }
    
    void loadStruct(ifstream& input){
        input>>rowOrigin>>colOrigin;
        for(int i = 0; i < numRowsStrctElem; i++)
            for(int j = 0; j < numColsStrctElem; j++)
                input>>structElem[i][j];
    }
    
    void zeroFrameImage(){
        for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
            for(int j = 0; j < numColsIMG + colFrameSize; j++){
                imgAry[i][j] = 0;
                morphAry[i][j] = 0;
                morphAry2[i][j] = 0;
            }
    }
    
    void dilation(int** img, int** morph){ //using img and write the result to morph
        for(int r = numRowsStrctElem; r < numRowsIMG + numRowsStrctElem; r++)
            for(int c = numColsStrctElem; c < numColsIMG + numColsStrctElem; c++)
                if(img[r][c] > 0)
                    for(int i = 0; i < numRowsStrctElem; i++)
                        for(int j = 0; j < numColsStrctElem; j++)
                            if(structElem[i][j] > 0)
                                morph[i+r-rowOrigin][j+c-colOrigin] = structElem[i][j];
    }
    
    bool isAllMatch(int r, int c, int** img){
        for(int i = 0; i < numRowsStrctElem; i++)
            for(int j = 0; j < numColsStrctElem; j++)
                if(structElem[i][j] > 0 && img[r-rowOrigin+i][c-colOrigin+j] != structElem[i][j])
                    return false;
        return true;
    }
    
    void erosion(int** img, int** morph){  //using img and write the result to morph
        for(int r = numRowsStrctElem; r < numRowsIMG + numRowsStrctElem; r++)
            for(int c = numColsStrctElem; c < numColsIMG + numColsStrctElem; c++)
                if(img[r][c] > 0 && isAllMatch(r, c, img)) morph[r][c] = 1;
    }
    
    void closing(){
        dilation(imgAry,morphAry2);
        erosion(morphAry2,morphAry);
    }
    
    void opening(){
        erosion(imgAry,morphAry2);
        dilation(morphAry2,morphAry);
    }
    
    void prettyPrint(int** ary, int rowStart, int colStart, int rowEnd, int colEnd){
        for(int i = rowStart; i < rowEnd; i++){
            for(int j = colStart; j < colEnd; j++)
                if(ary[i][j] > 0) cout<< ary[i][j] << " ";
                else cout<<"  ";

            cout<<endl;
        }
        
    }
    
    void outPutResult(ofstream& outFile){
        outFile<<numRowsIMG<<" "<<numColsIMG<<" "<<minValIMG<<" "<<maxValIMG<<endl;
        for(int i = numRowsStrctElem; i < numRowsIMG + numRowsStrctElem; i++){
            for(int j = numColsStrctElem; j < numColsIMG + numColsStrctElem; j++)
                outFile<<morphAry[i][j]<<" ";
            outFile<<endl;
        }
    }
    
    void reset(){
        for(int i = 0; i < numRowsIMG + rowFrameSize; i++)
            for(int j = 0; j < numColsIMG + colFrameSize; j++){
                morphAry[i][j] = 0;
                morphAry2[i][j] = 0;
            }
    }
    
};

int main(int argc, const char * argv[]) {

    // open input1 and input2
    ifstream inFile1(argv[1]);
    ifstream inFile2(argv[2]);
    ofstream outFile1(argv[3]);
    ofstream outFile2(argv[4]);
    ofstream outFile3(argv[5]);
    ofstream outFile4(argv[6]);
    
    int r1, c1, min1, max1;
    int r2, c2, min2, max2;
    inFile1>>r1>>c1>>min1>>max1;
    inFile2>>r2>>c2>>min2>>max2;
    
    // step 1 - 3
    Morphology morphology(r1,c1,min1,max1,r2,c2,min2,max2);
    morphology.zeroFrameImage();
    morphology.loadImage(inFile1);
    cout<<"Input Image:\n"<<endl;
    morphology.prettyPrint(morphology.imgAry,r2,c2,r1+r2,c1+c2);
    morphology.loadStruct(inFile2);
    cout<<"\nStructuring Element:\n"<<endl;
    morphology.prettyPrint(morphology.structElem, 0, 0, r2, c2);
    
    // step 4: call delation
    morphology.dilation(morphology.imgAry,morphology.morphAry);
    cout<<"\nAfter Dilation:\n"<<endl;
    morphology.prettyPrint(morphology.morphAry, r2, c2, r1+r2, c1+c2);
    morphology.outPutResult(outFile1);
    morphology.reset();
    
    // step 5: call erosion
    morphology.erosion(morphology.imgAry,morphology.morphAry);
    cout<<"\nAfter Erosion:\n"<<endl;
    morphology.prettyPrint(morphology.morphAry, r2, c2, r1+r2, c1+c2);
    morphology.outPutResult(outFile2);
    morphology.reset();
    
    // step 6: call closing
    morphology.closing();
    cout<<"\nAfter Closing:\n"<<endl;
    morphology.prettyPrint(morphology.morphAry, r2, c2, r1+r2, c1+c2);
    morphology.outPutResult(outFile3);
    morphology.reset();
    
    // step 7: call opening
    morphology.opening();
    cout<<"\nAfter Opening:\n"<<endl;
    morphology.prettyPrint(morphology.morphAry, r2, c2, r1+r2, c1+c2);
    morphology.outPutResult(outFile4);
    
    // step 8: close all files
    inFile1.close();
    inFile2.close();
    outFile1.close();
    outFile2.close();
    outFile3.close();
    outFile4.close();
    
    return 0;
}
