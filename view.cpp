#include "view.h"
#include "ui_view.h"
#include "QDebug"
#include "QRegExpValidator"
#include "QMessageBox"
#include "QElapsedTimer"
#include <iostream>
#include <math.h>


using namespace std;

View::View(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::View)
{
    ui->setupUi(this);

    ui->distance->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->speed->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->X->setValidator(new QRegExpValidator(QRegExp("[A-H]")));
    ui->Y->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
}

View::~View()
{
    delete ui;
}

int View::numTransByCoordinateX(int coordinateX) {
    return (coordinateX-50)/30 + 1;
}

int View::numTransByCoordinateY(int coordinateY) {
    return (coordinateY-80)/30 + 1;
}

void View::initMap() {
    sensor.sensorCount = 0;
    sensor.sensorNode.clear();
    Node temp;
    QList<QRadioButton*> buttons = ui->sensorMap->findChildren<QRadioButton*>();
    foreach(auto btn, buttons){
        if(btn->isChecked()) {
            sensor.sensorCount++;
            temp.X = numTransByCoordinateY(btn->geometry().y());
            temp.Y = numTransByCoordinateX(btn->geometry().x());
            temp.No = sensor.sensorCount - 1;
            sensor.sensorNode.append(temp);
        }
    }
}

void View::printInfo() {
    qDebug() << "初始化成功！";
    qDebug() << "传感器数量：" << sensor.sensorCount;
    qDebug() << "传感器坐标及序号：";
    foreach(auto node,sensor.sensorNode) {
        qDebug() << "序号：" << node.No << "(" << node.X << "," << node.Y << ")";
    }
    qDebug() << "传感器邻居距离:" << sensor.distance;
    qDebug() << "传感器传输速度:" << sensor.speed;
    qDebug() << "传感器起点序号:" << findSensor(sensor.startX,sensor.startY);
    sensor.startNo = findSensor(sensor.startX,sensor.startY);
    qDebug() << "路线起点坐标:" << sensor.startX << sensor.startY;
    if(sensor.target == 0) {
        qDebug() << "按最短距离寻找路径";
    } else {
        qDebug() << "按最短时间寻找路径";
    }
}

void View::initNodeDis() {
    Node nodeI,nodeJ;
    QVector<QVector<double>> temp(sensor.sensorCount,QVector<double>(sensor.sensorCount));
    for(int i = 0;i < sensor.sensorCount;i++) {
        nodeI = findSensor(i);
        for(int j = 0;j < sensor.sensorCount;j++) {
            nodeJ = findSensor(j);
            temp[i][j] = getSensorsDis(nodeI,nodeJ);
        }
    }
    sensor.nodeDis = temp;
}

void View::initNodeTime() {
    Node nodeI,nodeJ;
    QVector<QVector<int>> temp(sensor.sensorCount,QVector<int>(sensor.sensorCount));
    for(int i = 0;i < sensor.sensorCount;i++) {
        nodeI = findSensor(i);
        for(int j = 0;j < sensor.sensorCount;j++) {
            nodeJ = findSensor(j);
            temp[i][j] = getSensorsTime(nodeI,nodeJ);
        }
    }
    sensor.nodeTime = temp;
}

Node View::findSensor(int No) {
    foreach(auto node,sensor.sensorNode) {
        if(node.No == No) {
           return node;
        }
    }
}

int View::findSensor(int x, int y) {
    foreach(auto node,sensor.sensorNode) {
        if(node.X == x && node.Y == y) {
            return node.No;
        }
    }
    return -1;
}

double View::getSensorsDis(Node nodeI, Node nodeJ) {
    double xGap = abs(nodeI.X-nodeJ.X);
    double yGap = abs(nodeI.Y-nodeJ.Y);
    if(xGap == yGap) {
        return xGap * sqrt(2) * sensor.distance;
    } else {
        return xGap > yGap ? (yGap * sqrt(2) * sensor.distance + (xGap-yGap) * sensor.distance) : (xGap * sqrt(2) * sensor.distance + (yGap-xGap) * sensor.distance);
    }
}

int View::getSensorsTime(Node nodeI, Node nodeJ) {
    int xGap = abs(nodeI.X-nodeJ.X);
    int yGap = abs(nodeI.Y-nodeJ.Y);
    return xGap > yGap ? (xGap * sensor.distance/sensor.speed ) : (yGap * sensor.distance/sensor.speed );
}

void View::printVex(bool isDis) {
    cout << "\t";
    for(int i = 0;i < sensor.sensorCount;i++) {
        cout << i << "\t";
    }
    cout << endl;
    for(int i = 0;i < sensor.sensorCount;i++) {
        cout << i << "\t";
        for(int j = 0;j < sensor.sensorCount;j++) {
            if(isDis) {
                cout << sensor.nodeDis[i][j] << "\t";
            } else {
                cout << sensor.nodeTime[i][j] << "\t";
            }
        }
        cout << endl;
    }
}

void View::shortestPath(int start,QVector<QVector<double>> sensorDis) {
    qDebug() << "路径现有节点数：" << sensor.path.count() << "总结点数：" << sensor.sensorCount;
    if(sensor.path.count() == sensor.sensorCount) {
        return;
    }
    sensor.isVisitied[start] = true;
    sensor.path.append(start);
    double minCost = 1e10;
    int minTime;
    int nextNode;
    for(int i = 0; i < sensor.sensorCount;i++) {
        if(sensorDis[start][i] < minCost && sensorDis[start][i] > 0 && !sensor.isVisitied[i]) {
            minCost = sensorDis[start][i];
            minTime = sensor.nodeTime[start][i];
            nextNode = i;
        }
    }
    if(minCost < 1e9) {
       sensor.finalDistance += minCost;
       sensor.finalTime += minTime;
    }
    qDebug() << "当前节点：" << start << "下一节点：" << nextNode << "距离：" << minCost;
    shortestPath(nextNode,sensorDis);
}

