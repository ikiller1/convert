#include <QApplication>
#include<QMessageBox>
#include "convertdialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if(false==QFile::exists("ffmpeg.exe")){
         QMessageBox messageBox;
        messageBox.setWindowTitle("error");
        messageBox.setText("ffmpeg.exe不在该目录下！无法正常运行，点击确定退出程序");
        messageBox.exec();

        app.closeAllWindows();
        app.quit();return 0;
        }
    if(false==QFile::exists("mplayer.exe")){
         QMessageBox messageBox;//=new QMessageBox();
        messageBox.setWindowTitle("error");
        messageBox.setText("mplayer.exe不在该目录下！无法正常运行，点击确定退出程序");
        messageBox.exec();

        app.closeAllWindows();
        app.quit();return 0;
        }
    CDialog dialog;
    dialog.show();

    app.setStyleSheet("QDialog#ConvertDialog {background-image: url(:/images/background.png)}");
                     // "QPushButton[text=\"-> MP4\"]{background-color: rgb(160,155,73)}"
                     // "QPushButton[text=\"to split\"]{background-color: rgb(32,196,255)}");
    //app.setStyleSheet();
    return app.exec();
}
