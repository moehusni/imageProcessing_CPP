#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;


class BoundaryPt{
    friend class Image;
public:
    int X;
    int Y;
    int maxVotes;
    double maxDistance;
    int corner;
    bool isLocalMaxima;
    
    BoundaryPt(){
        maxVotes = 0;
        maxDistance = 0.0;
        corner = 1;
        isLocalMaxima = false;
        this->Y = 0;
        this->X = 0;
    }
    
    void loadXY(int x, int y){
        X = x;
        Y = y;
    }
};

class Image{
private:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int** imageAry;
    int label;
    
public:
    
    Image(int r, int c, int min, int max, int l)
            : numRows(r), numCols(c), minVal(min), maxVal(max), label(l){
        imageAry = new int*[numRows];
        for(int i =0;i<numRows; i++) imageAry[i] = new int[numCols];
    }
    
    void loadPtImage(BoundaryPt* boundAry,int numPts){
        int r, c;
        for(int i = 0; i < numPts; i++){
            r = boundAry[i].X;
            c = boundAry[i].Y;
            imageAry[r][c] = boundAry[i].corner;
        }
    }
    
    void prettyPrint(ofstream &outFile){
        for(int i = 0; i < numRows; i++){
            for(int j = 0; j < numCols; j++)
                if(imageAry[i][j] > 0) outFile << imageAry[i][j] << " ";
                else outFile<<"  ";
            outFile<<endl;
        }
    }
    
    void resultFile(ofstream &outFile, BoundaryPt* boundAry, int numPts){
        outFile<<numRows<<" "<<numCols<<" "<<minVal<<" "<<maxVal<<endl;
        outFile<<label<<endl;
        outFile<<numPts<<endl;
        for(int i = 0; i < numPts; i++)
            outFile<<boundAry[i].X<<" "<<boundAry[i].Y<<" "<<boundAry[i].corner<<endl;
    }
};

class ArcChordDistance{
    
    int KchordLength; // ask user form console
    int numPts;       // get from input-1
    double* chordAry;
    int P1;          // the array index of the first-end of the arc Chord;
    int P2;          // the array index of the second-end of the arc Chord;
    int currPt;
    
public:
    BoundaryPt* boundAry;
    
    ArcChordDistance(int k, int pts) : KchordLength(k), numPts(pts){
        boundAry = new BoundaryPt[numPts];
        chordAry = new double[k];
    }
    
    void loadData(int x, int y, int index){
        boundAry[index].loadXY(x, y);
    }
    
    double computeDistance(){
        double A = boundAry[P2].Y - boundAry[P1].Y;
        double B = boundAry[P1].X - boundAry[P2].X;
        double C = boundAry[P2].X * boundAry[P1].Y - boundAry[P1].X * boundAry[P2].Y;
        double dist = abs((A * boundAry[currPt].X + B * boundAry[currPt].Y + C))
                        / sqrt(A * A + B * B);
        return dist;
    }
    
    int findMaxDist(){
        int index = 0;
        double max = chordAry[0];
        for(int i = 1; i < KchordLength; i++)
            if(chordAry[i] > max){
                max = chordAry[i];
                index = i;
            }
        return index;
        
    }
    
    bool computeLocalMaxima(int index){
        for(int i = index - 3; i <= index + 3; i++){
            if(boundAry[(i+numPts) % numPts].maxVotes > boundAry[index].maxVotes)
                return false;
        }
        return true;
    }
    
    int isCorner(int index){
        if(boundAry[index].isLocalMaxima
           && boundAry[(index+numPts-1)%numPts].maxDistance < boundAry[index].maxDistance
           && boundAry[(index+1)%numPts].maxDistance < boundAry[index].maxDistance)

            return 9;
        
        return 1;
    }
    
