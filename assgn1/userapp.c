#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int main(int argc, const char * argv[]){
	uint16_t x=0;
	int fdx=open("/dev/adxl_x", O_RDONLY);
	int fdy=open("/dev/adxl_y", O_RDONLY);
	int fdz=open("/dev/adxl_z", O_RDONLY);
	char in;
	int flag=0;
	printf("Enter x, y, or z to read value for x,y and z axis respectively\n");
	printf("Enter q to stop the program\n");

	while (1){
		printf("> ");
		scanf(" %c", &in);
		switch (in){
			case 'x':
			case 'X':
				read(fdx, &x, 2);
				printf("ADXL X: %4x\n", x);
				break;
			case 'y':
			case 'Y':
				read(fdy, &x, 2);
				printf("ADXL Y: %4x\n", x);
				break;
			case 'z':
			case 'Z':
				read(fdz, &x, 2);
				printf("ADXL Z: %4x\n", x);
				break;
			case 'q':
			case 'Q':
				flag=1;
				break;
			default:
				printf("RETRY\n");
		}
		if (flag) break;
	}

	close(fdx);
	close(fdy);
	close(fdz);

	printf("Thank you!!!\n");
	return 0;
}