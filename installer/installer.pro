TEMPLATE = aux

INSTALLER = installer

OUTPUT = C:/Users/paipeng/Downloads/cp-camera-face-installer.exe
INPUT = $$PWD/config/config.xml
INPUT += $$PWD/packages/com.paipeng.cpcameraface/meta/package.xml
INPUT += $$PWD/packages/com.paipeng.cpcameraface/meta/installscript.qs
INPUT += $$PWD/packages/com.paipeng.cpcameraface/meta/LICENSE.txt
INPUT += $$PWD/packages/com.paipeng.installer/meta/package.xml
INPUT += $$PWD/packages/com.microsoft.redistubution/meta/package.xml
INPUT += $$PWD/packages/com.microsoft.redistubution/meta/installscript.qs

example.input = INPUT
example.output = $$INSTALLER
example.commands = C:/Qt/QtIFW-4.3.0/bin/binarycreator -c $$PWD/config/config.xml -p $$PWD/packages $$OUTPUT -v
#${QMAKE_FILE_OUT}
example.CONFIG += target_predeps no_link combine

QMAKE_EXTRA_COMPILERS += example

OTHER_FILES = README
