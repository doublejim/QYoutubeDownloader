#include "settings.h"

Settings::Settings(QString application_dir_path)
{
    settings_ = new QSettings(QSettings::IniFormat, QSettings::UserScope, "QYoutubeDownloader", "config");
    application_dir_path_ = application_dir_path;
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

    stacked_widget_active_page_ = settings_->value("Main/StackedWidgetActivePage").toInt();
    download_path_ = settings_->value("Main/DownloadPath").toString();
    expand_status_and_settings_ = settings_->value("Main/ExpandStatusAndSettings").toBool();
    last_search_ = settings_->value("Main/LastSearch").toString();
    combo_sort_type_ = settings_->value("Main/comboSortType").toInt();
    open_in_player_after_download_ = settings_->value("Main/OpenInPlayerAfterDownload").toBool();
    exit_when_finshed_ = settings_->value("Main/ExitWhenFinished").toBool();
    auto_download_ = settings_->value("Main/AutoDownload").toBool();
    save_to_subdir_ = settings_->value("Main/SaveToSubdir").toBool();
    combo_subdir_pattern_ = settings_->value("Main/SubdirPattern").toInt();

    hide_status_button_ = settings_->value("Settings/HideStatusButton").toBool();
    dark_style_ = settings_->value("Settings/DarkStyle").toBool();
    do_not_save_size_and_position_ = settings_->value("Settings/DoNotSaveSizeAndPosition").toBool();
    media_player_path_ = settings_->value("Settings/MediaPlayer").toString();
    media_player_args_ = settings_->value("Settings/MediaPlayerArgs").toString();
    output_template_ = settings_->value("Settings/OutputTemplate").toString();
    youtube_dl_executable_ = settings_->value("Settings/Youtube-dlExecutable").toString();
    ffmpeg_path_ = settings_->value("Settings/FFMPEGPath").toString();
    show_statusbar_ = settings_->value("Menu/ShowStatusBar").toBool();

    show_osd_ = settings_->value("OSD/Show").toBool();
    osd_size_ = settings_->value("OSD/Size").toSize();
    osd_position_ = settings_->value("OSD/Position").toPoint();
    osd_hide_decoration_ = settings_->value("OSD/HideDecoration").toBool();
    defaults();
}

void Settings::defaults()
{
    if(output_template() == "")
        setOutput_template("%(uploader)s - %(title)s.%(ext)s");

    if(youtube_dl_executable() == "")
    {
        QDir program(application_dir_path_);
        #ifdef Q_OS_LINUX
            setYoutube_dl_executable(program.absoluteFilePath("youtube-dl"));
        #elif defined(Q_OS_WIN)
            youtube_dl_executable_ = program.absoluteFilePath("youtube-dl.exe");
        #else
        #error "We don't support that version yet..."
        #endif
    }
    if(ffmpeg_path() == "")
    {
        QDir ffmpeg(application_dir_path_);
        setFfmpeg_path(ffmpeg.absoluteFilePath("ffmpeg"));
    }

}
int Settings::combo_subdir_pattern() const
{
    return combo_subdir_pattern_;
}

void Settings::setCombo_subdir_pattern(int combo_subdir_pattern)
{
    combo_subdir_pattern_ = combo_subdir_pattern;
    settings_->setValue("Main/SubdirPattern", combo_subdir_pattern);
}

bool Settings::save_to_subdir() const
{
    return save_to_subdir_;
}

void Settings::setSave_to_subdir(bool save_to_subdir)
{
    save_to_subdir_ = save_to_subdir;
    settings_->setValue("Main/SaveToSubdir", save_to_subdir);
}

bool Settings::show_osd() const
{
    return show_osd_;
}

void Settings::setShow_osd(bool show_osd)
{
    show_osd_ = show_osd;
    settings_->setValue("OSD/Show", show_osd);
}

bool Settings::osd_hide_decoration() const
{
    return osd_hide_decoration_;
}

void Settings::setOsd_hide_decoration(bool osd_hide_decoration)
{
    osd_hide_decoration_ = osd_hide_decoration;
    settings_->setValue("OSD/HideDecoration", osd_hide_decoration);
}

QSize Settings::osd_size() const
{
    return osd_size_;
}

void Settings::setOsd_size(const QSize &osd_size)
{
    osd_size_ = osd_size;
    settings_->setValue("OSD/Size", osd_size);
}

QPoint Settings::osd_position() const
{
    return osd_position_;
}

void Settings::setOsd_position(const QPoint &osd_position)
{
    osd_position_ = osd_position;
    settings_->setValue("OSD/Position", osd_position);
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

bool Settings::expand_status_and_settings() const
{
    return expand_status_and_settings_;
}
void Settings::setExpand_status_and_settings(bool expand_status_and_settings)
{
    expand_status_and_settings_ = expand_status_and_settings;
    settings_->setValue("Main/ExpandStatusAndSettings", expand_status_and_settings);
}

int Settings::stacked_widget_active_page() const
{
    return stacked_widget_active_page_;
}

void Settings::setStacked_widget_active_page(int stacked_widget_active_page)
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

bool Settings::exit_when_finshed() const
{
    return exit_when_finshed_;
}
void Settings::setExit_when_finshed(bool exit_when_finshed)
{
    exit_when_finshed_ = exit_when_finshed;
    settings_->setValue("Main/ExitWhenFinished", exit_when_finshed);
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

QString Settings::youtube_dl_executable() const
{
    return youtube_dl_executable_;
}
void Settings::setYoutube_dl_executable(const QString &youtube_dl_executable)
{
    youtube_dl_executable_ = youtube_dl_executable;
    if(youtube_dl_executable == "")
        defaults();
    else
        settings_->setValue("Settings/Youtube-dlExecutable", youtube_dl_executable);
}

QString Settings::ffmpeg_path() const
{
    return ffmpeg_path_;
}
void Settings::setFfmpeg_path(const QString &ffmpeg_path)
{
    ffmpeg_path_ = ffmpeg_path;
    if(ffmpeg_path == "")
        defaults();
    else
        settings_->setValue("Settings/FFMPEGPath", ffmpeg_path);
}

bool Settings::show_statusbar() const
{
    return show_statusbar_;
}
void Settings::setShow_statusbar(bool show_statusbar)
{
    show_statusbar_ = show_statusbar;
    settings_->setValue("Menu/ShowStatusBar", show_statusbar);
}
