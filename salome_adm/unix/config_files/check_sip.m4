
AC_DEFUN([CHECK_SIP],[
AC_REQUIRE([CHECK_PYTHON])dnl

sip_ok=yes

AC_ARG_WITH(sip,
    [  --with-sip=EXEC sip executable ],
    [SIP="$withval"
      AC_MSG_RESULT("select $withval as sip executable")
    ], [
      AC_PATH_PROG(SIP, sip)
    ])

if test "x$SIP" = "x"
then
    sip_ok=no
    AC_MSG_RESULT(sip not in PATH variable)
else
	SIP_ROOT="$SIPDIR"
	SIP_INCLUDES="${PYTHON_INCLUDES} -I${SIPDIR}"
	SIP_LIBS="-L${PYTHON_PREFIX}/lib/python${PYTHON_VERSION}/site-packages -L${SIPDIR} -lsip"
fi

AC_SUBST(SIP)
AC_SUBST(SIP_INCLUDES)
AC_SUBST(SIP_LIBS)

AC_MSG_RESULT(for sip: $sip_ok)

])dnl
dnl
