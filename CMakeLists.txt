# Project name. Should be the same as the project directory name and
# will become the name of the binary target.
SET( PROJECT_NAME "BehaviorSim")

# Project source code list
SET( PROJECT_SOURCES
  # Add all the cpp source files here
    main.cpp
    GameFactory.cpp
)

# Project executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

# Include needed to use SDL under Mac OS X
IF(APPLE)
  SET(PROJECT_SOURCES ${PROJECT_SOURCES} ../../conf/mac/SDLMain.m)
ENDIF(APPLE)

# Project dependencies
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
  OpenEngine_Core
  OpenEngine_Logging
  OpenEngine_Display
  OpenEngine_Devices
  OpenEngine_Renderers_OpenGL
  OpenEngine_Scene
  OpenEngine_Resources
  # Extensions
  Extensions_SDL
  Extensions_GenericHandlers
  Extensions_TGAResource
  Extensions_OBJResource
  Extensions_BehaviorTree
  Extensions_GenerelBehavior
)
