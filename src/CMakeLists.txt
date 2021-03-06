##################################################
#Macro for searching subdirectories
##################################################
MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h *.hpp *.cpp)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        # remove ":" from path
        STRING(REGEX REPLACE ":" "_" dir_path ${dir_path})

        # convert "/" to "_"
        STRING(REGEX REPLACE "/" "_" dir_path ${dir_path})

        # remove build subdir from path
        STRING(REGEX REPLACE "${root}_" "" dir_path ${dir_path})
		
		# remove prefixes		
		STRING(REGEX REPLACE "public_" "" dir_path ${dir_path})
		STRING(REGEX REPLACE "private_" "" dir_path ${dir_path})		

		#insert again the "/" characters
		STRING(REGEX REPLACE "_" "\\\\" dir_path ${dir_path})
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

##################################################
#useful variables for make source groups
##################################################
SET(root ${CMAKE_CURRENT_SOURCE_DIR})
STRING(REGEX REPLACE ":" "_" root ${root})
STRING(REGEX REPLACE "/" "_" root ${root})

##################################################
#Enable C++11 mode
##################################################
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_STD_CXX11)
    check_cxx_compiler_flag(--std=c++0x SUPPORTS_STD_CXX01)
    if(SUPPORTS_STD_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11")
    elseif(SUPPORTS_STD_CXX01)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++0x")
    else()
        message(ERROR "Compiler does not support --std=c++11 or --std=c++0x.")
    endif()
endif()

##################################################
#Add symbols definitions for GC and boost compatibilities
##################################################
ADD_DEFINITIONS("-DGC_THREADS")
IF(WIN32)
ADD_DEFINITIONS("-DWIN32_LEAN_AND_MEAN")
ENDIF()

##################################################
#Add subdirectories (public and private if exists)
##################################################
ADD_SUBDIRECTORY(public)
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/private/CMakeLists.txt")
	MESSAGE("Private library found")
	ADD_SUBDIRECTORY(private)
ELSE()
	MESSAGE("Private library not found")
ENDIF()

##################################################
#Get System library code
##################################################
GET_PROPERTY(SysLib GLOBAL PROPERTY SysSRC)

##################################################
#Get files
##################################################
HEADER_DIRECTORIES(SUBDIRS)

##################################################
#Add groups
##################################################
FOREACH(subdir ${SUBDIRS})
	SET(subdir_cpp "Source Files\\${subdir}")
	SET(subdir_h "Header Files\\${subdir}")
	FILE(GLOB tmpcppPub "public/${subdir}/*.cpp")
	FILE(GLOB tmpcppPrv "private/${subdir}/*.cpp")
	SET(tmpcpp ${tmpcppPub} ${tmpcppPrv})
	SOURCE_GROUP(${subdir_cpp} FILES ${tmpcpp})
	FILE(GLOB tmphPub "public/${subdir}/*.h")
	FILE(GLOB tmphPrv "private/${subdir}/*.h")
	SET(tmph ${tmphPub} ${tmphPrv})
	SOURCE_GROUP(${subdir_h} FILES ${tmph})
ENDFOREACH()

INCLUDE_DIRECTORIES(public)
ADD_LIBRARY(System ${SysLib})
SET_TARGET_PROPERTIES(System PROPERTIES LINKER_LANGUAGE CXX)

##################################################
#Find boost libraries
##################################################
GET_PROPERTY(IS_RELEASE GLOBAL PROPERTY GL_IS_RELEASE)

SET(Boost_USE_STATIC_LIBS        ON)
SET(Boost_USE_MULTITHREADED      ON)
SET(Boost_USE_STATIC_RUNTIME    OFF)

FIND_PACKAGE( Boost REQUIRED COMPONENTS system locale regex filesystem)

IF(Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
	TARGET_LINK_LIBRARIES(System ${Boost_LIBRARIES})
ELSE()
	MESSAGE("Please install boost libraries before using AlterNative. 1-55 or higher version recommended")
ENDIF()
