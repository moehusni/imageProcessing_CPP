#include <iostream>
#include <fstream>
using namespace std;

class Image {

private:
	int numRows; // number of row
	int numCols; // number of col
	int minVal;  // minimum value
	int maxVal;	 // maximum value

public:
	int** imgAry;

	Image(int numRows, int numCols, int minVal, int maxVal) {
		this->numRows = numRows;
		this->numCols = numCols;
		this->minVal = minVal;
		this->maxVal = maxVal;

		// dynamiclly allocating imgAry
		imgAry = new int*[numRows + 2];
		for (int i = 0; i < numRows + 2; i++)
			imgAry[i] = new int[numCols + 2];
	}

	void zeroFramed() {
		for (int i = 0; i < numRows + 2; i++) {
			imgAry[i][0] = 0;
			imgAry[i][numCols + 1] = 0;
		}
		for (int i = 1; i < numCols + 1; i++) {
			imgAry[0][i] = 0;
			imgAry[numRows + 1][i] = 0;
		}
	}

	void loadImage(ifstream &inFile) {
		for (int i = 1; i <= numRows; i++)
			for (int j = 1; j <= numCols; j++)
				inFile >> imgAry[i][j];
	}

	friend class chainCode;
};

class chainCode {

	// a nested Point Class to create an Object with row and column feilds
	class Point {
	public:
		int row; // pixel at row
		int col; // pixel at column
	};
public:
	Point neighborCoord[8]; // array list of Point
	Point startP;   // x-y coordinate of the first Pixel of the object
	Point currentP; // current none zero border pixel
	Point nextP;    // next none zore borader neighbor pixel
	int lastQ;      // rang from 0 tp 7, its the direction of
	// the last zero scanned from currentP
	int nextQ;
	// a look up table representing chain code
	// index Number = old p. dirction [E,NE, N,NW, W,SW, S,SE]
	// index value  = new p. dirction [S, E, E, N, N, W, W, S];
	int nextDirTable[8] = { 6, 0, 0, 2, 2, 4, 4, 6 };
	int nextDir; // the next scanning dir of the currentP's neighbor to find nextP,
	//range [0,7] need to mod  8.
	int PchainDir; // chain code direction from currentP to nextP

	int count;
	int label;      // label number

public:
	chainCode(int i, int j, int value) {
		// startP <-- (iRow, jCol)
		startP = {i, j};
		// currentP <-- (iRow, jCol)
		currentP = {i, j};
		lastQ = 4;
		label = value;
	}
	void loadNeighborsCoord(Point current) {
		// old p. dirction.
		// [NW] [N] [NE]
		//     \ | /
		// [W] -[P]- [E]
		//     / | \
		// [SW] [S] [SE]
		neighborCoord[0] = {current.row,current.col + 1};       //0
		neighborCoord[1] = {current.row - 1, current.col + 1};  //1
		neighborCoord[2] = {current.row - 1, current.col};      //2
		neighborCoord[3] = {current.row - 1, current.col -1};   //3
		neighborCoord[4] = {current.row, current.col - 1};      //4
		neighborCoord[5] = {current.row + 1, current.col - 1};  //5
		neighborCoord[6] = {current.row + 1, current.col};      //6
		neighborCoord[7] = {current.row + 1, current.col + 1};  //7
	}

	int findNextP(Point currentP, int nextQ, Point nextp, int** imgAry) {
		loadNeighborsCoord(currentP);
		int chainDir = getChainDir(imgAry);
		return chainDir;
	}
	int getChainDir(int** imgAry) {
		int chainDir = nextQ;
		while(imgAry[neighborCoord[chainDir].row][neighborCoord[chainDir].col] == 0) {
			chainDir = (chainDir + 1) % 8;
		}
		nextP = neighborCoord[chainDir];
		return chainDir;
	}

	void outputDir(ofstream &outFile1, ofstream &outFile2) {
		// output to file
		outFile1<<PchainDir;
		outFile2<<PchainDir<<" ";
		if(++count % 20 == 0) outFile2<<endl;
	}

	void getChainCode(ofstream &outFile1, ofstream &outFile2, int** imgAry) {
		do {
			// step 3:
			nextQ = (lastQ + 1) % 8;// nextQ <-- mod (lastQ + 1 , 8)

			// step 4:
			PchainDir = findNextP(currentP, nextQ, nextP, imgAry);// nextP will be determined inside the findNextP function

			// step 5:
			outputDir(outFile1, outFile2);

			// step 6:
			lastQ = nextDirTable[PchainDir];
			currentP = nextP;
		}
		// step 7: repeat step 3 tp ste[ 6 until currentP == startP
		while (currentP.row != startP.row || currentP.col != startP.col);

	}

};

int main(int argc, const char * argv[]) {
	// step 1: open inFile and outFiles
	ifstream inFile(argv[1]);
	ofstream outFile1(argv[2]);
	ofstream outFile2(argv[3]);

	if (inFile.is_open() && outFile1.is_open() && outFile2.is_open()) {

		int r, c, min, max;
		inFile >> r >> c >> min >> max;
		Image img(r, c, min, max);
		img.zeroFramed();
		img.loadImage(inFile);
		int first_row, first_col;
		// step 2: Scan the zeroFramed iamge form L to R & T to B
		for (int i = 1; i <= r; i++)
			for (int j = 1; j <= c; j++)
				if (img.imgAry[i][j] > 0) { // step 3
					first_row = i;
					first_col = j;
					outFile1 << r << " " << c << " " << min << " " << max << " " << endl
							<< i << j << img.imgAry[i][j];
					outFile2 << r << " " << c << " " << min << " " << max << " " << endl		;
					outFile2 << i << " " << j << " " << img.imgAry[i][j] << " ";
					goto gotFirstPixel;
				}

		gotFirstPixel: chainCode chain(first_row, first_col,
				img.imgAry[first_row][first_col]);
		chain.getChainCode(outFile1, outFile2, img.imgAry);
	} else {
		cout << "Check your Argv" << std::endl;
	}
	// step 9: close all files
	inFile.close();
	outFile1.close();
	outFile2.close();

	return 0;
}
