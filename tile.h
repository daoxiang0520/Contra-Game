#ifndef TILE_H
#define TILE_H
#include<QPixmap>
#include<QPainter>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>
#include<QGraphicsItem>
#include<QGraphicsScene>
#include<QGraphicsView>
#include<QString>
#define TILESIZE  32
class tile
{
public:
    tile()=default;
    tile(int Id,QString path,int w,int h,QString Name);
    int Width()const{return width;};
    int Height()const{return height;};
    QString Name()const {return name;};
    const QPixmap& Img(){return img;};
private:
    int id;
    QPixmap img;
    int width,height;
    QString name;
};

#endif // TILE_H
