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

    stacked_widget_active_page_ = settings_->value("Main/StackedWidgetActivePage").toUInt();
    download_path_ = settings_->value("Main/DownloadPath").toString();
    expand_details_ = settings_->value("Main/ExpandDetails").toBool();
    last_search_ = settings_->value("Main/LastSearch").toString();
    combo_sort_type_ = settings_->value("Main/comboSortType").toInt();
    open_in_player_after_download_ = settings_->value("Main/OpenInPlayerAfterDownload").toBool();
    auto_download_ = settings_->value("Main/AutoDownload").toBool();

    hide_status_button_ = settings_->value("Settings/HideStatusButton").toBool();
    dark_style_ = settings_->value("Settings/DarkStyle").toBool();
    do_not_save_size_and_position_ = settings_->value("Settings/DoNotSaveSizeAndPosition").toBool();
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

uint Settings::stacked_widget_active_page() const
{
    return stacked_widget_active_page_;
}

void Settings::setStacked_widget_active_page(uint stacked_widget_active_page)
{
    stacked_widget_active_page_ = stacked_widget_active_page;
    settings_->setValue("Main/StackedWidgetActivePage", stacked_widget_active_page);
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

bool Settings::open_in_player_after_download() const
{
    return open_in_player_after_download_;
}
void Settings::setOpen_in_player_after_download(bool value)
{
    open_in_player_after_download_ = value;
    settings_->setValue("Main/OpenInPlayerAfterDownload", value);
}

bool Settings::hide_status_button() const
{
    return hide_status_button_;
}
void Settings::setHide_status_button(bool hide_status_button)
{
    hide_status_button_ = hide_status_button;
    settings_->setValue("Settings/HideStatusButton", hide_status_button);
}

bool Settings::auto_download() const
{
    return auto_download_;
}
void Settings::setAuto_download(bool auto_download)
{
    auto_download_ = auto_download;
    settings_->setValue("Main/AutoDownload", auto_download);
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

bool Settings::do_not_save_size_and_position() const
{
    return do_not_save_size_and_position_;
}
void Settings::setDo_not_save_size_and_position(bool do_not_save_size_and_position)
{
    do_not_save_size_and_position_ = do_not_save_size_and_position;
    settings_->setValue("Settings/DoNotSaveSizeAndPosition", do_not_save_size_and_position);
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
