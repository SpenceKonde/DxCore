call build_all_dx 1> build_dx.log 2> error_dx.log
call build_all_dd 1> build_dd.log 2> error_dx.log
call "C:\program files\7-zip\7z.exe" a -ttar elves.tar *.elf
call "C:\program files\7-zip\7z.exe" a -ttar listings.tar *.elf
call "C:\program files\7-zip\7z.exe" a -tbz2 elves.bz2 elves.tar
call "C:\program files\7-zip\7z.exe" a -tbz2 listings.bz2 listings.tar
