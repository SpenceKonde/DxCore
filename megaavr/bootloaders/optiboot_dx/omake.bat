if exist "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\make.exe" (
    "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin\make.exe" OS=windows ENV=arduino %*
) else (
    ..\..\..\tools\avr\utils\bin\make OS=windows ENV=arduino %*
)