void View::shortestTime(int start,QVector<QVector<int>> sensorDis) {
    qDebug() << "路径现有节点数：" << sensor.path1.count() << "总结点数：" << sensor.sensorCount;
    if(sensor.path1.count() == sensor.sensorCount) {
        return;
    }
    sensor.isVisitied[start] = true;
    sensor.path1.append(start);
    double minCost = 1e10;
    double minDis;
    int nextNode;
    for(int i = 0; i < sensor.sensorCount;i++) {
        if(sensorDis[start][i] < minCost && sensorDis[start][i] > 0 && !sensor.isVisitied[i]) {
            minCost = sensorDis[start][i];
            minDis = sensor.nodeDis[start][i];
            nextNode = i;
        }
    }
    if(minCost < 1e9) {
       sensor.finalDistance1 += minDis;
       sensor.finalTime1 += minCost;
    }
    qDebug() << "当前节点：" << start << "下一节点：" << nextNode << "距离：" << minCost;
    shortestTime(nextNode,sensorDis);
}

void View::findPath() {
    sensor.finalDistance = 0;
    sensor.finalTime = 0;
    sensor.finalDistance1 = 0;
    sensor.finalTime1 = 0;
    QVector<bool> isVisited(sensor.sensorCount,false);
    sensor.isVisitied = isVisited;
    sensor.path.clear();
    shortestPath(findSensor(sensor.startX,sensor.startY),sensor.nodeDis);
    sensor.isVisitied = isVisited;
    sensor.path1.clear();
    shortestTime(findSensor(sensor.startX,sensor.startY),sensor.nodeTime);

}

void View::showPath() {
    if(sensor.target == 0) {
        if(sensor.finalDistance > sensor.finalDistance1) {
            sensor.finalDistance = sensor.finalDistance1;
            sensor.finalTime = sensor.finalTime1;
            sensor.path = sensor.path1;
        }
    } else {
        if(sensor.finalTime1 < sensor.finalTime) {
            sensor.finalDistance = sensor.finalDistance1;
            sensor.finalTime = sensor.finalTime1;
            sensor.path = sensor.path1;
        }
    }

    ui->result->clear();
    QString result;
    int count = 0;
    foreach(auto i,sensor.path) {
        Node node = findSensor(i);
        result = result + "(" + sensor.numToLetter[node.X] + "," + QString::number(node.Y,10) + ")";
        count++;
        if(count < sensor.path.count()) {
            result += ">";
        }
    }
    ui->result->insertPlainText(result);

    ui->finalDistance->setText(QString::number(sensor.finalDistance,10,2) + "m");
    ui->finalTime->setText(QString::number(sensor.finalTime) + "s");
}

double View::getMemory() {
    return sensor.getUsedMemory(GetCurrentProcessId());
}

void View::on_findPath_clicked()
{
    initMap();//从分布图获取每个传感器的横纵坐标
    int distance = ui->distance->text().toInt();//获取传感器之间的距离
    int speed = ui->speed->text().toInt();//获取传感器传输速度
    QString startX = ui->X->text();//获取起点横坐标
    int start_x = sensor.letterToNum[startX];//通过map找到数字表示的坐标
    int startY = ui->Y->text().toInt();//获取起点纵坐标
    int target = 0;//获取最优路径方式 距离/时间
    bool findStart = false;//判断是否找到起点

    //错误判断
    if(distance != 0) {
        sensor.distance = distance;
    } else {
        QMessageBox::critical(NULL, "输入有误", "传感器邻居距离不能为0！", QMessageBox::Ok);
        return;
    }

    if(speed != 0) {
        sensor.speed = speed;
    } else {
        QMessageBox::critical(NULL, "输入有误", "传感器传递速度不能为0！", QMessageBox::Ok);
        return;
    }

    if(startY > 12 || startY < 1) {
        QMessageBox::critical(NULL, "输入有误", "起点纵坐标范围1-12！", QMessageBox::Ok);
        return;
    }


    if(sensor.sensorNode.isEmpty()) {
        QMessageBox::critical(NULL, "输入有误", "尚未选择传感器！", QMessageBox::Ok);
        return;
    }
    foreach(auto node,sensor.sensorNode) {
        if(node.X == start_x && node.Y == startY) {
            sensor.startX = start_x;
            sensor.startY = startY;
            findStart = true;
        }
    }
    if(!findStart) {
        QMessageBox::critical(NULL, "输入有误", "起点不在所选传感器中！", QMessageBox::Ok);
        return;
    }

    if(ui->chooseDistance->isChecked()) {
        target = 0;
    }else {
        target = 1;
    }
    sensor.target = target;

    printInfo();
    QMessageBox::information(NULL,"开始寻找","参数设置成功！\n开始寻找……",QMessageBox::Ok);

    // 构建距离邻接矩阵
    initNodeDis();
    printVex(true);

    // 构建时间邻接矩阵
    initNodeTime();
    printVex(false);


    //寻找最短路径
    QElapsedTimer time;
    time.start();
    findPath();
    qlonglong end = time.elapsed();
    qDebug()<< end;
    ui->time->setText(QString::number(end/2)+"ms");
    showPath();
    QString memory = QString::number(getMemory()/10,10,2);
    memory += "MB";
    ui->memory->setText(memory);
}
