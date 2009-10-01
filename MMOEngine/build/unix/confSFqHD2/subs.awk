BEGIN {
S["LTLIBOBJS"]=""
S["LIBOBJS"]=""
S["USER_LIBS_PATH"]="/usr/local/lib"
S["USER_INCLUDES_PATH"]="/usr/local/include"
S["SYSTEM_LIBS"]="-lpthread -lwsock32"
S["SYSTEM_INCLUDES"]=""
S["CFLAGS"]=""
S["RANLIB"]="ranlib"
S["am__fastdepCXX_FALSE"]="#"
S["am__fastdepCXX_TRUE"]=""
S["CXXDEPMODE"]="depmode=gcc3"
S["AMDEPBACKSLASH"]="\\"
S["AMDEP_FALSE"]="#"
S["AMDEP_TRUE"]=""
S["am__quote"]=""
S["am__include"]="include"
S["DEPDIR"]=".deps"
S["OBJEXT"]="o"
S["EXEEXT"]=".exe"
S["ac_ct_CXX"]="g++"
S["CPPFLAGS"]=""
S["LDFLAGS"]=""
S["CXXFLAGS"]="  "
S["CXX"]="g++"
S["am__untar"]="${AMTAR} xf -"
S["am__tar"]="${AMTAR} chof - \"$$tardir\""
S["AMTAR"]="${SHELL} /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/missing --run tar"
S["am__leading_dot"]="."
S["SET_MAKE"]=""
S["AWK"]="gawk"
S["mkdir_p"]="/usr/bin/mkdir -p"
S["MKDIR_P"]="/usr/bin/mkdir -p"
S["INSTALL_STRIP_PROGRAM"]="$(install_sh) -c -s"
S["STRIP"]=""
S["install_sh"]="$(SHELL) /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/install-sh"
S["MAKEINFO"]="${SHELL} /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/missing --run makeinfo"
S["AUTOHEADER"]="${SHELL} /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/missing --run autoheader"
S["AUTOMAKE"]="${SHELL} /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/missing --run automake-1.10"
S["AUTOCONF"]="${SHELL} /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/missing --run autoconf"
S["ACLOCAL"]="${SHELL} /cygdrive/d/Programming/Projects/swgemu/core3/MMOEngine/build/unix/config/missing --run aclocal-1.10"
S["VERSION"]="1.0"
S["PACKAGE"]="engine3"
S["CYGPATH_W"]="cygpath -w"
S["am__isrc"]=" -I$(srcdir)"
S["INSTALL_DATA"]="${INSTALL} -m 644"
S["INSTALL_SCRIPT"]="${INSTALL}"
S["INSTALL_PROGRAM"]="${INSTALL}"
S["target_os"]="cygwin"
S["target_vendor"]="pc"
S["target_cpu"]="i686"
S["target"]="i686-pc-cygwin"
S["host_os"]="cygwin"
S["host_vendor"]="pc"
S["host_cpu"]="i686"
S["host"]="i686-pc-cygwin"
S["build_os"]="cygwin"
S["build_vendor"]="pc"
S["build_cpu"]="i686"
S["build"]="i686-pc-cygwin"
S["target_alias"]=""
S["host_alias"]=""
S["build_alias"]=""
S["LIBS"]=""
S["ECHO_T"]=""
S["ECHO_N"]="-n"
S["ECHO_C"]=""
S["DEFS"]="-DHAVE_CONFIG_H"
S["mandir"]="${datarootdir}/man"
S["localedir"]="${datarootdir}/locale"
S["libdir"]="${exec_prefix}/lib"
S["psdir"]="${docdir}"
S["pdfdir"]="${docdir}"
S["dvidir"]="${docdir}"
S["htmldir"]="${docdir}"
S["infodir"]="${datarootdir}/info"
S["docdir"]="${datarootdir}/doc/${PACKAGE_TARNAME}"
S["oldincludedir"]="/usr/include"
S["includedir"]="${prefix}/include"
S["localstatedir"]="${prefix}/var"
S["sharedstatedir"]="${prefix}/com"
S["sysconfdir"]="${prefix}/etc"
S["datadir"]="${datarootdir}"
S["datarootdir"]="${prefix}/share"
S["libexecdir"]="${exec_prefix}/libexec"
S["sbindir"]="${exec_prefix}/sbin"
S["bindir"]="${exec_prefix}/bin"
S["program_transform_name"]="s,x,x,"
S["prefix"]="/usr/local"
S["exec_prefix"]="${prefix}"
S["PACKAGE_BUGREPORT"]=""
S["PACKAGE_STRING"]="engine3 1.0"
S["PACKAGE_VERSION"]="1.0"
S["PACKAGE_TARNAME"]="engine3"
S["PACKAGE_NAME"]="engine3"
S["PATH_SEPARATOR"]=":"
S["SHELL"]="/bin/sh"
  for (key in S) S_is_set[key] = 1
  FS = ""

}
{
  line = $ 0
  nfields = split(line, field, "@")
  substed = 0
  len = length(field[1])
  for (i = 2; i < nfields; i++) {
    key = field[i]
    keylen = length(key)
    if (S_is_set[key]) {
      value = S[key]
      line = substr(line, 1, len) "" value "" substr(line, len + keylen + 3)
      len += length(value) + length(field[++i])
      substed = 1
    } else
      len += 1 + keylen
  }

  print line
}

