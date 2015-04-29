#ifndef SETTINGSINTERFACE
#define SETTINGSINTERFACE

#include <QObject>
#include <QSettings>
#include <QLineEdit>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>

/*
 * QSettingsInterface provides an interface between QSettings and the GUI.
 * A QSettings key is joined with a QLineEdit, QRadioButton, QComboBox or QCheckBox,
 * to/from which you can load or save the configuration.
 *
 * How to use:
 * 1. Make a QSettings object.
 * 2. Give a pointer to QSettings to make a SettingsInterface object.
 * 3. Join a key from QSettings with a GUI object, fx. "General/text" with ui->lineEdit,
 *    or join a bunch of keys with objects.
 *    When joining, the stored value is immediately applied to the object,
 *    and in case there isn't a value yet, the default value is applied.
 * 4. To save all the GUI settings to QSettings, run saveSettingsFromGUI().
 * 5. In case you want to save only a single GUI object value, run
 *    saveOneGUI(Object*). It will return a "true" if the object could be matched
 *    with the "join"-ed key and saved, or a "false" if it couldn't.
 * 6. Note that with loadSettingsToGUI() you can't specify any default values.
 * 7. Enjoy.
 */

class QSettingsInterface : public QObject
{
private:
    Q_OBJECT
    QMap<QString,QCheckBox*> map_CheckBoxes;
    QMap<QString,QRadioButton*> map_RadioButtons;
    QMap<QString,QLineEdit*> map_LineEdits;
    QMap<QString,QComboBox*> map_ComboBoxes;
    QSettings* qsettings;

public:
    QSettingsInterface(QSettings* qset): qsettings(qset) {}
    ~QSettingsInterface(){}

public slots:
    void join(QString settingsKey, QLineEdit *lineEdit, QString defaultValue = "");
    void join(QString settingsKey, QRadioButton* radioButton, Qt::CheckState defaultValue = Qt::Unchecked);
    void join(QString settingsKey, QCheckBox* checkBox, Qt::CheckState defaultValue = Qt::Unchecked);
    void join(QString settingsKey, QComboBox* comboBox, int defaultIndex = 0);
    void saveSettingsFromGUI();
    void loadSettingsToGUI();
    bool saveOneGUI(QCheckBox* checkBox);
    bool saveOneGUI(QRadioButton* radioButton);
    bool saveOneGUI(QLineEdit* lineEdit);
    bool saveOneGUI(QComboBox* comboBox);
};

#endif // SETTINGSINTERFACE

