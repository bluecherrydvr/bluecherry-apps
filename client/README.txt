Building:
    mkdir build; cd build
    qmake ../client.pro
    make (or nmake)
    
    For a release build, use: qmake ../client.pro 'CONFIG += release'

Without breakpad: qmake ../client.pro 'CONFIG += no-breakpad'

Breakpad (Linux):
    Build the included copy of breakpad (./configure --disable-shared; make). Do not install.
    Build the application normally. Breakpad is statically linked.

Breakpad (Windows + MSVC):
    Python is required.
    In the breakpad dir, run 'src/tools/gyp/gyp.bat src/client/windows'
    Open src/client/windows/breakpad_client.sln
    Set build type and architecture as needed
    Build all, then build the application normally.
    
    If CRT linker errors appear, change the CRT linking for all projects except build_all
    to use the multithreaded (debug/release) DLL.
    
Breakpad (Mac):
    Open breakpad/src/client/mac/Breakpad.xcodeproj
    Set built type and architecture
    Under targets, build 'All'
    Copy breakpad/src/client/mac/build/{Debug,Release}/Breakpad.framework to breakpad-bin
    (Optional) Remove unnecessary files from the framework
    Build the application
