#--------------------------------------------------
# COMMON PROJECT FILES TO DEFINE BUILD DIRECTORIES
# AND COMMON INCLUSIONS (INCLUDES AND LIBS)
#--------------------------------------------------

DEFINES +=SHARE_DIR=\\\"$$PREFIX\\\"

unix {
    CONFIG(release, debug|release) {
        DESTDIR         += ../../../build
        OBJECTS_DIR     += ../../../build/src/$$TARGET
        MOC_DIR         += ../../../build/src/$$TARGET
        RCC_DIR         += ../../../build/src/$$TARGET
        UI_DIR          += ../../../build/src/$$TARGET
        UI_HEADERS_DIR  += ../../../build/src/$$TARGET
        UI_SOURCES_DIR  += ../../../build/src/$$TARGET
    }
    CONFIG(debug, debug|release) {
        DESTDIR         += ../../../debugbuild
        OBJECTS_DIR     += ../../../debugbuild/src/$$TARGET
        MOC_DIR         += ../../../debugbuild/src/$$TARGET
        RCC_DIR         += ../../../debugbuild/src/$$TARGET
        UI_DIR          += ../../../debugbuild/src/$$TARGET
        UI_HEADERS_DIR  += ../../../debugbuild/src/$$TARGET
        UI_SOURCES_DIR  += ../../../debugbuild/src/$$TARGET
        INCLUDEPATH     += ../../../debugbuild/src/$$TARGET
        DEFINES         += DEBUG_MODE
    }

    INCLUDEPATH += /usr/include/ffmpeg/                                     # Specific for Fedora
    INCLUDEPATH += /usr/include/SDL
    LIBS        += -lexiv2                                                  #------ Exiv2
    LIBS        += -lqimageblitz                                            #------ QImageBlitz
}

win32 {
    CONFIG(release, debug|release) {
        DESTDIR         += ..\\..\\..\\winbuild
        OBJECTS_DIR     += ..\\..\\..\\winbuild\\src\\$$TARGET
        MOC_DIR         += ..\\..\\..\\winbuild\\src\\$$TARGET
        RCC_DIR         += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_DIR          += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_HEADERS_DIR  += ..\\..\\..\\winbuild\\src\\$$TARGET
        UI_SOURCES_DIR  += ..\\..\\..\\winbuild\\src\\$$TARGET
    }
    CONFIG(debug, debug|release) {
        DESTDIR         += ..\\..\\..\\windebugbuild
        OBJECTS_DIR     += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        MOC_DIR         += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        RCC_DIR         += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_DIR          += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_HEADERS_DIR  += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        UI_SOURCES_DIR  += ..\\..\\..\\windebugbuild\\src\\$$TARGET
        DEFINES         += DEBUG_MODE
    }

    #INCLUDEPATH += C:\\ffDiaporama-trunk-Qt\\libav-win32\\usr\\include      #------ libav library path
    INCLUDEPATH += C:\\ffDiaporama-trunk-Qt\\ffmpeg-win32-dev\\include      #------ ffmpeg library path
    INCLUDEPATH += C:\\ffDiaporama-trunk-Qt\\SDL-1.2.15\\include\\SDL       #------ SDL library path
    INCLUDEPATH += C:\\ffDiaporama-trunk-Qt\\ThirdLib\\include              #------ TAGlib and EXIV2 library path
    INCLUDEPATH += C:\\ffDiaporama-trunk-Qt\\qimageblitz-0.0.6\\blitz       #------ QImageBlitz
    INCLUDEPATH += .                                                        #------ I don't know why, but windows need this !

    #LIBS        += -L"C:\\ffDiaporama-trunk-Qt\\libav-win32\\usr\\bin"      #------ libav library path
    LIBS        += -L"C:\\ffDiaporama-trunk-Qt\\ffmpeg-win32-dev\\lib"      #------ ffmpeg library path
    LIBS        += -L"C:\\ffDiaporama-trunk-Qt\\SDL-1.2.15\\lib"            #------ SDL library path
    LIBS        += -L"C:\\ffDiaporama-trunk-Qt\\ThirdLib\\lib"              #------ library path (dynamic version) of third party libs (exiv2lib and taglib)
    LIBS        += -L"C:\\ffDiaporama-trunk-Qt\\qimageblitz-0.0.6\\bin\\bin"#------ QImageBlitz
    LIBS        += -lgdi32 -lkernel32 -luser32 -lshell32 -ladvapi32         #------ Windows GDI libs link
    LIBS        += -llibexiv2                                               #------ Exiv2 (dynamic version, so not -llibexpat -lxmpsdk -lzlib1)
    LIBS        += -llibQImageBlitz                                         #------ QImageBlitz
    LIBS        += -lswresample                                             #------ LibAV swresample
}

#---- Libs for windows and linux
LIBS        += -ltag                                                        #------ TAGlib
LIBS	    += -lSDL                                                        #------ SDL
LIBS        += -lavformat -lavcodec -lavutil -lswscale -lavfilter           #------ libav lib link
