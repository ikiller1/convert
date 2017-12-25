#include<QtWidgets>
#include "convertdialog.h"
#include<QThread>
CDialog::CDialog(QWidget *parent)
    : QDialog(parent)
{

    setupUi(this);
    checkBox->setVisible(false);
    timeEdit_2->setEnabled(false);
    timeEdit_3->setEnabled(false);
    QPushButton *convertButton =
            buttonBox->button(QDialogButtonBox::Apply);
    convertButton->setText(tr("-> MP4"));
    convertButton->setEnabled(false);

    connect(convertButton, SIGNAL(clicked()),
            this, SLOT(convert()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(&process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(updateOutputTextEdit()));
    connect(&process, SIGNAL(readyReadStandardError()),
            this, SLOT(updateOutputTextEdit()));

    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(textBrowser,SIGNAL(textChanged()),this,SLOT(autoScroll()));
    connect(stopbutton,SIGNAL(clicked()),this,SLOT(stopconvert()));
    connect(tosplit,SIGNAL(clicked()),this,SLOT(runsplit()));
    connect(helpbutton,SIGNAL(clicked(bool)),this,SLOT(help()));
    textBrowser->setReadOnly(true);

    connect(playbutton1,SIGNAL(clicked(bool)),this,SLOT(playstart()));
    connect(playbutton2,SIGNAL(clicked(bool)),this,SLOT(playend()));
    connect(&player, SIGNAL(readyReadStandardOutput()),
            this, SLOT(updateOutputTextEdit()));
    connect(&player, SIGNAL(readyReadStandardError()),
            this, SLOT(updateOutputTextEdit()));
    videowindow->setAutoFillBackground(true);
}

void CDialog::on_browse_clicked()
{
    QString initialName = path->text();
    if (initialName.isEmpty())
    {
        initialName = QDir::homePath();
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),initialName);
    fileName = QDir::toNativeSeparators(fileName);
    QPushButton *convertButton = buttonBox->button(QDialogButtonBox::Apply);
    if (!fileName.isEmpty())
    {
        path->setText(fileName);
        update();
        buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    }
    else
    {
        convertButton->setEnabled(false);
        QMessageBox messageBox;//=new QMessageBox();
       messageBox.setWindowTitle("error");
       messageBox.setText("文件路径为空");
       messageBox.exec();
        return;
    }
    this->sourceFile = path->text();
    int dotIndex=sourceFile.lastIndexOf(".");
    this->mp4File = QString(sourceFile).replace(dotIndex, this->sourceFile.size() - dotIndex,".mp4");

    if(true == QFile::exists(this->mp4File))
    {
        convertButton->setEnabled(false);
        QMessageBox messageBox;//=new QMessageBox();
       messageBox.setWindowTitle("error");
       messageBox.setText("同名文件已经存在");
       messageBox.exec();
    }
    else
    {
        convertButton->setEnabled(true);
    }
    textBrowser->setPlainText(this->sourceFile + "\n" + this->mp4File);
}

void CDialog::convert()
{

    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    textBrowser->clear();
    QStringList args;
    //"ffmpeg  -y -t 30 -i e:\YUAN.mkv  -vf subtitles='e\:\\YUAN.mkv'
    //-vcodec libx264 -s 640x480 -b:v 600k e:\fv.mp4"
    args.clear();
    if(overwrite1->checkState() == Qt::Checked)
    {
        args<<"-y";
    }

    if(test->checkState()==Qt::Checked)
    {
        args<<"-t"<<"20";
    }
    args<<"-i";
    args<<sourceFile;

    if(stoh->checkState()==Qt::Checked)
    {
        args<<"-vf";
        QString t=sourceFile;
        t.replace("\\","\\\\");
        t.insert(1,QString("\\"));
        args<<(QString("subtitles='")+t+QString("'"));
    }
    args<<"-vcodec"<<"libx264"<<"-s";
    args<<comboBox->currentText();// args<<"640x480";
    args<<"-b:v";
    args<<QString("%1").arg(bitrate->value())+QString("k");//args<<"500k";
    args<<mp4File;

    process.start("./ffmpeg.exe",args);
    textBrowser->setPlainText(sourceFile+mp4File);

    tosplit->setEnabled(false);
}

void CDialog::updateOutputTextEdit()
{
    QByteArray newData = process.readAllStandardOutput();
    QByteArray newData1 = process.readAllStandardError();
    QString text = textBrowser->toPlainText()
            + QString::fromLocal8Bit(newData)
            + QString::fromLocal8Bit(newData1);
    newData = player.readAllStandardOutput();
    newData1 = player.readAllStandardError();
    text = text
            + QString::fromLocal8Bit(newData)
            + QString::fromLocal8Bit(newData1);
    textBrowser->setPlainText(text);
    // textBrowser->scrollContentsBy(0,20);
    //textBrowser->scroll(0,20);
    //textBrowser->setPlainText("ehloof");

}

void CDialog::autoScroll()
{
    QTextCursor cursor=textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    textBrowser->setTextCursor(cursor);
}

void CDialog::stopconvert()
{
    process.kill();
    player.kill();
    //textBrowser->appendPlainText(args.join(" "));
    //process.start("ffmpeg");
}

void CDialog::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        //textBrowser->append(tr("Conversion program crashed"));
        textBrowser->appendPlainText(QString("Conversion program crashed"));
    }
    else if (exitCode != 0) {
        textBrowser->appendPlainText(QString(tr("Conversion failed")));
    }
    else {
        textBrowser->appendPlainText(QString(tr("File %1 created").arg(mp4File)));
        tosplit->setEnabled(true);
    }
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    tosplit->setEnabled(true);
    //stopbutton->setEnabled(false);
}

