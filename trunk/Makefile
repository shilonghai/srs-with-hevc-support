.PHONY: default _default install install-api help clean server srs_ingest_hls librtmp utest _prepare_dir 

# install prefix.
SRS_PREFIX=/usr/local/srs
__REAL_INSTALL=$(DESTDIR)$(SRS_PREFIX)

default:
	$(MAKE) _default

_default: server srs_ingest_hls librtmp utest 
	@bash objs/_srs_build_summary.sh

help:
	@echo "Usage: make <help>|<clean>|<server>|<srs_ingest_hls>|<librtmp>|<utest>|<install>|<install-api>|<uninstall>"
	@echo "     help               display this help menu"
	@echo "     clean              cleanup project"
	@echo "     server             build the srs(simple rtmp server) over st(state-threads)"
	@echo "     srs_ingest_hls     build the hls ingest tool of srs."
	@echo "     librtmp            build the client publish/play library, and samples"
	@echo "     utest              build the utest for srs"
	@echo "     install            install srs to the prefix path"
	@echo "     install-api        install srs and api-server to the prefix path"
	@echo "     uninstall          uninstall srs from prefix path"
	@echo "@remark all modules will auto genearted and build"
	@echo "For example:"
	@echo "     make"
	@echo "     make help"

clean: 
	(cd objs && rm -rf srs srs_utest )
	(cd objs && rm -rf src include lib)
	(cd objs/utest && rm -rf *.o *.a)
	(cd research/librtmp && make clean)
	(cd research/api-server/static-dir && rm -rf crossdomain.xml forward live players)

server: _prepare_dir
	@echo "build the srs(simple rtmp server) over st(state-threads)"
	$(MAKE) -f objs/Makefile srs
srs_ingest_hls: _prepare_dir
	@echo "build the srs_ingest_hls for srs"
	$(MAKE) -f objs/Makefile srs_ingest_hls

uninstall:
	@echo "rmdir $(SRS_PREFIX)"
	@rm -rf $(SRS_PREFIX)

install-api: install
	@echo "mkdir $(__REAL_INSTALL)"
	@mkdir -p $(__REAL_INSTALL)
	@echo "copy binary files"
	@mkdir -p $(__REAL_INSTALL)/research/api-server
	@cp research/api-server/server.py $(__REAL_INSTALL)/research/api-server
	@mkdir -p $(__REAL_INSTALL)/objs/ffmpeg/bin
	@cp objs/ffmpeg/bin/ffmpeg $(__REAL_INSTALL)/objs/ffmpeg/bin
	@echo "copy html files"
	@mkdir -p $(__REAL_INSTALL)/research/api-server/static-dir/players
	@cp research/api-server/static-dir/crossdomain.xml $(__REAL_INSTALL)/research/api-server/static-dir
	@cp research/api-server/static-dir/index.html $(__REAL_INSTALL)/research/api-server/static-dir
	@cp -r research/api-server/static-dir/players/* $(__REAL_INSTALL)/research/api-server/static-dir/players
	@echo "copy init.d script files"
	@mkdir -p $(__REAL_INSTALL)/etc/init.d
	@cp etc/init.d/srs-api $(__REAL_INSTALL)/etc/init.d
	@sed -i "s|^ROOT=.*|ROOT=\"$(SRS_PREFIX)\"|g" $(__REAL_INSTALL)/etc/init.d/srs-api
	@echo ""
	@echo "api installed, to link and start api:"
	@echo "     sudo ln -sf $(SRS_PREFIX)/etc/init.d/srs-api /etc/init.d/srs-api"
	@echo "     /etc/init.d/srs-api start"
	@echo "     http://$(shell bash auto/local_ip.sh):8085"
	@echo "@see: https://github.com/ossrs/srs/wiki/v1_CN_LinuxService"

install:
	@echo "mkdir $(__REAL_INSTALL)"
	@mkdir -p $(__REAL_INSTALL)
	@echo "make the http root dir"
	@mkdir -p $(__REAL_INSTALL)/objs/nginx/html
	@cp research/api-server/static-dir/crossdomain.xml $(__REAL_INSTALL)/objs/nginx/html
	@echo "copy binary files"
	@mkdir -p $(__REAL_INSTALL)/objs
	@cp objs/srs $(__REAL_INSTALL)/objs
	@echo "copy srs conf files"
	@mkdir -p $(__REAL_INSTALL)/conf
	@cp conf/*.conf $(__REAL_INSTALL)/conf
	@echo "copy init.d script files"
	@mkdir -p $(__REAL_INSTALL)/etc/init.d
	@cp etc/init.d/srs $(__REAL_INSTALL)/etc/init.d
	@sed -i "s|^ROOT=.*|ROOT=\"$(SRS_PREFIX)\"|g" $(__REAL_INSTALL)/etc/init.d/srs
	@echo ""
	@echo "srs installed, to link and start srs:"
	@echo "     sudo ln -sf $(SRS_PREFIX)/etc/init.d/srs /etc/init.d/srs"
	@echo "     /etc/init.d/srs start"
	@echo "@see: https://github.com/ossrs/srs/wiki/v1_CN_LinuxService"

librtmp: server
	@echo "srs-librtmp is disabled, ignore."

utest: server
	@echo "utest is disabled, ignore"

# the ./configure will generate it.
_prepare_dir:
	@mkdir -p objs
	@mkdir -p objs/src/core
	@mkdir -p objs/src/kernel
	@mkdir -p objs/src/protocol
	@mkdir -p objs/src/app
	@mkdir -p objs/src/libs
	@mkdir -p objs/src/main
