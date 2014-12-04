#include "ClusterAnalysis.h"
#include <fstream>
#include <iosfwd>
#include <math.h>

#include <cstdio>

using namespace std;
/*
�����������ʼ������
˵�����������ļ������뾶��������С���ݸ�����Ϣд������㷨�࣬��ȡ�ļ�����������Ϣ����д���㷨�����ݼ�����
������
char* fileName;    //�ļ���
double radius;    //�뾶
int minPTs;        //������С���ݸ���  
����ֵ�� true;    */

extern int clusterId;

bool ClusterAnalysis::Init(InData indata[],int num, double radius, int minPTs)
{
    this->radius = radius;        //���ð뾶
    this->minPTs = minPTs;        //����������С���ݸ���
    this->dimNum = DIME_NUM;    //��������ά��
    int j=0;
	int k=0;

    int i=0;            //���ݸ���ͳ��
    while (i<num )                //���ļ��ж�ȡPOI��Ϣ����POI��Ϣд��POI�б���
    {
        DataPoint tempDP;                //��ʱ���ݵ����
        double tempDimData[DIME_NUM];    //��ʱ���ݵ�ά����Ϣ
		tempDimData[j++]=indata[k].x;
		tempDimData[j]=indata[k].y;
		j=0;
        tempDP.SetDimension(tempDimData);    //��ά����Ϣ�������ݵ������

//char date[20]="";
//char time[20]="";
        ////double type;    //������Ϣ
        //ifs >> date;
//ifs >> time;    //������Ϣ����

        tempDP.SetDpId(i);                    //�����ݵ����ID����Ϊi
        tempDP.SetVisited(false);            //���ݵ����isVisited����Ϊfalse
        tempDP.SetClusterId(-1);            //����Ĭ�ϴ�IDΪ-1
        dadaSets.push_back(tempDP);            //������ѹ�����ݼ�������
        i++;        //����+1
    }
    dataNum =i;            //�������ݶ��󼯺ϴ�СΪi
    for(unsigned long i=0; i<dataNum;i++)
    {
        SetArrivalPoints(dadaSets[i]);            //�������ݵ������ڶ���
    }
    return true;    //����
}

/*
���������Ѿ��������㷨��������ݼ���д���ļ�
˵�������Ѿ���������д���ļ�
������
char* fileName;    //Ҫд����ļ���
����ֵ�� true    */
bool ClusterAnalysis::WriteToFile(OutData* outdata )
{    
    for(unsigned long i=0; i<dataNum;i++)                //�Դ������ÿ�����ݵ�д���ļ�
    {               //��ά����Ϣд���ļ�
			outdata[i].x=dadaSets[i].GetDimension()[0];
			outdata[i].y=dadaSets[i].GetDimension()[1];
			outdata[i].flag=dadaSets[i].GetClusterId();        //��������IDд���ļ�
    }
    return true;    //����
}

/*
�������������ݵ��������б�
˵�����������ݵ��������б�
������
����ֵ�� true;    */
void ClusterAnalysis::SetArrivalPoints(DataPoint& dp)
{
    for(unsigned long i=0; i<dataNum; i++)                //��ÿ�����ݵ�ִ��
    {
        double distance =GetDistance(dadaSets[i], dp);    //��ȡ���ض���֮��ľ���
        if(distance <= radius && i!=dp.GetDpId())        //������С�ڰ뾶�������ض����id��dp��id��ִͬ��
            dp.GetArrivalPoints().push_back(i);            //���ض���idѹ��dp�������б���
    }
    if(dp.GetArrivalPoints().size() >= minPTs)            //��dp���������ݵ�������> minPTsִ��
    {
        dp.SetKey(true);    //��dp���Ķ����־λ��Ϊtrue
        return;                //����
    }
    dp.SetKey(false);    //���Ǻ��Ķ�����dp���Ķ����־λ��Ϊfalse
}


