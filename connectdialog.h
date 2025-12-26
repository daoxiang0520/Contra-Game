#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H
#include<QDialog>
#include<QVBoxLayout>
#include<QPushButton>
#include<QLabel>
class ConnectDialog:public QDialog
{
    Q_OBJECT
public:
    ConnectDialog(QWidget* parent=nullptr);
    void buttonclose();
    void setLabelText();
    void setLabel(const QString& t);
    void setButton(const QString& t);
    void recover();
    QLabel * label;
    QPushButton* button;
};

#endif // CONNECTDIALOG_H
