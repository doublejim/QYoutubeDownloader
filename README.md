# QYoutubeDownloader
QYoutubeDownloader provides a Qt5 graphical user interface, for the youtube-dl application.

Features:
- Download queue
- Option to download video+audio or audio only.
- Light and dark theme
- Search in downloaded files
- Filename templates from youtube-dl
- Open downloaded file in media player of choice

Dependencies:
- Qt5 (https://www.qt.io/download/)
- youtube-dl (https://github.com/rg3/youtube-dl)
- ffmpeg (https://www.ffmpeg.org/download.html)

Build and execute:
1. Install dependencies.
2. Youtube-dl and ffmpeg must be in the PATH (we will fix this in the future)
3. Open in QtCreator, configure and run

The executable will be placed in the build directory

For usage with Firefox:
Install AppLauncher: https://addons.mozilla.org/en-US/firefox/addon/applauncher/
Add path to executable in the preferences and put &url; as an argument to the executable.
Goto youtube video, right click, select AppLauncher > QYoutubeDownloader

The video will now be put on the download queue.

Happy downloading pleasure will follow :-)
