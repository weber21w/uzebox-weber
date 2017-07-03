#include <stdio.h>

unsigned char convertmaptile(unsigned char c1, unsigned char c2){
	if(c1 == 'B' && c2 == 'R')
		return 0;
	if(c1 == 'T' && c2 == 'R')
		return 1;
	if(c1 == 'R' && c2 == 'K')
		return 2;
	if(c1 == 'W' && c2 == 'T')
		return 3;
	if(c1 == 'W' && c2 == 'U')
		return 4;
	if(c1 == 'W' && c2 == 'D')
		return 5;
	if(c1 == 'W' && c2 == 'L')
		return 6;
	if(c1 == 'W' && c2 == 'R')
		return 7;
	if(c1 == 'B' && c2 == 'X')
		return 8;
	if(c1 == 'B' && c2 == 'Y')
		return 9;
	if(c1 == 'C' && c2 == 'X')
		return 10;
	if(c1 == 'C' && c2 == 'Y')
		return 11;
	if(c1 == 'E' && c2 == 'F')
		return 12;
	if(c1 == 'H' && c2 == 'T')
		return 13;
	if(c1 == 'H' && c2 == 'S')
		return 14;
	if(c1 == 'G' && c2 == 'S')
		return 15;
	if(c1 == 'S' && c2 == 'N')
		return 16;
	if(c1 == 'L' && c2 == 'O')
		return 17;
	if(c1 == 'S' && c2 == 'N')
		return 18;
	if(c1 == 'M' && c2 == 'D')
		return 19;
	if(c1 == 'D' && c2 == 'U')
		return 20;
	if(c1 == 'D' && c2 == 'D')
		return 21;
	if(c1 == 'D' && c2 == 'L')
		return 22;
	if(c1 == 'D' && c2 == 'R')
		return 23;
	if(c1 == 'G' && c2 == 'U')
		return 24;
	if(c1 == 'G' && c2 == 'D')
		return 25;
	if(c1 == 'G' && c2 == 'L')
		return 26;
	if(c1 == 'G' && c2 == 'R')
		return 27;
	if(c1 == 'A' && c2 == 'U')
		return 28;
	if(c1 == 'A' && c2 == 'D')
		return 29;
	if(c1 == 'A' && c2 == 'L')
		return 30;
	if(c1 == 'A' && c2 == 'R')
		return 31;
	if(c1 == 'L' && c2 == 'U')
		return 32;
	if(c1 == 'L' && c2 == 'D')
		return 33;
	if(c1 == 'L' && c2 == 'L')
		return 34;
	if(c1 == 'L' && c2 == 'R')
		return 35;
	if(c1 == 'S' && c2 == 'U')
		return 36;
	if(c1 == 'S' && c2 == 'D')
		return 37;
	if(c1 == 'S' && c2 == 'L')
		return 38;
	if(c1 == 'S' && c2 == 'R')
		return 39;
	if(c1 == 'R' && c2 == 'U')
		return 40;
	if(c1 == 'R' && c2 == 'D')
		return 41;
	if(c1 == 'R' && c2 == 'L')
		return 42;
	if(c1 == 'R' && c2 == 'R')
		return 43;
	if(c1 == 'O' && c2 == 'U')//MOBY
		return 44;
	if(c1 == 'O' && c2 == 'D')
		return 45;
	if(c1 == 'O' && c2 == 'L')
		return 46;
	if(c1 == 'O' && c2 == 'R')
		return 47;
	if(c1 == 'S' && c2 == '0')
		return 48;
	if(c1 == 'S' && c2 == '1')
		return 49;
	if(c1 == 'S' && c2 == '2')
		return 50;
	if(c1 == 'S' && c2 == '3')
		return 51;
	if(c1 == 'S' && c2 == '4')
		return 52;
	if(c1 == 'S' && c2 == '5')
		return 53;
	if(c1 == 'S' && c2 == '6')
		return 54;
	if(c1 == 'S' && c2 == '7')
		return 55;
	if(c1 == 'C' && c2 == 'S')
		return 56;
	if(c1 == 'W' && c2 == 'U')
		return 57;
	if(c1 == 'W' && c2 == 'D')
		return 58;
	if(c1 == 'W' && c2 == 'L')
		return 59;
	if(c1 == 'W' && c2 == 'R')
		return 60;
//	if(c1 == '' && c2 == '')
//		return ;

	
		
	return 255;
}


int main(int argc, char *argv[]) {
	FILE *fin = fopen("levels.txt","r");
	FILE *fout = fopen("../../default/lolo1.lvl","wb");
	printf("Lolo Level Convertor v1.0\n");
	if(fin == NULL || fout == NULL){
		printf("Failed to open file.\n\n");
		return 0;
	}

	unsigned char c1,c2,c3,x,y=0;
	int level = 0;	

	long int totalbytes = 0;
	while(!feof(fin)){
		fscanf(fin,"%c",&c1);
		totalbytes++;
	}
	unsigned short int numlevels = totalbytes/((11*11*3)+11+5);
 	fseek(fin,0,SEEK_SET);
	//fprintf(fout,"%d",numlevels);
	
	while(!feof(fin)){

		for(int i=0;i<11;i++){
			x = 0;
			for(int j=0;j<11;j++){
				fscanf(fin,"%c%c,",&c1,&c2);
				c3 = convertmaptile(c1,c2);
				if(c3 == 255){
					printf("Bad Map Tile %c%c On Level %d @%d,%d\n",c1,c2,level,x,y);
					return 0;
				}
				fprintf(fout,"%c",c3);
				x++;
			}
			y++;
			fscanf(fin,"\n");
		}
		fscanf(fin,"%c,%c,\n",&c1,&c2);//get door pos
		c1 -= '0';c2 -= '0';
		fprintf(fout,"%c%c",c1,c2);
		level++;
		y = 0;
	}
	_fcloseall();
	printf("Total Levels:%d\n",level);
	
	return 0;
}
