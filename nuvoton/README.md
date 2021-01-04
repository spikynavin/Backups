Yocto-custom-recipes for HHC75xx Machine.

1.The recipes are included here:-

################# MAIN RECIPES ##################
	
1.Daemon recipes like:-

	1.GPSD
	2.GPIOD
	3.EEPROMTOOL
	4.TASKD
	5.KEYD
	6.NETD
	7.NFC
	8.STANDBYD
	9.RMGMTD
	10.MACHID

2.Configuration recipes like:-
	
	1.BBlayer.conf
	2.local.conf

3.Yocto image recipes like:-

	1.core-image-x11

4.Thrid party library & scripts recipes like:-
	
	1.Morpho
	2.icu
	3.zint
	4.scripts_1.0.0.bb "For the daemon_files status & scripts in usr/share"

5.Application recipes like:-
	
	1.Test-startapplication

6.QT-main library recipes like:-

	1.hhc75xx_1.0.0.bb "For running qt gui applications"

7.Main modules recipes like:-

	1.ko-files_1.0.0.bb "For the pre-compiled modules like printer,backlight,wifi..etc"

Note:- the module only working in 3.10 Kernel


