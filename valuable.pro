TEMPLATE = app
CONFIG -= qt
CONFIG += console c++14

isEmpty(GOOGLETEST_PATH): GOOGLETEST_PATH=$$(GOOGLETEST_PATH)
isEmpty(GOOGLETEST_PATH): error(Set GOOGLETEST_PATH to the location of the Google Test git checkout)
INCLUDEPATH += \
  $$GOOGLETEST_PATH/googletest \
  $$GOOGLETEST_PATH/googletest/include
SOURCES += \
  $$GOOGLETEST_PATH/googletest/src/gtest_main.cc \
  $$GOOGLETEST_PATH/googletest/src/gtest-all.cc

SOURCES += \
  valuable/tests/main.cpp
HEADERS += \
  valuable/include/value-ptr.hpp
INCLUDEPATH += \
  $$OUT_PWD

COPY_HEADERS += \
  valuable/include/value-ptr.hpp
COPY_HEADERS_PREFIX = valuable

# see https://stackoverflow.com/a/33310168/1329652

QMAKE_EXTRA_COMPILERS += copy_headers

copy_headers.name = COPY
copy_headers.input = COPY_HEADERS
copy_headers.CONFIG = no_link

copy_headers.output_function = headerCopyDestination
defineReplace(headerCopyDestination) {
  return($$COPY_HEADERS_PREFIX/$$basename(1))
}

win32:isEmpty(MINGW_IN_SHELL) {
  # Windows shell
  copy_headers.commands = copy /y ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
  TOUCH = copy /y nul
}
else {
  # Unix shell
  copy_headers.commands = mkdir -p `dirname ${QMAKE_FILE_OUT}` && cp ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
  TOUCH = touch
}

QMAKE_EXTRA_TARGETS += copy_headers_cookie
copy_headers_cookie.target = copy_headers.cookie
copy_headers_cookie.depends = compiler_copy_headers_make_all

win32:!mingw {
  # NMake/MSBuild
  copy_headers_cookie.commands = $$TOUCH $** && $$TOUCH $@
}
else {
  # GNU Make
  copy_headers_cookie.commands = $$TOUCH $<  && $$TOUCH $@
}

PRE_TARGETDEPS += $${copy_headers_cookie.target}
