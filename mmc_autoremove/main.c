#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    struct stat st = {0};

    umount("/media/sdcard");
    umount("/dev/mmcblk0p1");
    if (stat("/media/sdcard", &st) != -1) {
        system("rm /media/sdcard -rf");
    }

    return 0;
}
