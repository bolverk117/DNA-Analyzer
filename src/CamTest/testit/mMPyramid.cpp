#include <iostream>
#include <cstdlib>
#include <cmath>

#define SIZE 16

int MAX(int a, int b)
{
	return((a > b) ? a : b);
}

int MIN(int a, int b)
{
	return((a > b) ? b : a);
}

int CEILING(float a)
{
	return((int) (a+1));
}

using namespace std;

int main()
{
	int image[SIZE][SIZE], minsurface[SIZE][SIZE], maxsurface[SIZE][SIZE], scratchmin[SIZE][SIZE], scratchmax[SIZE][SIZE], diffVolume[SIZE][SIZE];

	int i, j, k, m, n, myr, r, volume, minval, maxval;

	srand(5646);
	for (i=0; i < SIZE; i++)
		for (j=0; j<SIZE; j++)
		{
			image[i][j] = rand() % 255;
//			image[i][j] = 128;
			scratchmax[i][j] = image[i][j];
			scratchmin[i][j] = image[i][j];
		}

		// Original Image
		for (i=0; i < SIZE; i++)
		{
			for (j=0; j<SIZE; j++)
				if (image[i][j] < 10)
					cout << image[i][j] << "   ";
				else if (image[i][j] < 100)
					cout << image[i][j] << "  ";
				else
					cout << image[i][j] << " ";
				cout << endl;
		}
		cout << endl;
		myr = 1;
		// blocks of size 2x2 to 16x16
		for (k = SIZE/2; k > 0; k /= 2)
		{
			r = (SIZE/k);
			myr += 2;
			cout << "r = " << r << " Log(r) = " << log10(r) << endl;
			for (i = 0; i < k; i++)
			{
				for (j = 0; j < k; j++)
				{
					m = i*2;
					n = j*2;
					maxsurface[i][j] = MAX(MAX(scratchmax[m][n],scratchmax[m+1][n]), MAX(scratchmax[m][n+1],scratchmax[m+1][n+1]));
					minsurface[i][j] = MIN(MIN(scratchmin[m][n],scratchmin[m+1][n]), MIN(scratchmin[m][n+1],scratchmin[m+1][n+1]));
					diffVolume[i][j] = maxsurface[i][j] - minsurface[i][j];
				}
			}
			cout << "Min:" << endl;
			for (i = 0; i < k; i++)
			{
				for (j = 0; j < k; j++)
				{
					if (minsurface[i][j] < 10)
						cout << minsurface[i][j] << "   ";
					else if (minsurface[i][j] < 100)
						cout << minsurface[i][j] << "  ";
					else
						cout << minsurface[i][j] << " ";
					scratchmax[i][j] = maxsurface[i][j];
				}
				cout << endl;
			}
			cout << "MAX:" << endl;
			for (i = 0; i < k; i++)
			{
				for (j = 0; j < k; j++)
				{
					if (maxsurface[i][j] < 10)
						cout << maxsurface[i][j] << "   ";
					else if (maxsurface[i][j] < 100)
						cout << maxsurface[i][j] << "  ";
					else
						cout << maxsurface[i][j] << " ";
					scratchmin[i][j] = minsurface[i][j];
				}
				cout << endl;
			}
			cout << "Difference Volume:" << endl;
			volume = 0;
			for (i = 0; i < k; i++)
			{
				for (j = 0; j < k; j++)
				{
					volume += (diffVolume[i][j] / (SIZE / k)) + 1;
					if (diffVolume[i][j] < 10)
						cout << diffVolume[i][j] << "   ";
					else if (diffVolume[i][j] < 100)
						cout << diffVolume[i][j] << "  ";
					else
						cout << diffVolume[i][j] << " ";
				}
				cout << endl;
			}


			cout << "Volume = " << volume << " Log(Volume) = " << log10(volume) << endl;

			cout << "My way (or the highway)" << endl;
			minval = maxval = image[SIZE/2][SIZE/2];
			for (i = (SIZE/2)-(myr/2); i < (SIZE/2)+(myr/2); i++)
			{
				for (j = (SIZE/2)-(myr/2); j < (SIZE/2)+(myr/2); j++)
				{
					minval = MIN(minval, image[i][j]);
					maxval = MAX(maxval, image[i][j]);
				}
			}
			cout << "Min: " << minval << " Max: " << maxval << endl;
			cout << "My r: " << myr << " Log(r): " << log10(myr) << endl;
			cout << "Number of columns: " << (CEILING(SIZE/(1.0*myr))*CEILING(SIZE/(1.0*myr))) << endl;
			volume = (((maxval-minval)/myr)+1)*(CEILING(SIZE/(1.0*myr))*CEILING(SIZE/(1.0*myr)));
//			volume = (maxval - minval);
			cout << "My volume = " << volume << " log10(My volume): " << log10(volume) << endl;
			cout << endl;
		}

		return 0;
}
