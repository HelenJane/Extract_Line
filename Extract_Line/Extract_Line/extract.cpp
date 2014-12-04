#include <stdio.h>
#include <math.h>
#include <cmath>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <iostream>
#include <fstream>
#include "ClusterAnalysis.h"


using namespace std;
#define BOUND 260
#define D 2
#define MAXSIZE 10000

double *LSM(double *x,double *y,double *re,int n)         //最小二乘法
{
	//double a,b,c;
	double x1=0,x2=0,x3=0,x4=0;
	double x1y1=0,x2y1=0,y1=0;
	double deno=0,m1=0,m2=0,m3=0;
	for(int i=0;i<n;i++)
	{
		x1+=x[i];
		x2+=x[i]*x[i];
		x3+=x[i]*x[i]*x[i];
		x4+=x[i]*x[i]*x[i]*x[i];
		y1+=y[i];
		x1y1+=x[i]*y[i];
		x2y1+=x[i]*x[i]*y[i];		
	}
	deno=x4*x2*n+x3*x1*x2+x2*x3*x1-(x2*x2*x2+x4*x1*x1+x3*x3*n);
	m1=x2y1*x2*n+x3*x1*y1+x2*x1y1*x1-(x2*x2*y1+x2y1*x1*x1+x3*x1y1*n);
	m2=x4*x1y1*n+x2y1*x1*x2+x2*x3*y1-(x2*x1y1*x2+x4*x1*y1+x2y1*x3*n);
	m3=x4*x2*y1+x3*x1y1*x2+x2y1*x3*x1-(x2y1*x2*x2+x4*x1y1*x1+x3*x3*y1);
	re[0]=m1/deno;
	re[1]=m2/deno;
	re[2]=m3/deno;
	return re;
}

//删除聚类点数少的聚类组。<总聚类点*20%则删除
void Filter(OutData outdata[],int cluster[],int cluster_num,int sum_cluster)  
{
	for(int i=0;i<sum_cluster;i++)
	{
		cluster[outdata[i].flag+1]++;
	} 
	for(int i=0;i<=cluster_num;i++)
		cout<<cluster[i]<<" ";
	for(int j=0;j<=cluster_num;j++)
	{
		if(cluster[j]<sum_cluster*10/100)
			cluster[j]=0;
	}
	cout<<endl;
	for(int i=0;i<=cluster_num;i++)
		cout<<cluster[i]<<" ";
	for(int i=0;i<sum_cluster;i++)
	{
		if(cluster[outdata[i].flag+1]==0)
			outdata[i].need=false;
	}
}

