#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListView>
#include <QtSql>
#include <QMessageBox>
#include <QObject>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDesktopServices>
// TODO
/*
 * datum nastanka vnosa : DONE
 * prijava na sazas : DONE
 * link do sazasove prijave :DONE,needs manual entry
 * avtor besedila, melodije, priredbe :DONE,needs manual entry
 * razvrščanje po avtorju :DONE
 * show lyrics with MS word : DONE
 *
 *
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pixmap;
    pixmap.load("F:/play.png");
    QIcon icon;
    icon.addPixmap(pixmap);
    ui->playbutton->setIcon(icon);
    QPixmap pausemap;
    pausemap.load("F:/pause.png");
    QIcon pauseicon;
    pauseicon.addPixmap(pausemap);
    ui->pausebutton->setIcon(pauseicon);
    QPixmap stopmap;
    stopmap.load("F:/stop.png");
    QIcon stopicon;
    stopicon.addPixmap(stopmap);
    ui->stopbutton->setIcon(stopicon);
    MainWindow::setdatabase();
    QPalette pallete;
    pallete.setColor(ui->mainToolBar->backgroundRole(),Qt::white);
    ui->mainToolBar->setPalette(pallete);
    QModelIndex index = table->index(0,1);
    QObject::connect(ui->actionEditmode,SIGNAL(toggled(bool)),this,SLOT(on_edit_clicked(bool)));
    QObject::connect(ui->tableView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(on_celldata_clicked(const QModelIndex &)));

    QObject::connect(ui->actionaddsongs,SIGNAL(triggered(bool)),this,SLOT(on_addsongs_clicked()));
    QObject::connect(ui->playbutton,SIGNAL(clicked(bool)),this,SLOT(on_playsong_clicked()));
    QObject::connect(ui->pausebutton,SIGNAL(toggled(bool)),this,SLOT(on_pause_clicked(bool)));
    QObject::connect(ui->stopbutton,SIGNAL(clicked(bool)),this,SLOT(on_stopsong_clicked()));
    QObject::connect(ui->actionShow_all_entries,SIGNAL(clicked(bool)),this,SLOT(on_reset_filter_clicked()));
    QObject::connect(ui->actionShow_hidden_columns,SIGNAL(toggled(bool)),this,SLOT(on_togglecolumns_toggled(bool)));
    QObject::connect(ui->actionFindallsongs,SIGNAL(triggered(bool)),this,SLOT(on_findsongs_clicked()));
    QObject::connect(ui->actionDeletesongs,SIGNAL(triggered(bool)),this,SLOT(on_deleterecord_clicked()));
    ui->tableView->hideColumn(6);
    ui->tableView->hideColumn(7);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setdatabase(){
    db=QSqlDatabase::addDatabase("QSQLITE");
    QString currentpath = QCoreApplication::applicationDirPath();
    db.setDatabaseName(currentpath + "/database.sqlite3");
    db.open();

    if(!db.open()){
        QMessageBox::warning(this,"error","failed to connect to database");
    }
    else{
        table = new QSqlTableModel(this);
        table->setTable("songs");
        table->select();
        ui->tableView->setModel(table);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->show();
    }
}

void MainWindow::on_addsongs_clicked()
{
    QProcess *process = new QProcess(this);
    QString file ="addmusictosql.exe";
    process->setProgram(file);
    process->start();
    process->waitForFinished();
    table->select();
    ui->tableView->setModel(table);
    ui->tableView->show();

}
void MainWindow::on_deleterecord_clicked(){
    int rows = ui->tableView->currentIndex().row();
    QString rows_1= QString::number(rows);
    table->removeRow(rows);
    table->submitAll();
    table->select();
    ui->tableView->setModel(table);
    ui->tableView->show();
}
void MainWindow::on_findsongs_clicked(){
    //get the path of the directory
    findsongs = QFileDialog::getExistingDirectory(this,"find dir","C:");
    QDir dir(findsongs);
    //set a filter
    dir.setNameFilters(QStringList() << "*.mp3");
    //scan the directory
    allfiles = dir.entryList();
    allcount =allfiles.count();

    //make a new widget window to confirm selected files if findsongs and allcount are not empty
    if(findsongs != NULL && allcount != NULL){
        widgwin = new QWidget();
        //resize to wanted resolution
        widgwin->resize(400,widgwin->height());
        widgwin->resize(800,widgwin->width());
        //make a listwidget where the files will be shown
        view = new QListWidget(widgwin);
        view->resize(800,350);
        view->addItems(allfiles);
        //make 2 buttons to confirm or cancel selection
        yesmulti = new QPushButton(widgwin);
        cancelmulti = new QPushButton(widgwin);
        yesmulti->setText("OK");
        cancelmulti->setText("Cancel");
        cancelmulti->setGeometry(470,350,72,30);
        yesmulti->setGeometry(400,350,72,30);
        label = new QLabel(widgwin);
        label->setText("Do you wish to insert these files into the database?");
        label->setGeometry(50,350,300,30);
        QFont font = label->font();
        font.setPointSize(10);
        label->setFont(font);
        QObject::connect(cancelmulti,SIGNAL(clicked()),widgwin,SLOT(close()));
        QObject::connect(yesmulti,SIGNAL(clicked(bool)),this,SLOT(on_yesmulti_clicked()));
        widgwin->show();

    }


}
void MainWindow::on_yesmulti_clicked(){
    QSqlQuery query;
    QDate date = QDate::currentDate();
    for(n=0;n<allcount;n++){
        QFile textfile = findsongs + "/" +allfiles[n];
        QString textstring= findsongs + "/" + allfiles[n];
        QFileInfo textinfo(textfile.fileName());
        QFile textlyrics = findsongs + "/" + textinfo.baseName() + ".docx";
        QString lyricspath = findsongs + "/" + textinfo.baseName() + ".docx";
        query.prepare("INSERT INTO songs(songname) values(:songname);");
        query.bindValue(":songname",allfiles[n]);
        query.exec();
        query.prepare("Update songs SET filepath = :path WHERE songname = :song;");
        query.bindValue(":path",textstring);
        query.bindValue(":song",allfiles[n]);
        query.exec();
        query.prepare("UPDATE songs set lyricspath = :lyricspath WHERE songname = :namesong;");
        query.bindValue(":lyricspath",lyricspath);
        query.bindValue(":namesong",allfiles[n]);
        query.exec();
        query.prepare("UPDATE songs set Date = :date WHERE songname = :songnamesong;");
        query.bindValue(":date",date);
        query.bindValue(":songnamesong",allfiles[n]);
        query.exec();
        query.prepare("UPDATE songs set Sazas = :sazas WHERE songname = :namesongsong;");
        query.bindValue(":sazas","No");
        query.bindValue(":namesongsong",allfiles[n]);
        query.exec();
    }
    QFile textfile = findsongs + "/" +allfiles[n-1];
    QString textstring= findsongs + "/" + allfiles[n-1];
    QFileInfo textinfo(textfile.fileName());
    QFile textlyrics = findsongs + "/" + textinfo.baseName() + ".txt";

    if(!textlyrics.open(QIODevice::ReadOnly)){
        QMessageBox::warning(widgwin,"error",textlyrics.errorString());
    }
    else{
        QTextStream in(&textlyrics);
        while(!in.atEnd()){
            QString line = in.readLine();
            query.prepare("UPDATE songs SET lyrics = :lyrics WHERE songname = :name;");
            query.bindValue(":lyrics",line);
            for(n=0;n<allcount;n++){
                query.bindValue(":name",allfiles[n]);
            }
            query.exec();

        }
    }
    query.prepare("select * from songs;");
    query.exec();
    table->submitAll();
    table->select();
    ui->tableView->setModel(table);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->show();
    widgwin->close();
}
void MainWindow::on_playsong_clicked()
{
    int rows = ui->tableView->currentIndex().row();
    /*pull data from tablecell lyricspath to insert into QTextStream
     * for reading from a files
     */
    QModelIndex index = table->index(rows,6);
    QFile lyricspathcell = table->data(index).toString();
    QString lyricspathsstring =table->data(index).toString();
    QWidget *lyrics  = new QWidget();

    if(!lyricspathcell.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"error",lyricspathcell.errorString());
    }
    else{
        QTextBrowser *lyricsdisplay = new QTextBrowser(lyrics);
        lyricsdisplay->resize(800,600);
        QTextStream in(&lyricspathcell);
        while(!in.atEnd()){
            linei = in.readLine();
            list.append(linei);
            lyricsdisplay->append(linei);

        }
        lyrics->resize(800,600);
        lyrics->show();
    }
    player = new QMediaPlayer(this);
    QModelIndex song=table->index(rows,7);
    QString datacell = table->data(song).toString();
    if(player->isAvailable()){
        player->setMedia(QUrl::fromLocalFile(datacell));
    }
    player->play();
    QObject::connect(player,SIGNAL(durationChanged(qint64)),this, SLOT(MediaStatusChanged()));
    QObject::connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(SliderChanged(qint64)));
    QObject::connect(ui->slider,SIGNAL(sliderMoved(int)),this,SLOT(setsongdur(int)));

    /*if(player->isMetaDataAvailable()){
        QMessageBox::information(this,"error","available");
    }
    else{
        QMessageBox::information(this,"error","not available");
        //durationhi = player->metaData(QMediaMetaData::Duration).toInt();
    }
    int durationhi = player->metaData(QMediaMetaData::Duration).toInt();
    int seconds = durationhi/1000;
    int remainder = seconds % 60;
    int minutes = seconds/60;
    QString halou = QString::number(minutes) +"min" +" " + QString::number(remainder)+"sec";
    QMessageBox::information(this,"duration",halou);*/

}
void MainWindow::on_stopsong_clicked(){
    player->stop();
}
void MainWindow::on_pause_clicked(bool checked){
    if(checked == true){
        player->pause();
    }
    else{
        player->play();
    }

}
void MainWindow::on_celldata_clicked(const QModelIndex &ind){

    /*
     * old obsolete way of displaying lyrics. Couldn't edit but didn't rely on other software.
     * may come handy for linux if i develop for linux later on :)
     QModelIndex data = table->index(row,6);
     QFile lyricspathcell = table->data(data).toString();
     QModelIndex showlyrics = table->index(row,3);
     QModelIndex authorsort =table->index(row,8);

     if(ind == showlyrics){
        if(!lyricspathcell.open(QIODevice::ReadOnly)){
            QMessageBox::warning(this,"error",lyricspathcell.errorString());
        }
        else{
            QWidget *lyrics =new QWidget();
            lyrics->resize(800,600);
            QTextBrowser *browser = new QTextBrowser(lyrics);
            browser->resize(800,600);
            QTextStream in(&lyricspathcell);
            while(!in.atEnd()){
                lyricsline = in.readLine();
                browser->append(lyricsline);
            }
            lyrics->show();
        }   

    }
    else if(ind == authorsort){
         QModelIndex getauthor = ind;
         QString authorstring = table->data(getauthor).toString();
         QString filter1 = "lyrics_author = '"+authorstring+ "'";
         table->setFilter(filter1);
         ui->tableView->setModel(table);
         ui->tableView->show();
    }
    */
    /*
     * may come in handy someday. Slow af but probably can be improved. Inside Qt, will use full word for now.
    QWidget *worddoc = new QWidget();
    QAxWidget* word = new QAxWidget("Word.Application",worddoc);
    word->setGeometry(0,0,300,300);
    word->setControl("F:/read_dis.docx");
    word->show();
    worddoc->show();
    */
    int row = ui->tableView->currentIndex().row();
    QModelIndex index = table->index(row,3);
    QModelIndex authorsort =table->index(row,8);
    QModelIndex lyricspath = table->index(row,6);
    QString lyrics = table->data(lyricspath).toString();
    if(ind == index){
        QDesktopServices::openUrl(QUrl::fromLocalFile(lyrics));
    }
    else if(ind == authorsort){
         QModelIndex getauthor = ind;
         QString authorstring = table->data(getauthor).toString();
         QString filter1 = "lyrics_author = '"+authorstring+ "'";
         table->setFilter(filter1);
         ui->tableView->setModel(table);
         ui->tableView->show();
    }

}
void MainWindow::MediaStatusChanged()
{
    durationhi = player->duration();
    ui->slider->setRange(0,durationhi / 1000);
    seconds = durationhi/1000;
    int remainder = seconds % 60;
    int minutes = seconds/60;
    //QMessageBox::information(this,"error",s.LoadedMedia);
    ui->length->setText(QString::number(minutes) +"min "+ QString::number(remainder)+"sec");
    //QMessageBox::information(this,"duration",halou);


}
void MainWindow::SliderChanged(qint64 time){
    ui->slider->setValue(time / 1000);
}
void MainWindow::setsongdur(int value){
    player->setPosition(value * 1000);
}
void MainWindow::on_reset_filter_clicked(){
    table->setFilter(NULL);
}
void MainWindow::on_edit_clicked(bool toggled){

    if(toggled == false){
        QObject::connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(on_tablecell_clicked()));
        QObject::connect(ui->tableView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(on_celldata_clicked(const QModelIndex &)));
    }
    else{
        ui->tableView->disconnect();
    }
}
void MainWindow::on_togglecolumns_toggled(bool toggled)
{
    if(toggled == true){
        ui->tableView->showColumn(6);
        ui->tableView->showColumn(7);
    }
    else{
        ui->tableView->hideColumn(6);
        ui->tableView->hideColumn(7);
    }
}
