# QYoutubeDownloader
QYoutubeDownloader provides a Qt 5 frontend, to the youtube-dl application.

Features:
- Download queue
- Option to download video+audio or audio only
- Light and dark theme
- Search in downloaded files
- Filename templates from youtube-dl
- Open downloaded file in media player of choice

Prebuilt 64 bit Linux snapshot (2015-02-21), with all dependencies bundled (48.8 MiB):
https://s3-eu-west-1.amazonaws.com/triplejim/QYoutubeDownloader-Linux-64bit.tar.xz

For usage with Firefox:
Install AppLauncher: https://addons.mozilla.org/en-US/firefox/addon/applauncher/
Add path to executable in the preferences and put &url; as an argument to the executable.
Goto youtube video, right click, select AppLauncher > QYoutubeDownloader
The video will now be put on the download queue.

For usage with vimperator:
Put these two lines in ~/.vimperatorrc (will map the user command :ytdl, to Ctrl+u hotkey):

command -nargs=0 qytdl :execute "!/path/to/QYoutubeDownloader.sh " + content.location.href
map <C-u> :qytdl <Enter> <Esc>


Compiling yourself

Dependencies:
- Qt 5 (https://www.qt.io/download/)
- Python 2.6, 2.7 or 3.2+
- youtube-dl (https://github.com/rg3/youtube-dl)
- ffmpeg (https://www.ffmpeg.org/download.html)

Build and execute:
- Install dependencies
- Open in QtCreator, configure and run
- Configure paths to youtube-dl and FFMPEG via preferences

The executable will be placed in the build directory

Happy downloading :-)
