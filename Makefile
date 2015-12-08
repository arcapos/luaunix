SRCS=		luaunix.c select.c
LIB=		unix

OS!=		uname
LUA_VERSION?=	5.2

OPENGL ?=	no

.if ${OS} == "NetBSD"
XDIR=		${X11ROOTDIR}
LOCALBASE?=	/usr/pkg
LDADD+=		-R/usr/lib -R${LOCALBASE}/lib
.else
LOCALBASE=	/usr/local
.endif

NOLINT=	1
CFLAGS+=	-I${XDIR}/include -I${LOCALBASE}/include
LDADD+=		-L${LOCALBASE}/lib

LIBDIR=		${LOCALBASE}/lib/lua/${LUA_VERSION}

libinstall:

install:
	${INSTALL} -d ${DESTDIR}${LIBDIR}
	${INSTALL} lib${LIB}.so ${DESTDIR}${LIBDIR}/${LIB}.so

.include <bsd.lib.mk>
