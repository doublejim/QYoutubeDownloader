#include <qsettingsinterface.h>

void QSettingsInterface::join(QString settingsKey, QLineEdit* lineEdit, QString defaultValue)
{
    lineEdit->setText( qsettings->value(settingsKey, defaultValue).toString() );
    map_LineEdits[settingsKey] = lineEdit;
}

void QSettingsInterface::join(QString settingsKey, QRadioButton* radioButton, Qt::CheckState defaultValue)
{
    radioButton->setChecked( qsettings->value(settingsKey, defaultValue).toBool() );
    map_RadioButtons[settingsKey] = radioButton;
}

void QSettingsInterface::join(QString settingsKey, QCheckBox* checkBox, Qt::CheckState defaultValue)
{
    checkBox->setChecked( qsettings->value(settingsKey, defaultValue).toBool() );
    map_CheckBoxes[settingsKey] = checkBox;
}

void QSettingsInterface::join(QString settingsKey, QComboBox* comboBox, int defaultIndex)
{
    comboBox->setCurrentIndex( qsettings->value(settingsKey, defaultIndex).toInt() );
    map_ComboBoxes[settingsKey]=comboBox;
}

void QSettingsInterface::saveSettingsFromGUI()
{
    foreach(QString i, map_CheckBoxes.keys())
        qsettings->setValue(i, map_CheckBoxes.value(i)->isChecked());

    foreach(QString i, map_RadioButtons.keys())
        qsettings->setValue(i, map_RadioButtons.value(i)->isChecked());

    foreach(QString i, map_LineEdits.keys())
        qsettings->setValue(i, map_LineEdits.value(i)->text());

    foreach(QString i, map_ComboBoxes.keys())
        qsettings->setValue(i, map_ComboBoxes.value(i)->currentIndex());
}

void QSettingsInterface::loadSettingsToGUI()
{
    foreach(QString i, map_CheckBoxes.keys())
        map_CheckBoxes.value(i)->setChecked(qsettings->value(i).toBool());

    foreach(QString i, map_RadioButtons.keys())
        map_RadioButtons.value(i)->setChecked(qsettings->value(i).toBool());

    foreach(QString i, map_LineEdits.keys())
        map_LineEdits.value(i)->setText(qsettings->value(i).toString());

    foreach(QString i, map_ComboBoxes.keys())
        map_ComboBoxes.value(i)->setCurrentIndex(qsettings->value(i).toInt());
}

bool QSettingsInterface::saveOneGUI(QCheckBox* checkBox)
{
    QString key = map_CheckBoxes.key(checkBox);
    if (key!="")
    {
        qsettings->setValue( key, checkBox->isChecked() );
        return true;
    }
    else return false;
}

bool QSettingsInterface::saveOneGUI(QRadioButton* radioButton)
{
    QString key = map_RadioButtons.key(radioButton);
    if (key!="")
    {
        qsettings->setValue( key, radioButton->isChecked() );
        return true;
    }
    else return false;
}

bool QSettingsInterface::saveOneGUI(QLineEdit *lineEdit)
{
    QString key = map_LineEdits.key(lineEdit);
    if (key!="")
    {
        qsettings->setValue( key, lineEdit->text() );
        return true;
    }
    else return false;
}

bool QSettingsInterface::saveOneGUI(QComboBox* comboBox)
{
    QString key = map_ComboBoxes.key(comboBox);
    if (key!="")
    {
        qsettings->setValue( key, comboBox->currentIndex() );
        return true;
    }
    else return false;
}
