HEADERS += \
    ./database/database.h \
    ./database/datatypes.h \
    ./src/dlglogin.h \
    ./src/dlgnotification.h \
    ./src/dlgpassword.h \
    ./src/dlgsettings.h \
    ./database/dlgstructure.h \
    ./database/journalmodel.h \
    ./src/mainwindow.h \
    ./src/plotter.h \
    ./src/qcomboboxkeyboard.h \
    ./src/qviewex.h \
    ./database/relationalmodel.h \
    ./database/solutionmodel.h \
    ./src/stdafx.h \
    ./src/tablemodel.h \
    ./src/udpalarm.h
SOURCES += \
    ./database/database.cpp \
    ./src/dlglogin.cpp \
    ./src/dlgnotification.cpp \
    ./src/dlgsettings.cpp \
    ./database/dlgstructure.cpp \
    ./src/dlgpassword.cpp \
    ./database/journalmodel.cpp \
    ./src/main.cpp \
    ./src/mainwindow.cpp \
    ./src/plotter.cpp \
    ./src/qviewex.cpp \
    ./database/relationalmodel.cpp \
    ./database/solutionmodel.cpp \
    ./src/stdafx.cpp \
    ./src/tablemodel.cpp \
    ./src/udpalarm.cpp
FORMS += ./src/mainwindow.ui \
    ./src/dlglogin.ui \
    ./src/dlgnotification.ui \
    ./src/dlgsettings.ui \
    ./database/dlgstructure.ui \
    ./src/dlgpassword.ui     
RESOURCES += resources/rajournal.qrc
