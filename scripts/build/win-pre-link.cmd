@echo off

setlocal
set debug_or_release = %1

echo "arg %1"
echo "in %cd%"

if /i $1 == debug (
    echo "debug"
    copy /Y ..\libs\build-quazip-qt-5.3.1-mingw-4.8.2-x86\debug\quazipd.dll .\debug
    copy /Y ..\libs\build-wwwidgets-qt-5.3.1-mingw-4.8.2-x86\debug\wwwidgets5d.dll .\debug
    copy /Y ..\libs\build-g3log-mingw-4.8.2-x86\g3logger_shared.dll .\debug
) else (
    echo "release"
    copy /Y ..\libs\build-quazip-qt-5.3.1-mingw-4.8.2-x86\release\quazip.dll .\release
    copy /Y ..\libs\build-wwwidgets-qt-5.3.1-mingw-4.8.2-x86\release\wwwidgets5.dll .\release
    copy /Y ..\libs\build-g3log-mingw-4.8.2-x86\g3logger_shared.dll .\release
)

