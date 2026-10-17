cmake_minimum_required(VERSION 3.16)
project(MiniEditorQt6 VERSION 1.0 LANGUAGES CXX)

# Standard stabile per evitare conflitti con i moduli C++26 di GCC 15
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport)

# Definiamo i file sorgente
set(SOURCES
        main.cpp
        FinestraPrincipale.h
        FinestraPrincipale.cpp
        risorse.qrc
)

# Definiamo il nome dell'eseguibile in una variabile per evitare errori di battitura
set(TARGET_NAME MiniEditorQt6)

# Crea l'eseguibile
add_executable(${TARGET_NAME} ${SOURCES})

# Configura gli include (adattati ai percorsi corretti di AlmaLinux)
target_include_directories(${TARGET_NAME} PRIVATE
        /usr/include/qt6
        /usr/include/qt6/QtPrintSupport
        /usr/include/qt6/QtWidgets
)

# Collega le librerie
target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Widgets Qt6::PrintSupport)

# --- CONFIGURAZIONE DI INSTALLAZIONE E CPACK (RPM / WINDOWS) ---

# Definisce dove installare l'eseguibile sul sistema Linux (standard: /usr/local/bin)
install(TARGETS MiniEditorQt6 DESTINATION bin)

# Configurazione metadati del pacchetto RPM
set(CPACK_PACKAGE_NAME "minieditorqt6")
set(CPACK_PACKAGE_VENDOR "Giambattista")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Editor di testo avanzato sviluppato in Qt6 e C++26")
set(CPACK_PACKAGE_RELEASE "1")
set(CPACK_PACKAGE_CONTACT "giambattista@canuinformatica.eu")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")

# Se siamo su Linux, genera un pacchetto RPM
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CPACK_GENERATOR "RPM")
    # Specifica le dipendenze software che AlmaLinux verificherà prima di installarlo
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.0.0, qt6-qtprintsupport")
endif()

# Include il modulo CPack (deve essere sempre l'ultima riga)
include(CPack)

