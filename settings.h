#ifndef SETTINGS_H
#define SETTINGS_H
#include <QSettings>
#include <QSize>
#include <QPoint>

class Settings
{
public:
    Settings();
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

    bool expand_details() const;
    void setExpand_details(bool expand_details);

    int combo_sort_type() const;
    void setCombo_sort_type(int combo_sort_type);

    // Settings window
    bool always_hide_details() const;
    void setAlways_hide_details(bool always_hide_details);

    bool auto_download() const;
    void setAuto_download(bool auto_download);

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

private:
    QSettings *settings_;
    void load();
    void defaults();

    // Tip i just learned. If you put curser on a member vaiable (without getters and setters)
    // and presses alt+enter
    // then it gives the option to create getter/setter member functions.

    // MainWindow
    QSize size_;
    QPoint position_;

    // Main

    QString download_path_ = ".";
    QString last_search_;
    bool expand_details_ = true; //rename to expand, because it is not the oposite of "always hide details"
    int combo_sort_type_;
    bool open_in_player_after_download_ = false;

    // Settings window
    bool always_hide_details_ = false;
    bool auto_download_ = false;
    bool dark_style_ = false;
    bool do_not_save_size_and_position_ = false;
    QString media_player_path_;
    QString media_player_args_;
    QString output_template_;
};

#endif // SETTINGS_H
