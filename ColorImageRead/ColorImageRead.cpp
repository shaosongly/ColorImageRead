// ColorImageRead.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ColorImageRead.h"
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

typedef struct CvVecFile
{
    FILE*  input;//�����ļ�
    int    count;//�ļ���������
    int    vecsize;//�����ߴ�w*h
    int    last;
    uchar* vector;
} CvVecFile;

#define CHANNEL 3

int icvGetTraininDataFromVecCallback( CvMat* img, void* userdata )//����vec�ļ��е����ݻָ�ԭͼimg
{
	//img->operator CvMat();
    uchar tmp = 0;
    int r = 0;
    int c = 0;

    assert( img->rows * img->cols == ((CvVecFile*) userdata)->vecsize );
    
    fread( &tmp, sizeof( tmp ), 1, ((CvVecFile*) userdata)->input );
    fread( ((CvVecFile*) userdata)->vector, sizeof( uchar ),
           ((CvVecFile*) userdata)->vecsize*CHANNEL, ((CvVecFile*) userdata)->input );
    
    if( feof( ((CvVecFile*) userdata)->input ) || 
        (((CvVecFile*) userdata)->last)++ >= ((CvVecFile*) userdata)->count )
    {
        return 0;
    }
    
    for( r = 0; r < img->rows; r++ )
    {
        for( c = 0; c < img->cols; c++ )
        {
			//CV_MAT_ELEM( *img, uchar, r, c ) = (uchar) ( ((CvVecFile*) userdata)->vector[r * img->cols*3 + 3*c] );
			(img->data.ptr+r*img->step)[CHANNEL*c]=(uchar)( ((CvVecFile*) userdata)->vector[r * img->cols*CHANNEL + CHANNEL*c] );
			if(CHANNEL==3)
			{
				(img->data.ptr+r*img->step)[CHANNEL*c+1]=(uchar)( ((CvVecFile*) userdata)->vector[r * img->cols*CHANNEL + CHANNEL*c+1] );
				(img->data.ptr+r*img->step)[CHANNEL*c+2]=(uchar)( ((CvVecFile*) userdata)->vector[r * img->cols*CHANNEL + CHANNEL*c+2] );
			}
        }
    }

    return 1;
}




int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
			const char *filename="samples.vec";
			CvMat mat = cvMat(128, 64, CV_8UC3,cvAlloc( sizeof( uchar ) * 128 * 64*3 ) );
			CvMat mat1 = cvMat(128, 64, CV_8UC1,cvAlloc( sizeof( uchar ) * 128 * 64 ) );
			CvMat mat2 = cvMat(128, 64, CV_8UC1,cvAlloc( sizeof( uchar ) * 128 * 64 ) );
			CvMat mat3 = cvMat(128, 64, CV_8UC1,cvAlloc( sizeof( uchar ) * 128 * 64 ) );
			CvMat luv_mat = cvMat(128, 64, CV_8UC3,cvAlloc( sizeof( uchar ) * 128 * 64*3 ) );
			IplImage* img = cvCreateImage(cvGetSize(&mat),8,CHANNEL);
			cvNamedWindow("main",CV_WINDOW_AUTOSIZE);
			CvVecFile file;
			short tmp = 0;    
    
			file.input = NULL;
			if( filename ) file.input = fopen( filename, "rb" );
			if( file.input != NULL )
			{
				fread( &file.count, sizeof( file.count ), 1, file.input );
				fread( &file.vecsize, sizeof( file.vecsize ), 1, file.input );
				fread( &tmp, sizeof( tmp ), 1, file.input );
				fread( &tmp, sizeof( tmp ), 1, file.input );
				if( !feof( file.input ) )
				{
					file.last = 0;
					file.vector = (uchar*) cvAlloc( sizeof( *file.vector ) * file.vecsize*CHANNEL );
					while(true)
					{
						
						int value=icvGetTraininDataFromVecCallback(&mat,&file);
						if(!value)
						{
							break;
						}
						cvCvtColor(&mat,&luv_mat,CV_BGR2Luv);
						cvSplit(&luv_mat,&mat1,&mat2,&mat3,NULL);
						cvGetImage(&mat,img);
						cvShowImage("main",img);
						cvWaitKey(100);
						
					}
				}
				
			}
			
			fclose( file.input );
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}
	system("pause");
	return nRetCode;
}