    void doArcChordDistance(ofstream& outFile2, ofstream& outFile4){
        //step 4:
        P1 = 0;
        P2 = KchordLength - 1;
        
        while(P2 != KchordLength / 2){
            // step 5:
            int index = 0;
            currPt = P1;
            
            while(index < KchordLength){
                // step 6:
                double dist = computeDistance();
                chordAry[index] = dist;
                index++;
                currPt = (currPt + 1) % numPts;
            } // step 7 : repeat step 6 while index < KChordLength

            // step 8 : print chordAry to debugging file (argv[5])
            outFile4<<"P1: "<<P1<<" P2: "<<P2<<" currpt: "<<currPt<<endl;
            outFile4<<"chordAry: ";
            for(int i = 0; i < KchordLength; i++)
                outFile4<<chordAry[i]<<" ";
            outFile4<<endl;
            
            //step 9: find the max of distances of all points in chordAry
            int maxIndex = findMaxDist();
            int whichIndex = (P1 + maxIndex) % numPts;
            boundAry[whichIndex].maxVotes++;
            if(boundAry[whichIndex].maxDistance < chordAry[maxIndex]){
                boundAry[whichIndex].maxDistance = chordAry[maxIndex];
            }
            
            // step 10:
            for(int i = P1; i <= P2; i++)
                outFile4<<"boundAry["<<i<<"]: X = "<<boundAry[i].X<<" Y = "<<boundAry[i].Y<<endl;
            
            // step 11:
            P1 = (P1 + 1) % numPts;
            P2 = (P2 + 1) % numPts;
            
        }// step 12: repeat step 5 to step 11 until P2 == (KChordLength / 2)
        
        // step 13: print the info (x, y, maxVotes, maxDist) of the entire boundaryAry to argv[5]
        outFile4<< "\n=================================================================\n";
        outFile4 << "boundAry:" <<endl;
        for(int i = 0; i < numPts; i++){
            outFile4 <<i<<":x="<< boundAry[i].X<<",y="<<boundAry[i].Y<<",maxVotes="
            <<boundAry[i].maxVotes<<",maxDistance="<<boundAry[i].maxDistance<<"\t";
            if((i + 1) % 5 == 0) outFile4<<endl;
        }
        
        // step 14: compute LocalMaxima for all point in boundPtAry[index],
        //          index from 0 to numPts-1
        for(int i = 0; i < numPts; i++)
            boundAry[i].isLocalMaxima = computeLocalMaxima(i);
        
        // step 15:
        for(int i = 0; i < numPts; i++)
            boundAry[i].corner = isCorner(i);
        
        // step 16: output info (x, y, corner) of the entire boundaryAry to argv[3]
        for(int i = 0; i < numPts; i++)
            outFile2<<"X:"<<boundAry[i].X<<" Y:"<<boundAry[i].Y<<" Corner:"<<boundAry[i].corner<<endl;
    }
    
};

int main(int argc, const char * argv[]) {
    // step 0: open all files
    ifstream inFile(argv[1]);
    ofstream outFile1(argv[2]);
    ofstream outFile2(argv[3]);
    ofstream outFile3(argv[4]);
    ofstream outFile4(argv[5]);
    

    // ask user for KchordLength
    int k;
    cout<< "enter the value for KChordLength :";
    cin>>k;
    
    // read from argv[2] for numRows, numCols, minVal, maxVal, numPts
    int r, c, min, max, label, pts;
    inFile>>r>>c>>min>>max>>label>>pts;
    
    Image img(r,c,min,max,label);
    ArcChordDistance acd(k,pts);
    int index = 0;
    
    while(index < pts){
    // step 1: (x, y) <-- read from input
        int x, y;
        inFile>>x>>y;
        acd.loadData(x, y, index);
    // step 2:
        index++;
    } // step 3 : repeat step 1 - 2 until the file is empty
    
    // step 4 - 16
    acd.doArcChordDistance(outFile2,outFile4);
    
    // step 17: Call loadPtImage // load the boundary point onto the imgAry, i.e.,
    // for each boundaryAry[i], put it corner value at Img(x, y))
    img.loadPtImage(acd.boundAry,pts);
    
    // step 18: prettyPrint imgAry to argv[4]
    img.prettyPrint(outFile3);
    img.resultFile(outFile1, acd.boundAry, pts);
    
    // step 19: close all files
    inFile.close();
    outFile1.close();
    outFile2.close();
    outFile3.close();
    outFile4.close();
    return 0;
}