/*
������ִ�о������
˵����ִ�о������
������
����ֵ�� true;    */
bool ClusterAnalysis::DoDBSCANRecursive()
{
    unsigned long clusterId=0;                        //����id��������ʼ��Ϊ0
    for(unsigned long i=0; i<dataNum;i++)            //��ÿһ�����ݵ�ִ��
    {
        DataPoint& dp=dadaSets[i];                    //ȡ����i�����ݵ����
        if(!dp.isVisited() && dp.IsKey())            //������û�����ʹ��������Ǻ��Ķ���ִ��
        {
            dp.SetClusterId(clusterId);                //���øö���������IDΪclusterId
            dp.SetVisited(true);                    //���øö����ѱ����ʹ�
            KeyPointCluster(i,clusterId);            //�Ըö��������ڵ���о���
            clusterId++;                            //clusterId����1
        }
        //cout << "������\T" << i << endl;
    }

    cout <<"������" <<clusterId<<"��"<< endl;        //�㷨��ɺ�����������
    return true;    //����
}

/*
�����������ݵ������ڵĵ�ִ�о������
˵�������õݹ�ķ�����������Ⱦ�������
������
unsigned long dpID;            //���ݵ�id
unsigned long clusterId;    //���ݵ�������id
����ֵ�� void;    */
void ClusterAnalysis::KeyPointCluster(unsigned long dpID, unsigned long clusterId )
{
    DataPoint& srcDp = dadaSets[dpID];        //��ȡ���ݵ����
    if(!srcDp.IsKey())    return;
    vector<unsigned long>& arrvalPoints = srcDp.GetArrivalPoints();        //��ȡ���������ڵ�ID�б�
    for(unsigned long i=0; i<arrvalPoints.size(); i++)
    {
        DataPoint& desDp = dadaSets[arrvalPoints[i]];    //��ȡ�����ڵ����ݵ�
        if(!desDp.isVisited())                            //���ö���û�б����ʹ�ִ��
        {
            //cout << "���ݵ�\t"<< desDp.GetDpId()<<"����IDΪ\t" <<clusterId << endl;
            desDp.SetClusterId(clusterId);        //���øö��������ص�IDΪclusterId�������ö����������
            desDp.SetVisited(true);                //���øö����ѱ�����
            if(desDp.IsKey())                    //���ö����Ǻ��Ķ���
            {
                KeyPointCluster(desDp.GetDpId(),clusterId);    //�ݹ�ضԸ���������ݵ������ڵĵ�ִ�о������������������ȷ���
            }
        }
    }
}

//�����ݵ�֮�����
/*
��������ȡ�����ݵ�֮�����
˵������ȡ�����ݵ�֮���ŷʽ����
������
DataPoint& dp1;        //���ݵ�1
DataPoint& dp2;        //���ݵ�2
����ֵ�� double;    //����֮��ľ���        */
double ClusterAnalysis::GetDistance(DataPoint& dp1, DataPoint& dp2)
{
    double distance =0;        //��ʼ������Ϊ0
    for(int i=0; i<DIME_NUM;i++)    //������ÿһά����ִ��
    {
        distance += pow(dp1.GetDimension()[i] - dp2.GetDimension()[i],2);    //����+ÿһά���ƽ��
    }
    return pow(distance,0.5);        //���������ؾ���
}



int main()
{
    ClusterAnalysis myClusterAnalysis;                        //�����㷨��������
    myClusterAnalysis.Init("out.txt",8,8);        //�㷨��ʼ��������ָ���뾶Ϊ15����������С���ݵ����Ϊ3�����ڳ�������ָ������ά��Ϊ2��
    myClusterAnalysis.DoDBSCANRecursive();                    //ִ�о����㷨
    myClusterAnalysis.WriteToFile("outResult.txt");//дִ�к�Ľ��д���ļ�

    system("pause");    //��ʾ���
    return 0;            //����
}