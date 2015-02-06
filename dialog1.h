#ifndef DIALOG1_H
#define DIALOG1_H

#include <QDialog>

namespace Ui {
class Dialog1;
}

class Dialog1 : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog1(QWidget *parent = 0);
    ~Dialog1();
    QString user_input;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog1 *ui;
};

#endif // DIALOG1_H
