#ifndef MAPDATA_H
#define MAPDATA_H
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonParseError>
#include<QJsonObject>
#include<QFile>
#include<QString>
#include <QJsonObject>
#include<QVector>
#include"sqlhandle.h"
#include<unordered_map>
#include<QFile>
#include"tile.h"
class mapdata{
public:
    mapdata(int ID=1);
    ~mapdata()=default;
    int width()const{return Width;};
    int height()const{return Height;};
    int tileWidth()const{return TileWidth;};
    int tileHeight()const{return TileHeight;};
    int Firstgid()const{return firstgid;};
    tile getTile(int Id){ return Tile.at(Id); } // keep original (may throw if absent)
    // Safe helpers:
    bool hasTile(int Id) const { return Tile.find(Id) != Tile.end(); }
    tile getTileSafe(int Id) const {
        auto it = Tile.find(Id);
        if (it != Tile.end()) return it->second;
        return tile(); // default empty tile
    }

    QVector<QVector<QVector<int>>> MapLayer;
    QVector<QVector<int>> RealMap;
private:
    sqlhandle SQLOpen;
    int firstgid = 1;
    int Width=0,Height=0;
    int TileWidth=0,TileHeight=0;
    std::unordered_map<int,tile> Tile;
    QFile JsonFile;
    QJsonDocument JsonMap;
    QJsonParseError JsonErr;
};

#endif // MAPDATA_H
