#include "settings.h"

Settings::Settings()
{
    // I have changed the settings location, because I hate when small programs
    // write their config files in a directory under their organization name (even though Triple Jim is awesome).
    // It is always like, QYoutubeDownloader, who wrote that crap?
    // I want to find the damn config file. Why can't they put it in a directory, named after the bloddy program.
    settings_ = new QSettings(QSettings::IniFormat, QSettings::UserScope, "QYoutubeDownloader", "config");
    load();
}

Settings::~Settings()
{
    delete settings_; //Delete to sync() settings
}

void Settings::load()
{
    size_ = settings_->value("MainWindow/size", QSize(400, 400)).toSize();
    position_ = settings_->value("MainWindow/position", QPoint(200, 200)).toPoint();

    download_path_ = settings_->value("Main/DownloadPath").toString();
    expand_details_ = settings_->value("Main/ExpandDetails").toBool();
    last_search_ = settings_->value("Main/LastSearch").toString();
    combo_sort_type_ = settings_->value("Main/comboSortType").toInt();

    always_hide_details_ = settings_->value("Settings/AlwaysHideDetails").toBool();
    auto_download_ = settings_->value("Settings/AutoDownload").toBool();
    dark_style_ = settings_->value("Settings/DarkStyle").toBool();
    media_player_path_ = settings_->value("Settings/MediaPlayer").toString();
    media_player_args_ = settings_->value("Settings/MediaPlayerArgs").toString();
    output_template_ = settings_->value("Settings/OutputTemplate").toString();
    defaults();
}

void Settings::defaults()
{
    if(output_template() == "")
        setOutput_template("%(uploader)s - %(title)s.%(ext)s");
}


QSize Settings::size() const
{
    return size_;
}
void Settings::setSize(const QSize &size)
{
    size_ = size;
    settings_->setValue("MainWindow/size", size);
}

QPoint Settings::position() const
{
    return position_;
}
void Settings::setPosition(const QPoint &position)
{
    position_ = position;
    settings_->setValue("MainWindow/position", position);
}

QString Settings::download_path() const
{
    return download_path_;
}
void Settings::setDownload_path(const QString &download_path)
{
    download_path_ = download_path;
    settings_->setValue("Main/DownloadPath", download_path);
}

QString Settings::last_search() const
{
    return last_search_;
}
void Settings::setLast_search(const QString &last_search)
{
    last_search_ = last_search;
    settings_->setValue("Main/LastSearch", last_search);
}

bool Settings::expand_details() const
{
    return expand_details_;
}
void Settings::setExpand_details(bool expand_details)
{
    expand_details_ = expand_details;
    settings_->setValue("Main/ExpandDetails", expand_details);
}

int Settings::combo_sort_type() const
{
    return combo_sort_type_;
}
void Settings::setCombo_sort_type(int combo_sort_type)
{
    combo_sort_type_ = combo_sort_type;
    settings_->setValue("Main/comboSortType", combo_sort_type);
}

bool Settings::always_hide_details() const
{
    return always_hide_details_;
}
void Settings::setAlways_hide_details(bool always_hide_details)
{
    always_hide_details_ = always_hide_details;
    settings_->setValue("Settings/AlwaysHideDetails", always_hide_details);
}

bool Settings::auto_download() const
{
    return auto_download_;
}
void Settings::setAuto_download(bool auto_download)
{
    auto_download_ = auto_download;
    settings_->setValue("Settings/AutoDownload", auto_download);
}

bool Settings::dark_style() const
{
    return dark_style_;
}
void Settings::setDark_style(bool dark_style)
{
    dark_style_ = dark_style;
    settings_->setValue("Settings/DarkStyle", dark_style);
}

QString Settings::media_player_path() const
{
    return media_player_path_;
}
void Settings::setMedia_player_path(const QString &media_player_path)
{
    media_player_path_ = media_player_path;
    settings_->setValue("Settings/MediaPlayer", media_player_path);
}

QString Settings::media_player_args() const
{
    return media_player_args_;
}

void Settings::setMedia_player_args(const QString &media_player_args)
{
    media_player_args_ = media_player_args;
    settings_->setValue("Settings/MediaPlayerArgs", media_player_args);
}

QString Settings::output_template() const
{
    return output_template_;
}
void Settings::setOutput_template(const QString &output_template)
{
    output_template_ = output_template;
    settings_->setValue("Settings/OutputTemplate", output_template);
}
