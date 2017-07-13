#include "addmultiple.h"
#include "ui_addmultiple.h"
#include "mainwindow.h"

addmultiple::addmultiple(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addmultiple)
{
    ui->setupUi(this);
    MainWindow.

}

addmultiple::~addmultiple()
{
    delete ui;
}
