#include"mapdata.h"
mapdata::mapdata(int ID)
{
    QByteArray PriData;
    if(JsonFile.open(QIODevice::ReadOnly)) {
        PriData = JsonFile.readAll();
        JsonFile.close();
    }
    // if SQL backend provides JSON, use it
    QByteArray sqlData = SQLOpen.JSONQuery(ID).toUtf8();
    if (!sqlData.isEmpty()) PriData = sqlData;

    JsonMap = QJsonDocument::fromJson(PriData, &JsonErr);
    if (JsonErr.error != QJsonParseError::NoError){
        throw std::runtime_error(JsonErr.errorString().toStdString());
    }
    if (JsonMap.isNull()){
        throw std::runtime_error("NullJson");
    }

    QJsonObject Root = JsonMap.object();
    Width = Root.value("width").toInt();
    Height = Root.value("height").toInt();
    TileWidth = Root.value("tilewidth").toInt();
    TileHeight = Root.value("tileheight").toInt();
    QJsonArray TileSet = Root.value("tilesets").toArray();
    for(const auto& Tile_v:TileSet){
        if(Tile_v.isObject()){
            QJsonObject TileObj = Tile_v.toObject();
            if(TileObj.contains("tiles")){
                QJsonArray TileAttr = TileObj.value("tiles").toArray();
                for(const auto& Attr_v:TileAttr){
                    QJsonObject Attr = Attr_v.toObject();
                    int id = Attr.value("id").toInt();
                    qDebug()<<id;
                    QString image = ":/" + Attr.value("image").toString();
                    int iw = Attr.contains("imagewidth")?Attr.value("imagewidth").toInt():0;
                    int ih = Attr.contains("imagewidth")?Attr.value("imageheight").toInt():0;
                    QString type = Attr.value("type").toString();
                    Tile.emplace(id, tile(id, image, iw, ih, type));
                }
            }
            if(TileObj.contains("firstgid")) firstgid = TileObj.value("firstgid").toInt();
        }
    }

    // parse layers
    QJsonArray Layer = Root.value("layers").toArray();
    for(const auto& LayerValue:Layer){
        if(LayerValue.isObject()){
            QJsonObject Layer_v = LayerValue.toObject();
            if(Layer_v.contains("data")){
                QJsonArray datas = Layer_v.value("data").toArray();
                int y = 0;
                QVector<int> tmp;
                QVector<QVector<int>> TMP;
                for(const auto& DataTile:datas){
                    tmp.push_back(DataTile.toInt());
                    y++;
                    if(y >= Width){ TMP.push_back(tmp); tmp.clear(); y = 0; }
                }
                MapLayer.push_back(TMP);
                if(Layer_v.contains("name") && Layer_v.value("name").toString() == "Map"){
                    RealMap = TMP;
                }
                TMP.clear();
            }
        }
    }

    // Debug prints
    qDebug() << "mapdata loaded: width,height =" << Width << Height
             << " tileset firstgid=" << firstgid
             << " MapLayer layers=" << MapLayer.size()
             << " RealMap rows=" << RealMap.size();
}
