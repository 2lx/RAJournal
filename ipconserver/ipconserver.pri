HEADERS += ./controllerthread.h \
	./mainwindow.h \
	../database/database.h \
	../database/datatypes.h \
	../database/journalmodel.h \
	../database/solutionmodel.h \
	../database/relationalmodel.h \
	../database/dlgstructure.h

PRECOMPILED_HEADER = ./stdafx.h

SOURCES += ./controllerthread.cpp \ 
	./main.cpp \
	./mainwindow.cpp \
	./stdafx.cpp \
	../database/database.cpp \
	../database/journalmodel.cpp \
	../database/solutionmodel.cpp \
	../database/relationalmodel.cpp \
	../database/dlgstructure.cpp

FORMS += ./mainwindow.ui \
	../database/dlgstructure.ui

RESOURCES += ../resources/ipconserver.qrc
