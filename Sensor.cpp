#include "Sensor.h"
#include "QProcess"

Sensor::Sensor()
{
    initLTN();
    initNTL();
}

void Sensor::initLTN() {
    letterToNum.insert("A",1);
    letterToNum.insert("B",2);
    letterToNum.insert("C",3);
    letterToNum.insert("D",4);
    letterToNum.insert("E",5);
    letterToNum.insert("F",6);
    letterToNum.insert("G",7);
    letterToNum.insert("H",8);
}


void Sensor::initNTL() {
    numToLetter.insert(1,"A");
    numToLetter.insert(2,"B");
    numToLetter.insert(3,"C");
    numToLetter.insert(4,"D");
    numToLetter.insert(5,"E");
    numToLetter.insert(6,"F");
    numToLetter.insert(7,"G");
    numToLetter.insert(8,"H");
}

double Sensor::getUsedMemory(DWORD pid)
{
    char pidChar[25];
    //将DWORD类型转换为10进制的char*类型
    _ultoa(pid,pidChar,10);

    //调用外部命令
    QProcess p;
    p.start("tasklist /FI \"PID EQ " + QString(pidChar) + " \"");
    p.waitForFinished();
    //得到返回结果
    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    //关闭外部命令
    p.close();

    //替换掉","
    result = result.replace(",","");
    //匹配 '数字+空格+K'部分。
    QRegExp rx("(\\d+)(\\s)(K)");
    //初始化结果
    QString usedMem("");
    if(rx.indexIn(result) != -1){
        //匹配成功
        usedMem = rx.cap(0);
    }
    //截取K前面的字符串，转换为数字，供换算单位使用。
    usedMem = usedMem.left(usedMem.length() - 1);
    //换算为MB的单位
    return usedMem.toDouble() / 1024;
}
