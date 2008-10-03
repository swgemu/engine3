xcopy ..\..\src %1\engine3\include /s /e /exclude:exclude.lst /Y
copy ..\..\lib\win32\engine3.lib %1\engine3\lib
copy ..\..\lib\win32\engine3d.lib %1\engine3\lib