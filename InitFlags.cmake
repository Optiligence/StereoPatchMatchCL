IF(NOT CMAKE_FLAGS_INIT)
	SET(CMAKE_FLAGS_INIT TRUE CACHE INTERNAL "initial flags set")
	# initial compiler flags can be set here, this is only
	# executed once in the first configure run.
	IF(CMAKE_COMPILER_IS_GNUCXX)
		IF(NOT CMAKE_C_FLAGS)
			SET(CMAKE_C_FLAGS "-W -Wall -Wextra")
		ENDIF()
		IF(NOT CMAKE_CXX_FLAGS)
			SET(CMAKE_CXX_FLAGS "-W -Wall -Wextra")
		ENDIF()
		IF(NOT CMAKE_SHARED_LINKER_FLAGS)
			SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed,--no-undefined")
		ENDIF()
	ENDIF(CMAKE_COMPILER_IS_GNUCXX)
	IF(MSVC)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4251 /wd4275 /wd4290")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4275 /wd4290")
	ENDIF(MSVC)

	# commit changed flags
	SET(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING
		"Flags used by the compiler during all build types."
		FORCE
	)
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING
		"Flags used by the compiler during all build types."
		FORCE
	)
	SET(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} CACHE STRING
		"Flags used by the linker during the creation of dll's."
		FORCE
	)

	# set postfix to append to debug library names
	SET(CMAKE_DEBUG_POSTFIX _d)
	
	# default to debug builds
	IF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")
		SET(CMAKE_BUILD_TYPE "Debug"
			CACHE STRING
			"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
			FORCE
		)
	ENDIF(DEFINED CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE STREQUAL "")
	# set install prefix to global install path
	IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		SET(CMAKE_INSTALL_PREFIX "${charon-core_ROOT_DIR}"
			CACHE PATH "Install path prefix, prepended onto install directories."
			FORCE
		)
	ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
ENDIF(NOT CMAKE_FLAGS_INIT)

# set definitions depending on build environment
IF(WIN32)
	ADD_DEFINITIONS(-DWINDOWS)
	IF(MSVC)
		ADD_DEFINITIONS(-DMSVC)
		# Tell MSVC not to use the unholy min and max macros
		ADD_DEFINITIONS(-DNOMINMAX)
		ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
	ENDIF(MSVC)
	IF(MINGW)
		ADD_DEFINITIONS(-DMINGW)
	ENDIF(MINGW)
ENDIF(WIN32)
IF(UNIX)
	ADD_DEFINITIONS(-DUNIX)
	IF(APPLE)
		ADD_DEFINITIONS(-DAPPLE)
	ENDIF(APPLE)
ENDIF(UNIX)
