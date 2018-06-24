import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.PrintWriter;
import java.util.Scanner;

public class Main {

	public static void main(String[] args) throws FileNotFoundException {
		Scanner inFile = new Scanner(new FileReader(args[0]));
	    PrintWriter outFile1 = new PrintWriter(args[1]);
	    PrintWriter outFile2 = new PrintWriter(args[2]);
	    System.out.print("enter K : ");
	    int k = (new Scanner(System.in)).nextInt();
	    int numPts = inFile.nextInt(); 
	    int numRow = inFile.nextInt();
	    int numCol = inFile.nextInt();
	    K_IsoData iso = new K_IsoData(k,numRow,numCol,numPts);
	    iso.loadPointSet(inFile);
	    iso.IsoClustering(outFile1,outFile2);
	    // step 12
	    inFile.close();
	    outFile1.close();
	    outFile2.close();
	}

}

class Point {
	public int X;
    public int Y;
    public int ClusterID;
    public double distance;
    
    public Point(int x, int y){
    	X = x;
    	Y = y;
    }
    
    public void printPoint(PrintWriter outFile1){
        outFile1.println(X + " " + Y + " " + ClusterID);
    }
}

class K_IsoData {
	private int K;
    
	class xyCoord{
        int x;
        int y;
//        public xyCoord(int x, int y) {
//        	this.x = x;
//        	this.y = y;
//		}
    };
    private int numPts;
    private Point[] pointSet;
    private xyCoord[] Centroids;
    private double[] XX2ndMM;
    private double[] YY2ndMM;
    private double[] XY2ndMM;
    private int numRow;
    private int numCol;
    private int[][] imageArray;
    private int[] numInGroups;
    private double[] angle;
    
    public K_IsoData(int k, int r,int c,int numP){
    	K = k; numPts = numP;
    	numRow = r; numCol = c;
    	pointSet = new Point[numPts];
        Centroids = new xyCoord[K];
        XX2ndMM = new double[K];
        XY2ndMM = new double[K];
        YY2ndMM = new double[K];
        imageArray = new int[numRow][numCol];
        numInGroups = new int[k];
        angle = new double[K];
        for(int i = 0; i < K; i++)
        	Centroids[i] = new xyCoord();
    }
    
    public void loadPointSet(Scanner input){
    	int x, y;
    	for(int i = 0; i < numPts; i++){
            x = input.nextInt();
	    	y = input.nextInt();
	    	pointSet[i] = new Point(x, y);
    	}	
    }
    
    public void assignLabel() {
    	for(int i = 0; i < numPts; i++){
            int label = i % K + 1;
            pointSet[i].ClusterID = label;
            numInGroups[label-1]++;
    	}
	}
    
    private void mapPoint2Image(){
        for (int i = 0; i < numPts; i++)
            imageArray[pointSet[i].X][pointSet[i].Y] = pointSet[i].ClusterID;
    }
    
    private void setZero(){
        for(int i = 0; i < K; i++){
            Centroids[i].x = 0;
            Centroids[i].y = 0;            
            XX2ndMM[i] = 0.0;
            YY2ndMM[i] = 0.0;
            XY2ndMM[i] = 0.0;
        }
    }
    
    private void computeCentroidsAndAngles(){
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
            double ans1 = (-b + Math.sqrt(b*b + 4)) / 2;
            angle[i] = Math.atan(ans1) * 180 / 3.1415926;
        }
    }
    
    void IsoClustering(PrintWriter outFile1,PrintWriter outFile2){
        assignLabel();          // step 2
        Boolean firstTime = true;
        Boolean changeLabel = true;
        while(changeLabel){
            changeLabel = false;
            mapPoint2Image();       // step 3
            prettyPrint(outFile2);  // step 4
            setZero();
            computeCentroidsAndAngles();// step 5 - 7
            if(firstTime){
                for(int i = 0; i < numPts; i++){
                    double t = angle[pointSet[i].ClusterID - 1] -
                        Math.atan2(pointSet[i].X,pointSet[i].Y) - 3.1415926/2;
                    pointSet[i].distance = Math.abs(Math.sqrt(Math.pow(pointSet[i].X,2)
                                        + Math.pow(pointSet[i].Y,2)) * Math.cos(t));
                }
            }
            
            double[] slope = new double[K];
            double[] b = new double[K];
            double[] denominator = new double[K];
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
                double minDist = pointSet[i].distance;
                int minLabel = pointSet[i].ClusterID;
                for (int j = 0; j < K; j++) {
                    double x_dis = (pointSet[i].Y - b[j]) / slope[j];
                    double angle = Math.atan2((Centroids[j].y - b[j]) , Centroids[j].x);
                    double temp = Math.abs(Math.sin(angle) * (pointSet[i].X - x_dis));
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

    void printPointSet(PrintWriter outFile1){
        outFile1.println(K);
        outFile1.println(numPts);
        outFile1.println(numRow + " " + numCol);
        for(int i = 0; i < numPts; i++)
            pointSet[i].printPoint(outFile1);
    }
    
    void prettyPrint(PrintWriter outFile2){
        for (int i = 0; i < numRow; i++) {
            for (int j = 0; j < numCol; j++) {
                if(imageArray[i][j] > 0) outFile2.print(imageArray[i][j] + " ");
                else outFile2.print("  ");
            }
            outFile2.println();
        }
    }   
}