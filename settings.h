#ifndef SETTINGS_H
#define SETTINGS_H
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QDir>

class Settings
{
public:
    Settings(QString application_dir_path);
    ~Settings();

    // MainWindow

    QSize size() const;
    void setSize(const QSize &size);

    QPoint position() const;
    void setPosition(const QPoint &position);

    // Main
    QString download_path() const;
    void setDownload_path(const QString &download_path);

    QString last_search() const;
    void setLast_search(const QString &last_search);

    bool expand_status_and_settings() const;
    void setExpand_status_and_settings(bool expand_status_and_settings);

    int stacked_widget_active_page() const;
    void setStacked_widget_active_page(int stacked_widget_active_page);

    int combo_sort_type() const;
    void setCombo_sort_type(int combo_sort_type);

    // Main - options

    bool auto_download() const;
    void setAuto_download(bool auto_download);

    // Settings window
    bool hide_status_button() const;
    void setHide_status_button(bool hide_status_button);

    bool dark_style() const;
    void setDark_style(bool dark_style);

    QString media_player_path() const;
    void setMedia_player_path(const QString &media_player_path);

    QString media_player_args() const;
    void setMedia_player_args(const QString &media_player_args);

    QString output_template() const;
    void setOutput_template(const QString &output_template);

    bool open_in_player_after_download() const;
    void setOpen_in_player_after_download(bool value);

    bool do_not_save_size_and_position() const;
    void setDo_not_save_size_and_position(bool do_not_save_size_and_position);

    QString youtube_dl_executable() const;
    void setYoutube_dl_executable(const QString &youtube_dl_executable);

    QString ffmpeg_path() const;
    void setFfmpeg_path(const QString &ffmpeg_path);

    bool exit_when_finshed() const;
    void setExit_when_finshed(bool exit_when_finshed);

    bool show_statusbar() const;
    void setShow_statusbar(bool show_statusbar);

private:
    QSettings *settings_;
    void load();
    void defaults();

    // Tip i just learned. If you put curser on a member vaiable (without getters and setters)
    // and presses alt+enter
    // then it gives the option to create getter/setter member functions.

    QString application_dir_path_;

    // MainWindow
    QSize size_;
    QPoint position_;

    // Main

    QString download_path_ = ".";
    QString last_search_;
    bool auto_download_ = false;
    bool expand_status_and_settings_ = true; //rename to expand, because it is not the oposite of "always hide details"
    int stacked_widget_active_page_ = 0;
    int combo_sort_type_;
    bool open_in_player_after_download_ = false;
    bool exit_when_finshed_ = false;

    // Settings window
    bool hide_status_button_ = false;
    bool dark_style_ = false;
    bool do_not_save_size_and_position_ = false;
    QString media_player_path_;
    QString media_player_args_;
    QString output_template_;
    QString youtube_dl_executable_;
    QString ffmpeg_path_;


    //Menu
    bool show_statusbar_ = false;
};

#endif // SETTINGS_H
