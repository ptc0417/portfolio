#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define FRAME_WIDTH		640
#define FRAME_HEIGHT	480

#define FRAME_BUFFER_DEVICE	"/dev/fb0"



/*************************************************************
 *  <<Julia set相關資料>>
 *  https://en.wikipedia.org/wiki/Julia_set
 *
 *  cX 為 Julia set數學式中複數 "c" 的實部
 *  cY 為 Julia set數學式中複數 "c" 的虛部
 *  調整cX(值域:-1.0~1.0)與cY(值域:0.0~1.0)可得到不同的圖形
 *

*************************************************************/


int main()
{
	//RGB16
	int16_t frame[FRAME_HEIGHT][FRAME_WIDTH];

	int max_cX = -700;
	int min_cY = 270;

	int cY_step = -5;
	int cX = -700;	// x = -700~-700
	int cY;			// y = 400~270

	int fd;

    char team[20] ;
    char name1[20] ;
    char name2[20] ;
    char name3[20] ;

    int *id1 = malloc(sizeof(int));
    int *id2 = malloc(sizeof(int));
    int *id3 = malloc(sizeof(int));
    int *sum = malloc(sizeof(int));

	printf( "Function1: Name\n" );

	//Dummy Function. Please refer to the specification of Project 1.
	NAME(team, name1, name2, name3 ) ;

	printf( "Function2: ID\n" );

	//Dummy Function. Please refer to the specification of Project 1.
	ID(id1, id2, id3, sum);

	//Dummy printout. Please refer to the specification of Project 1.
	printf( "Main Function:\n" );
	printf( "*****Print All*****\n" );
    printf("%s\n", team ) ;
    printf("%d  %s\n",*id1,name1);
    printf("%d  %s\n",*id2,name2);
    printf("%d  %s\n",*id3,name3);
	printf( "ID Summation = %d\n",*sum );
	printf( "*****End Print*****\n" );


	printf( "\n***** Please enter p to draw Julia Set animation *****\n" );

	while(getchar()!='p') {}

	// 清除畫面
	system( "clear" );

	// 打開 Frame Buffer 硬體裝置的Device Node，準備之後的驅動程式呼叫
	fd = open( FRAME_BUFFER_DEVICE, (O_RDWR | O_SYNC) );

	if( fd<0 )
	{	printf( "Frame Buffer Device Open Error!!\n" );	}
	else
	{
		for( cY=400 ; cY>=min_cY; cY = cY + cY_step ) {

			// 計算目前cX,cY參數下的Julia set畫面
			drawJuliaSet( cX, cY, FRAME_WIDTH, FRAME_HEIGHT, frame );

			// 透過低階I/O操作呼叫Frame Buffer的驅動程式
			// (將畫面資料寫入Frame Buffer)
			write( fd, frame, sizeof(int16_t)*FRAME_HEIGHT*FRAME_WIDTH );

			// 移動檔案操作位置至最前端，以便下一次的畫面重新寫入
			lseek( fd, 0, SEEK_SET );
		}


		//Dummy printout. Please refer to the specification of Project 1.

		printf( ".*.*.*.<:: Happy New Year ::>.*.*.*.\n" );

        printf("by %s\n", team ) ;
        printf("%d  %s\n",*id1,name1);
        printf("%d  %s\n",*id2,name2);
        printf("%d  %s\n",*id3,name3);

		// 關閉 Device Node檔案，結束驅動程式的使用
		close( fd );
	}

	// 等待使用者輸入正確指令
	while(getchar()!='p') {
	}

	return 0;
}