void CDialog::runsplit()
{
    static long i=-1;
    ++i;
    QString mp4File = this->mp4File;
    //if(checkBox->checkState()==Qt::Checked)
    {
        if(true==QFile::exists(mp4File))
        {
            //ffmpeg  -y  -ss 01:30 -t 30 -i fv.mp4   fv1.mp4
            //ffmpeg -y -i fv1.mp4 -vn -ar 44100  -f mp3 fv1.mp3
            //if(!QFile(mp4File).exists())return;
            int dotIndex=mp4File.lastIndexOf(".");
            QStringList args;
            QString time;
            args.clear();
            if(timeEdit_0->time()<timeEdit_1->time())
            {
                if(overwrite2->checkState()==Qt::Checked)
                {
                    args<<"-y";
                }
                args<<"-ss"<<timeEdit_0->time().toString("hh:mm:ss");
                args<<"-t"<<time.setNum(timeEdit_0->time().secsTo(timeEdit_1->time()),10);
                args<<"-i"<<mp4File;
                //args<<((timeEdit_0->time().toString("hh:mm:ss"))+"to"+(timeEdit_1->time().toString("hh:mm:ss"))+".mp4");
                QString output_mp4_name=mp4File.insert(dotIndex,
                                                       QString().setNum(timeEdit_0->time().hour())
                                                       +QString().setNum(timeEdit_0->time().minute())
                                                       +QString().setNum(timeEdit_0->time().second())
                                                       +"_"
                                                       +QString().setNum(timeEdit_1->time().hour())
                                                       +QString().setNum(timeEdit_1->time().minute())
                                                       +QString().setNum(timeEdit_1->time().second())
                                                       ) ;
                args<<output_mp4_name;
                QString output_mp3_name=output_mp4_name;
                int dotIndex=output_mp3_name.lastIndexOf(".");
                output_mp3_name = output_mp3_name.replace(dotIndex,output_mp3_name.size()-dotIndex,".mp3");

                args<<"-vn"<<"-ar"<<"44100"<<"-f"<<"mp3"<<output_mp3_name;
                textBrowser->appendPlainText(args.join(" "));
                process.start("ffmpeg",args);
                tosplit->setEnabled(false);
            }
        }
        else
        {
            QMessageBox messageBox;//=new QMessageBox();
           messageBox.setWindowTitle("error");
           messageBox.setText("源文件" + mp4File + "不存在");
           messageBox.exec();
        }
    }

}
void CDialog::help()
{
    m_Box.setWindowTitle("Help");
    m_Box.setDefaultButton(QMessageBox::Ok);
    m_Box.setIcon(QMessageBox::Information);
    m_Box.setText(QString("使用方法：\n1.browse选择mkv格式的文件，若存在软字幕，则勾选\"soft sub to...\"选项，\n点击\"->mp4\"按钮。\n"
                         )+QString("2.输入合适时间段，点击\"to split\"，即可生成相应的mp4,mp3文件。\n"
                                   )+QString("\n(文件默认生成目录为源文件目录，名称为源文件名后加上序号)。\n若已经是MP4文件，即可在第一步中直接选择MP4文件，然后直接进入第二步。\n勾选overwrite即可覆盖已存在文件。\n"
                                             )+QString("Test勾选生成20秒的视频用于测试效果。\n遇到其他问题请查看窗口中部的信息输出栏。\n\n"
                                                       )+QString("\nThanks for your using !       2016.5.3")
                 );
    m_Box.setModal(false);
    m_Box.move(this->pos()+QPoint(this->width()/2,this->height()/3));

    m_Box.show();
}

void CDialog::playstart()
{
    player.kill();
    player.waitForFinished(1000);
    QStringList args;
    args << tr("-slave");
    args << "-quiet";
    args << "-zoom";
    //args<<"-ss"<<"100";
    args<<"-ss"<<timeEdit_0->time().toString("hh:mm:ss");
    //args<<"-sid"<<"0";
    //args << tr("-wid") << QString::number(renderTarget->winId());
    args << tr("-wid") << QString::number(uint(videowindow->winId()));
    args << path->text();//fileName;//tr("/home/hdc/视频/02-112009中国香港动作大片风云2 风云2DVD-RMVB国语中字 1024x434高清晰 无水印版.rmvb");
    //player.start("./mplayer", args);
    player.start("mplayer",args);
}
void CDialog::playend()
{
    player.kill();
    player.waitForFinished(1000);
    QStringList args;
    args << tr("-slave");
    args << "-quiet";
    args << "-zoom";
    //args<<"-ss"<<"100";
    args<<"-ss"<<timeEdit_1->time().toString("hh:mm:ss");
    //args<<"-sid"<<"0";
    //args << tr("-wid") << QString::number(renderTarget->winId());
    args << tr("-wid") << QString::number(uint(videowindow->winId()));
    args << path->text();
    //player.start("./mplayer", args);
    player.start("mplayer",args);
}
