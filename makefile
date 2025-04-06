# ----------------------------
# Makefile Options
# ----------------------------

NAME = GALLERY
ICON = icon.png
DESCRIPTION = "USB Image Viewer -- William Wierzbowski"
COMPRESSED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
