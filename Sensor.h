#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QMap>
#include <windows.h>

struct Node {
    int X;//第几行
    int Y;//第几列
    int No;//节点序号
};

class Sensor
{
public:
    Sensor();
    QVector<Node> sensorNode;//传感器节点存放
    QVector<QVector<double>> nodeDis;//距离邻接矩阵 图的处理
    QVector<QVector<int>> nodeTime;//时间邻接矩阵
    QVector<bool> isVisitied;//是否遍历的数组
    QVector<int> path;//找到的路径
    QVector<int> path1;//找到的路径
    int sensorCount = 0;//传感器数量
    int distance;       //邻居距离
    int speed;          //传感器速度
    QMap<QString,int> letterToNum;
    QMap<int,QString> numToLetter;
    int startX;         //起点横坐标
    int startY;         //起点纵坐标
    int startNo;        //起点序号
    int target;         //最短距离为0，最短时间为1

    double finalDistance = 0;//最短路径的距离
    int finalTime = 0;
    double finalDistance1 = 0;//最短路径的距离
    int finalTime1 = 0;

    double getUsedMemory(DWORD pid);
private:
    void initLTN();
    void initNTL();
};

#endif // MODEL_H
