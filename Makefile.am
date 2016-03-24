SUBDIRS = src

if USE_SILENT_RULES
  AM_MAKEFLAGS = -s
endif

ACLOCAL_AMFLAGS = -I m4/config

EXTRA_DIST = LICENSE
CLEANFILES =
BUILT_SOURCES =
MAINTAINERCLEANFILES =

if USE_DEV_HEADERS
config_includedir = $(ipseclibdir)/include
nodist_config_include_HEADERS = config.h
endif

# we leave config files behind intentionally so prevent distcheck from complaining
distuninstallcheck_listfiles = find . -type f \! -name '*.conf' -print

.PHONY: cov-reset-common cov-reset cov-report coverage apidoc
