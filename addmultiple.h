#ifndef ADDMULTIPLE_H
#define ADDMULTIPLE_H

#include <QWidget>

namespace Ui {
class addmultiple;
}

class addmultiple : public QWidget
{
    Q_OBJECT

public:
    explicit addmultiple(QWidget *parent = 0);
    ~addmultiple();

private:
    Ui::addmultiple *ui;
};

#endif // ADDMULTIPLE_H
