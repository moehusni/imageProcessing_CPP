//
//  main.cpp
//  Project9.2
//
//  Created by jian chen on 4/19/17.
//  Copyright © 2017 Jian Chen. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

class Point {
public:
    int X;
    int Y;
    int ClusterID;
    double distance;
    void printPoint(ofstream& outFile1){
        outFile1<< X << " " << Y << " " << ClusterID<<endl;
    }
};

class K_IsoData {
    int K;
    struct xyCoord{
        int x;
        int y;
    };
    int numPts;
    Point* pointSet;
    xyCoord* Centroids;
    double* XX2ndMM;
    double* YY2ndMM;
    double* XY2ndMM;
    int numRow;
    int numCol;
    int** imageArray;
    int* numInGroups;
    double* angle;
public:
    K_IsoData(int k,int r,int c,int numP) : K(k),numRow(r),numCol(c),numPts(numP) {
        pointSet = new Point[numPts];
        Centroids = new xyCoord[K];
        XX2ndMM = new double[K];
        XY2ndMM = new double[K];
        YY2ndMM = new double[K];
        imageArray = new int*[numRow];
        numInGroups = new int[k];
        angle = new double[K];
        for(int i = 0; i < numRow; i++)
            imageArray[i] = new int[numCol];
        
        for (int i = 0; i < numRow; i++)
            for (int j = 0; j < numCol; j++)
                imageArray[i][j] = 0;
    }
    
    void loadPointSet(ifstream& input){
        for(int i = 0; i < numPts; i++)
            input >> pointSet[i].X >> pointSet[i].Y;
    }
    
    void assignLabel(){
        for(int i = 0; i < numPts; i++){
            int label = i % K + 1;
            pointSet[i].ClusterID = label;
            numInGroups[label-1]++;
        }
    }
    
    void mapPoint2Image(){
        for (int i = 0; i < numPts; i++)
            imageArray[pointSet[i].X][pointSet[i].Y] = pointSet[i].ClusterID;
    }
    
    void setZero(){
        for(int i = 0; i < K; i++){
            Centroids[i].x = 0;
            Centroids[i].y = 0;
            XX2ndMM[i] = 0.0;
            YY2ndMM[i] = 0.0;
            XY2ndMM[i] = 0.0;
        }
    }
    
    void computeCentroidsAndAngles(){
        for(int i = 0; i < numPts; i++){
            Centroids[pointSet[i].ClusterID - 1].x += pointSet[i].X;
            Centroids[pointSet[i].ClusterID - 1].y += pointSet[i].Y;
            XX2ndMM[pointSet[i].ClusterID - 1] += pointSet[i].X * pointSet[i].X;
            YY2ndMM[pointSet[i].ClusterID - 1] += pointSet[i].Y * pointSet[i].Y;
            XY2ndMM[pointSet[i].ClusterID - 1] += pointSet[i].X * pointSet[i].Y;
        }
        
        for(int i = 0; i < K; i++){
            int members = numInGroups[i];
            Centroids[i].x /= members;
            Centroids[i].y /= members;
            XX2ndMM[i] /= members;
            XY2ndMM[i] /= members;
            YY2ndMM[i] /= members;
            
            double b = (XX2ndMM[i] - YY2ndMM[i]) / XY2ndMM[i];
            double ans1 = (-b + sqrt(b*b + 4)) / 2;
            angle[i] = atan(ans1) * 180 / 3.1415926;
        }
        
        
    }
    
    void IsoClustering(ofstream& outFile1,ofstream& outFile2){
        assignLabel();          // step 2
        bool firstTime = true;
        bool changeLabel = true;
        int i = 0;
        while(changeLabel){
            cout << i++ <<endl;
            changeLabel = false;
            mapPoint2Image();       // step 3
            prettyPrint(outFile2);  // step 4
            setZero();
            computeCentroidsAndAngles();// step 5 - 7
            if(firstTime){
                for(int i = 0; i < numPts; i++){
                    double t = angle[pointSet[i].ClusterID - 1] -
                        atan2(pointSet[i].X,pointSet[i].Y) - 3.1415926/2;
                    pointSet[i].distance = abs(sqrt(pow(pointSet[i].X,2)
                                        + pow(pointSet[i].Y,2)) * cos(t));
                }
            }
            
            double slope[K];
            double b[K];
            double denominator[K];
            for(int i = 0; i < numPts; i++){
                double X = pointSet[i].X - Centroids[pointSet[i].ClusterID - 1].x;
                slope[pointSet[i].ClusterID - 1] += X * (pointSet[i].Y - Centroids[pointSet[i].ClusterID - 1].y);
                denominator[pointSet[i].ClusterID - 1] += X * X;
            }
            
            for(int i = 0; i < K; i++){
                slope[i] /= denominator[i];
                b[i] = Centroids[i].y - slope[i] * Centroids[i].x;
            }
            
            
            for(int i = 0; i < numPts; i++){
                int minDist = pointSet[i].distance;
                int minLabel = pointSet[i].ClusterID;
                for (int j = 0; j < K; j++) {
                    double x_dis = (pointSet[i].Y - b[j]) / slope[j];
                    double angle = atan((Centroids[j].y - b[j]) / Centroids[j].x);
                    double temp = abs(sin(angle) * (pointSet[i].X - x_dis));
                    if(temp < minDist){ minDist = temp; minLabel = j + 1;}
                }
                
                if(minDist < pointSet[i].distance && pointSet[i].ClusterID != minLabel){
                    numInGroups[minLabel - 1]++;
                    numInGroups[pointSet[i].ClusterID - 1]--;
                    pointSet[i].distance = minDist;
                    pointSet[i].ClusterID = minLabel;
                    changeLabel = true;
                }
            }// step 8 - 9
        }// step 10
        
        printPointSet(outFile1); // step 11

    }
    
    void printPointSet(ofstream& outFile1){
        outFile1<<K<<endl;
        outFile1<<numPts<<endl;
        outFile1<<numRow<<" "<<numCol<<endl;
        for(int i = 0; i < numPts; i++)
            pointSet[i].printPoint(outFile1);
    }
    
    void prettyPrint(ofstream& outFile2){
        for (int i = 0; i < numRow; i++) {
            for (int j = 0; j < numCol; j++) {
                if(imageArray[i][j] > 0) outFile2<<imageArray[i][j]<<" ";
                else outFile2<<"  ";
            }
            outFile2<<endl;
        }
    }
};

int main(int argc, const char * argv[]) {
    ifstream inFile(argv[1]);
    ofstream outFile1(argv[2]);
    ofstream outFile2(argv[3]);
    int k;
    cout<< "enter K : ";
    cin>>k;
    int numRow, numCol, numPts;
    inFile >> numPts >> numRow >> numCol;
    K_IsoData iso(k,numRow,numCol,numPts);
    iso.loadPointSet(inFile);
    iso.IsoClustering(outFile1,outFile2);
    // step 12
    inFile.close();
    outFile1.close();
    outFile2.close();
    return 0;
}
