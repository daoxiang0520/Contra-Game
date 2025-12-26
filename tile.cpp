#include "tile.h"

tile::tile(int Id,QString path,int w,int h,QString Name)
    :id(Id),width(w),height(h),name(Name)
{
    bool test=img.load(path);
    qDebug()<<path<<test;
    //img.fill(Qt::transparent);
}
