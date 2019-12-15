
#pkgdata_DATA = platform/@PLATFORM@ translations skins/@SCREEN_RES@

install-data-local:
	test -z "$(pkgdatadir)" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
	$(MKDIR_P) "$(DESTDIR)$(pkgdatadir)/skins"
	cp -r translations about.txt "$(DESTDIR)$(pkgdatadir)"
	cp -r skins/@SCREEN_RES@/* "$(DESTDIR)$(pkgdatadir)/skins"
	cp -r platform/@PLATFORM@/* "$(DESTDIR)$(pkgdatadir)"
