import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.PrintWriter;
import java.util.Scanner;

public class Main {

	public static void main(String[] args) throws FileNotFoundException {
		Scanner inFile = new Scanner(new FileReader(args[0]));
		PrintWriter outFile = new PrintWriter(args[1]);
		int r = inFile.nextInt();
		int c = inFile.nextInt();
		int min =inFile.nextInt();
		int max = inFile.nextInt();
		int counter = 0;
		int[][] arr = new int[r][c];
		
		for (int i = 0; i < r; i++)
	        for(int j = 0; j < c; j++){
	            arr[i][j] = inFile.nextInt();
	            if(arr[i][j] > 0) counter++;
	        }
	    
	    outFile.println(counter);
	    outFile.println(r + " " + c);
	    for (int i = 0; i < r; i++)
	        for(int j = 0; j < c; j++){
	            if(arr[i][j] > 0)
	                outFile.println(i + " " + j);
	        }
	    inFile.close();
	    outFile.close();
	}
}
