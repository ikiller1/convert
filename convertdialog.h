#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H


#include <QDialog>
#include <QProcess>

#include "ui_convert.h"

class CDialog : public QDialog, private Ui::ConvertDialog
{
    Q_OBJECT

public:
    CDialog(QWidget *parent = 0);

private slots:
    void on_browse_clicked();
    void convert();
    void updateOutputTextEdit();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  //  void processError(QProcess::ProcessError error);
    void autoScroll();
    void stopconvert();
    void runsplit();
    void help();
    void playstart();
    void playend();

private:
    QProcess process;
    QProcess player;
    QString mp4File;
    QString sourceFile;
    bool tomp4;
};


#endif // CONVERTDIALOG_H
