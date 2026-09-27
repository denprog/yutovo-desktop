@echo off

for /r .\library %%f in (*.yut) do (
    echo Packing: %%f
    "C:\Program Files\7-Zip\7z.exe" a -tgzip "%%f.gz" "%%f" -y
    move /y "%%f.gz" "%%f" >nul
)

echo Done
pause