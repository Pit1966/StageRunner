TEMPLATE = subdirs
SUBDIRS += src
!android:!ios {
  SUBDIRS += test
}
