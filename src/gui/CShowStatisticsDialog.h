#ifndef CSHOWSTATISTICSDIALOG_H
#define CSHOWSTATISTICSDIALOG_H

#include "rapidjson/document.h"
#include <QDialog>

namespace Ui {
class CShowStatisticsDialog;
}

class CShowStatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CShowStatisticsDialog(QWidget *parent = 0);
    ~CShowStatisticsDialog();

private:
    void createHtml();
    void convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str);
    Ui::CShowStatisticsDialog *ui;
};

#endif // CSHOWSTATISTICSDIALOG_H
