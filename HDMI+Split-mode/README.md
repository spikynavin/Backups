#########################################
IWAVE DEV BOARD

1. 1080p@30hz HDMI Output is added
2. 10.25 inch LVDS Display is added (need to update clock frequency)

Uboot Command line args:
setenv hdmi 'video=mxcfb1:dev=hdmi,1920x1080M@30,if=RGB24,bpp=24'
setenv lcd 'video=mxcfb0:dev=ldb,1920x720M@30,if=RGB24,bpp=24'
saveenv
