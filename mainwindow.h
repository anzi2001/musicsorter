#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QListWidget>
#include <QDir>
#include <QMainWindow>
#include <QtSql>
#include <QWidget>
#include <QPushButton>
#include <QWidget>
#include <QTextBrowser>
#include <QLabel>
#include <QMediaPlayer>
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void setdatabase();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_addsongs_clicked();
    void on_deleterecord_clicked();
    void on_findsongs_clicked();
    void on_yesmulti_clicked();
    void on_playsong_clicked();
    void on_stopsong_clicked();
    void on_celldata_clicked(const QModelIndex &ind);
    void MediaStatusChanged();
    void SliderChanged(qint64 time);
    void setsongdur(int value);
    void on_pause_clicked(bool checked);
    void on_reset_filter_clicked();
    void on_edit_clicked(bool toggled);
    void on_togglecolumns_toggled(bool toggled);

private:
    Ui::MainWindow *ui;
    QSqlQueryModel *model;
    QSqlTableModel *table;
    QSqlDatabase db;
    QWidget *widgwin;
    QPushButton *yesmulti;
    QPushButton *cancelmulti;
    QListWidget *view;
    QStringList allfiles;
    int allcount;
    int n;
    QTextBrowser *textbrowser;
    QLabel *label;
    QString findsongs;
    QMediaPlayer *player;
    QStringList songlist;
    QMediaPlaylist *playlist;
    QString linei;
    QStringList list;
    QString lyricsline;
    QStringList listlyrics;
    int durationhi;
    QMediaPlayer *hi;
    int seconds;
    QDate date;


};

#endif // MAINWINDOW_H
