bin_PROGRAMS = ichor
ichor_SOURCES = main.cc drawing.cc util.cc Menu.cc Texture.cc GalagaMode.cc DuelMode.cc HighScoreMode.cc
noinst_HEADERS = Explosion.h FluidGrid.h GameMode.h HighScoreSheet.h Input.h Menu.h Particle.h Player.h Texture.h Timer.h color.h drawing.h tweak.h util.h vec.h
SUBDIRS = media

CPPFLAGS = -DICHOR_DATADIR="\"$(pkgdatadir)\"" -DICHOR_SCORESDIR="\"$(DESTDIR)$(scoredir)\"" @CPPFLAGS@

install-data-local:
	-$(mkinstalldirs) $(DESTDIR)$(scoredir)
	-chown $(scores_user):$(scores_group) $(DESTDIR)$(scoredir)
	-chmod 775 $(DESTDIR)$(scoredir)
	-echo -ne '\0' > $(DESTDIR)$(scoredir)/scores.dat
	-chown $(scores_user):$(scores_group) $(DESTDIR)$(scoredir)/scores.dat
	-chmod 664 $(DESTDIR)$(scoredir)/scores.dat
