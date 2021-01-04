#include <stdio.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void)
{
    struct stat st = {0};

    umount("/media/thumbdrive");
    umount("/dev/sda1");
    if (stat("/media/thumbdrive", &st) != -1) {
        system("rm /media/thumbdrive -rf");
    }

    return 0;
}
