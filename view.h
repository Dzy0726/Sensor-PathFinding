#ifndef VIEW_H
#define VIEW_H

#include <QMainWindow>
#include <Sensor.h>

QT_BEGIN_NAMESPACE
namespace Ui { class View; }
QT_END_NAMESPACE

class View : public QMainWindow
{
    Q_OBJECT

public:
    View(QWidget *parent = nullptr);
    ~View();

    Sensor sensor;//model创建

private slots:
    void on_findPath_clicked();

private:
    Ui::View *ui;

    int numTransByCoordinateX(int coordinateX);//根据横坐标确定行号
    int numTransByCoordinateY(int coordinateY);//根据纵坐标确定列号
    void initMap(); //初始化传感器分布图

    void printInfo();//输出所有参数 便与调试
    void printVex(bool);//输出邻接矩阵

    void initNodeDis();//初始化距离邻接矩阵
    void initNodeTime();//初始化时间邻接矩阵
    Node findSensor(int No);//根据传感器节点的编号在vector中查找并返回此节点
    int findSensor(int x,int y);//根据传感器节点的坐标在vector中查找并返回此节点序号
    double getSensorsDis(Node nodeI,Node nodeJ);//计算两个传感器节点之间的距离
    int getSensorsTime(Node nodeI,Node nodeJ);//计算两个传感器节点之间的时间

    void shortestPath(int start,QVector<QVector<double>>);//贪心算法距离最短
    void shortestTime(int start,QVector<QVector<int>>);//贪心算法时间最短
    void findPath();//寻找最短路径
    void showPath();//输出路径

    double getMemory();//获取内存
};
#endif // VIEW_H