int main( int argc, char** argv )
{
	//int clusterId=0;
	//声明IplImage指针
	IplImage* img = NULL;
	//IplImage* cannyImg = NULL;
	char *filename;
	filename="curve3.jpg";
	//filename="straight_line3.jpg";
	img=cvLoadImage(filename,-1);
	int width=img->width;
	int height=img->height;
	CvScalar t2;
	CvScalar Cluster;      //聚类着色
	/*double *X=new double[height*width];
	double *Y=new double[height*width];*/
	//double *A=new double[3];
	IplImage *imggray=cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);;
	cvCvtColor(img,imggray,CV_BGR2GRAY);
	//载入图像，强制转化为Gray
	if((imggray = cvLoadImage(filename, 0)) != 0 )
	{
		//为canny边缘图像申请空间
		IplImage* cannyImg = cvCreateImage(cvGetSize(imggray),IPL_DEPTH_8U,1);
		IplImage* color_img =cvCreateImage(cvGetSize(imggray),IPL_DEPTH_8U,3);
		CvMemStorage *storage =cvCreateMemStorage();
		CvSeq *lines=0;
		//canny边缘检测
		cvCanny(imggray, cannyImg, 350, 400, 3);
		//去掉天空等非感兴趣区域
		for(int y=0;y<BOUND;y++)
		{
			uchar *ptr=(uchar*)cannyImg->imageData+y*cannyImg->widthStep;
			for(int x=0;x<cannyImg->width;x++)
			{
				ptr[x]=0;
			}
		}

		int i=0;
		InData indata[MAXSIZE];
	
		for(int y=0;y<height;y++)        //提取疑似车道的点
		{
				uchar *ptr1=(uchar*)cannyImg->imageData+y*cannyImg->widthStep;
				for(int x=0;x<width;x++)
				{
					if(ptr1[x]==255)
					{
			
						indata[i].x=x;
						indata[i].y=y;
						i++;
					}
				}
		}
    	ofstream in_file;
		in_file.open("in.txt",ios::out);
		for(int k=0;k<i;k++)
		{
			in_file<<indata[k].x<<" ";
			in_file<<indata[k].y<<endl;
		}
		ClusterAnalysis myClusterAnalysis; 
		myClusterAnalysis.Init(indata,i,15,3);
		myClusterAnalysis.DoDBSCANRecursive();
		OutData outdata[MAXSIZE];
		myClusterAnalysis.WriteToFile(outdata);

	
		int cluster_filter[10];
		for(int k=0;k<=clusterId;k++)
		{
			cluster_filter[k]=0;
		}
		Filter(outdata,cluster_filter,clusterId,i);

			//将聚类后的坐标及类别标记存入out.txt文件
		ofstream out_file;
		out_file.open("out.txt",ios::out);

		for(int k=0;k<i;k++)
		{
			out_file<<outdata[k].x<<" ";
			out_file<<outdata[k].y<<" ";
			out_file<<outdata[k].flag<"  ";
			out_file<<outdata[k].need<<endl;
		}
		  //为不同的聚类点着不同的颜色，供检测聚类是否正确，为中间结果，后期可删除
		cvSaveImage("outP1.jpg",cannyImg);            
		cvCvtColor(cannyImg,color_img,CV_GRAY2BGR);
		
		/*while(countnum<i)              
		{
			if(outdata[countnum].need)
			{
			switch(outdata[countnum].flag)
			{
			case -1:
				{
				Cluster.val[0]=255;
				Cluster.val[1]=0;
				Cluster.val[2]=255;
				break;
				}
			case 0:
				{
				Cluster.val[0]=0;
				Cluster.val[1]=255;
				Cluster.val[2]=0;
				break;
				}
			case 1:
				{
				Cluster.val[0]=0;
				Cluster.val[1]=0;
				Cluster.val[2]=255;
				break;
				}
			case 2:
				{
				Cluster.val[0]=0;
				Cluster.val[1]=255;
				Cluster.val[2]=255;
				break;
				}
			case 3:
				{
				Cluster.val[0]=255;
				Cluster.val[1]=255;
				Cluster.val[2]=0;
				break;
				}
			default:
				{
				Cluster.val[0]=255;
				Cluster.val[1]=0;
				Cluster.val[2]=255;
				break;
				}
			}
			cvSet2D(color_img,outdata[countnum].y,outdata[countnum].x,Cluster);
			}
			countnum++;
		}
*/

// ##########后期处理：对不同类别的点分别进行曲线拟合！############
		//############未完待续
		int turn=-1;
		while(turn<clusterId)
		{
			bool flag=false;
			CvScalar t1;
			int j=0;
			int countnum=0;
			double X[MAXSIZE];
			double Y[MAXSIZE];	
			double min_x=width-1;
			double max_x=0;
			while(countnum<i)
			{
				
				if(outdata[countnum].need&&outdata[countnum].flag==turn)
				{
					flag=true;
					X[j]=outdata[countnum].x;
					Y[j]=outdata[countnum].y;
					if(X[j]<min_x)
						min_x=X[j];
					if(X[j]>max_x)
						max_x=X[j];
					j++;
					
				}
				countnum++;
			}
			if(flag)
			{
			ofstream XY_file;
			XY_file.open("xy.txt",ios::out);
			int k=0;
			for(k=0;k<j;k++)
			{
				XY_file<<X[k]<<" ";
				XY_file<<Y[k]<<endl;
			}
			double *A=new double[3];
			A=LSM(X,Y,A,j);//获得多项式系数
			cout<<A[0]<<" "<<A[1]<<" "<<A[2]<<endl;
			for(int x=0;x<width-3;x++)
			{
					//描出符合多项式y=a*x^2+b*x+c的点
				int y=A[0]*pow(double(x),2)+A[1]*x+A[2];
					//int y=0.00101126*pow(double(x),2)-0.243228*x+284.776;
				if(y>Y[0]&&y<Y[k-1]&&x>min_x&&x<max_x)
				{
					t1.val[0]=0;
					t1.val[1]=0;
					t1.val[2]=255;
					cvSet2D(img,y,x,t1);
				}		
			}
			}
			turn++;
		}
		



	//A=LSM(X,Y,A,i);//获得多项式系数
	//			for(int x=100;x<width-3;x++)
	//			{
	//				//描出符合多项式y=a*x^2+b*x+c的点
	//				int y=A[0]*pow(double(x),2)+A[1]*x+A[2];
	//				//int y=0.00101126*pow(double(x),2)-0.243228*x+284.776;
	//				{
	//					t1.val[0]=255;
	//					t1.val[1]=0;
	//					t1.val[2]=0;
	//					cvSet2D(img,y,x,t1);
	//					/*cvSet2D(img,y,x+1,t1);
	//					cvSet2D(img,y,x-1,t1);
	//					cvSet2D(img,y,x-2,t1);
	//					cvSet2D(img,y,x-3,t1);*/
	//				}
	//					
	//			}
		
		cvSaveImage("outP.jpg",img);
		cvSaveImage("cluster.jpg",color_img);
		//创建窗口
		cvNamedWindow("src", 1);
		cvNamedWindow("canny",1);

		//cvNamedWindow("hough",1);
		//显示图像
		cvShowImage( "src", cannyImg );
		cvShowImage( "canny", img );
		//cvShowImage ("hough",color_img);
		cvWaitKey(0); //等待按键
		//销毁窗口
		cvDestroyWindow( "src" );
		cvDestroyWindow( "canny" );
		//cvDestroyWindow ("hough");
		//释放图像
		cvReleaseImage( &img );
		cvReleaseImage( &cannyImg );
		return 0;
	}
	return -1;
}