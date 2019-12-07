@if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\IDE" set DevenvPathVS=%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\IDE
@set DevenvExeVS=devenv.exe

@set PATH=%PATH%;C:\TFO\CIVA_DK\CIVA_DK_64_dev_003\outilsExternes\tbb_2018_1\bin\intel64

@start "" /B "%DevenvPathVS%\%DevenvExeVS%"
