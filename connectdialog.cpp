#include "connectdialog.h"

ConnectDialog::ConnectDialog(QWidget* parent):QDialog(parent) {
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    //创建标签和按钮
    label = new QLabel("连接中……",this);
    button = new QPushButton("取消",this);
    layout->addWidget(label);
    layout->addWidget(button);
    connect(button,&QPushButton::clicked,this,&ConnectDialog::buttonclose);
}
void ConnectDialog::buttonclose()
{
    this->close();
}
void ConnectDialog::setLabelText(){
    label->setText("连接成功");
}void ConnectDialog::setLabel(const QString& t){
    label->setText(t);
}void ConnectDialog::setButton(const QString& t){
    button->setText(t);
}void ConnectDialog::recover(){
    label->setText("连接中……");
    button->setText("取消");
}
