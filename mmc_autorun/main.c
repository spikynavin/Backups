#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    struct stat st = {0};
    FILE *file;

    if (stat("/media/sdcard", &st) == -1) {
        mkdir("/media/sdcard", 0700);
    }
    umount("/dev/mmcblk0p1");
    mount("/dev/mmcblk0p1","/media/sdcard","vfat",0, NULL);

    if( access("/media/sdcard/patch.clan", F_OK) != -1 ) {
	system("sh /usr/share/scripts/backlight 4;export DISPLAY=0:0;exec /usr/bin/clan_installer 2> /dev/null &");
    } else if(access("/media/sdcard/application.clan", F_OK) != -1){
	system("sh /usr/share/scripts/backlight 4;export DISPLAY=0:0;exec /usr/bin/clan_installer 2> /dev/null &");
	}

    return 0;
}

