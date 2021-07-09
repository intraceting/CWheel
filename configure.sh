#!/bin/bash
#
# This file is part of ABCDK.
#  
# MIT License
##


# Functions
checkReturnCode()
{
    rc=$?
    if [ $rc != 0 ];then
        exit $rc
    fi
}

#
CheckSystemName()
# $1 System Name
{
    if [ -f /etc/os-release ];then 
	    grep '^ID=' /etc/os-release |cut -d = -f 2 |sed 's/\"//g' | grep -iE "${1}" |wc -l
    elif [ -f /usr/lib/os-release ];then 
        grep '^ID=' /usr/lib/os-release |cut -d = -f 2 |sed 's/\"//g' | grep -iE "${1}" |wc -l
    else 
        echo "0"
    fi 
}

#
CheckPackageKitName()
{
	if [ $(CheckSystemName "Ubuntu|Debian") -ge 1 ];then
		echo "DEB"
	elif [ $(CheckSystemName "CentOS|Red Hat|RedHat|RHEL|fedora|Amazon|Oracle") -ge 1 ];then
		echo "RPM"
	else
		echo ""
	fi
}

#
CheckHavePackageFromKit()
# $1 KIT_NAME
# $1 PACKAGE
{
    #
    STATUS="1"

    #
    KIT_NAME="$1"
    PACKAGE="$2"

    #
	if [ "DEB" == "${KIT_NAME}" ];then 
        STATUS=$(dpkg -V ${PACKAGE} >> /dev/null 2>&1 ; echo $?)
	elif [ "RPM" == "${KIT_NAME}" ];then
		STATUS=$(rpm -q ${PACKAGE} >> /dev/null 2>&1 ; echo $?)
    fi

	#
	echo "${STATUS}"
}

#
CheckHavePackageFromWhich()
# $1 KIT_NAME
# $2 PACKAGE
{
    #
    STATUS="1"

    #
    KIT_NAME="$1"
    PACKAGE="$2"

    #
    STATUS=$(which ${PACKAGE} >> /dev/null 2>&1 ; echo $?)

	#
	echo "${STATUS}"
}

#
CheckHavePackage()
# $1 KIT_NAME
# $2 PACKAGE
{
    # 0 is Ok, otherwise not Ok.
    STATUS="1" 
    NAMES=""

    # 1 is KIT,2 is WHICH.
    METHOD=1

    #
    KIT_NAME="$1"
    PACKAGE="$2"

    #
	if [ "DEB" == "${KIT_NAME}" ];then 
	{   
        if [ "${PACKAGE}" == "openmp" ];then
            NAMES="libomp-dev"
        elif [ "${PACKAGE}" == "unixodbc" ];then
            NAMES="unixodbc-dev"
        elif [ "${PACKAGE}" == "sqlite" ];then
            NAMES="libsqlite3-dev"
        elif [ "${PACKAGE}" == "openssl" ];then
            NAMES="libssl-dev"
        elif [ "${PACKAGE}" == "ffmpeg" ];then
            NAMES="libswscale-dev libavutil-dev"
        elif [ "${PACKAGE}" == "freeimage" ];then
            NAMES="libfreeimage-dev"
        elif [ "${PACKAGE}" == "pkgconfig" ];then
        {
            METHOD=2
            NAMES="pkg-config"
        }
        fi     
    }
	elif [ "RPM" == "${KIT_NAME}" ];then
	{
        if [ "${PACKAGE}" == "openmp" ];then
            NAMES="gcc"
        elif [ "${PACKAGE}" == "unixodbc" ];then
            NAMES="unixODBC-devel"
        elif [ "${PACKAGE}" == "sqlite" ];then
            NAMES="sqlite-devel"
        elif [ "${PACKAGE}" == "openssl" ];then
            NAMES="openssl-devel"
        elif [ "${PACKAGE}" == "ffmpeg" ];then
            NAMES="ffmpeg-devel"
        elif [ "${PACKAGE}" == "freeimage" ];then
            NAMES="freeimage-devel"
        elif [ "${PACKAGE}" == "pkgconfig" ];then
        {
            METHOD=2
            NAMES="pkg-config"
        }
        fi
    }
    fi 

    #
    if [ ${METHOD} -eq 1 ];then
        STATUS=$(CheckHavePackageFromKit ${KIT_NAME} "${NAMES}")
    elif [ ${METHOD} -eq 2 ];then
        STATUS=$(CheckHavePackageFromWhich ${KIT_NAME} "${NAMES}")
    fi

	#
	echo "${STATUS}"
}

#
CheckKeyword()
# $1 keywords
# $2 word
{
	NUM=$(echo "$1" |grep -wi "$2" | wc -l)
    echo ${NUM}
}

#
SOLUTION_NAME=abcdk

#
SHELL_PWD=$(cd `dirname $0`; pwd)
KIT_NAME=$(CheckPackageKitName)

#
BUILD_PATH=$(realpath "${SHELL_PWD}/build/")
MAKE_CONF=${BUILD_PATH}/makefile.conf

#
HOST_PLATFORM=$(uname -m)
TARGET_PLATFORM=${HOST_PLATFORM}

#
VERSION_MAJOR="1"
VERSION_MINOR="1"
VERSION_DATETIME=$(date -u +"%Y-%m-%dT%H:%M:%SZ")

#
DEPEND_FUNC="Nothing"
BUILD_TYPE="release"
INSTALL_PREFIX="/usr/local/${SOLUTION_NAME}/"

#
PrintUsage()
{
    echo "usage: [ OPTIONS ]"
    echo -e "\n\t-d < KEY,KEY,... >"
    echo -e "\t\t依赖项目。关键字：have-openmp,have-unixodbc,have-sqlite,have-openssl,have-ffmpeg,have-freeimage"
    echo -e "\n\t-g"
    echo -e "\t\t生成调试符号。默认：关闭。"
    echo -e "\n\t-i < PATH >"
    echo -e "\t\t安装路径。默认：${INSTALL_PREFIX}"
}

#
while getopts "?d:gi:" ARGKEY 
do
    case $ARGKEY in
    \?)
        PrintUsage
        exit 22
    ;;
    d)
        DEPEND_FUNC="$OPTARG"
    ;;
    g)
        BUILD_TYPE="debug"
    ;;
    i)
        INSTALL_PREFIX=$(realpath "${OPTARG}/")
    ;;
    esac
done

#
mkdir -p ${BUILD_PATH}

#
if [ ! -d ${BUILD_PATH} ];then
echo "'${BUILD_PATH}' must be an existing directory."
exit 22
fi 

#
DEPEND_FLAGS="-D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 ${DEPEND_FLAGS}"

#
DEPEND_LIBS="-ldl -pthread -lrt -lc -lm ${DEPEND_LIBS}"

#
STATUS=$(CheckHavePackage ${KIT_NAME} pkgconfig)
if [ ${STATUS} -ne 0 ];then
{
    echo "pkgconfig kit not found."
    exit 22
}
fi

#
if [ $(CheckKeyword ${DEPEND_FUNC} "have-openmp") -eq 1 ];then
{
    STATUS=$(CheckHavePackage ${KIT_NAME} openmp)
    if [ ${STATUS} -eq 0 ];then
    {
        HAVE_OPENMP="Yes"
        DEPEND_FLAGS=" -DHAVE_OPENMP -fopenmp ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -fopenmp ${DEPEND_LIBS}"
    }
    else
    {
        echo "openmp kit not found."
        exit 22
    }
    fi
}
fi

#
if [ $(CheckKeyword ${DEPEND_FUNC} "have-unixodbc") -eq 1 ];then
{
    STATUS=$(CheckHavePackage ${KIT_NAME} unixodbc)
    if [ ${STATUS} -eq 0 ];then
    {
        HAVE_UNIXODBC="Yes"
        DEPEND_FLAGS=" -DHAVE_UNIXODBC ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -lodbc ${DEPEND_LIBS}"
    }
    else
    {
        echo "unixodbc kit not found."
        exit 22
    }
    fi
}
fi

#
if [ $(CheckKeyword ${DEPEND_FUNC} "have-sqlite") -eq 1 ];then
{
    STATUS=$(CheckHavePackage ${KIT_NAME} sqlite)
    if [ ${STATUS} -eq 0 ];then
    {
        HAVE_SQLITE="Yes"
        DEPEND_FLAGS=" -DHAVE_SQLITE ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags sqlite3) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs sqlite3) ${DEPEND_LIBS}"
    }
    else
    {
        echo "sqlite kit not found."
        exit 22
    }
    fi
}
fi

#
if [ $(CheckKeyword ${DEPEND_FUNC} "have-openssl") -eq 1 ];then
{
    STATUS=$(CheckHavePackage ${KIT_NAME} openssl)
    if [ ${STATUS} -eq 0 ];then
    {
        HAVE_OPENSSL="Yes"
        DEPEND_FLAGS=" -DHAVE_OPENSSL ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags openssl) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs openssl) ${DEPEND_LIBS}"
    }
    else
    {
        echo "openssl kit not found."
        exit 22
    }
    fi
}
fi

#
if [ $(CheckKeyword ${DEPEND_FUNC} "have-ffmpeg") -eq 1 ];then
{
    STATUS=$(CheckHavePackage ${KIT_NAME} ffmpeg)
    if [ ${STATUS} -eq 0 ];then
    {
        HAVE_FFMPEG="Yes"
        DEPEND_FLAGS=" -DHAVE_FFMPEG ${DEPEND_FLAGS}"
        DEPEND_FLAGS=" $(pkg-config --cflags libswscale libavutil) ${DEPEND_FLAGS}"
        DEPEND_LIBS=" $(pkg-config --libs libswscale libavutil) ${DEPEND_LIBS}"
    }
    else
    {
        echo "ffmpeg kit not found."
        exit 22
    }
    fi
}
fi

#
if [ $(CheckKeyword ${DEPEND_FUNC} "have-freeimage") -eq 1 ];then
{
    STATUS=$(CheckHavePackage ${KIT_NAME} freeimage)
    if [ ${STATUS} -eq 0 ];then
    {
        HAVE_FREEIMAGE="Yes"
        DEPEND_FLAGS=" -DHAVE_FREEIMAGE ${DEPEND_FLAGS}"
        DEPEND_LIBS=" -lfreeimage ${DEPEND_LIBS}"
    }
    else
    {
        echo "freeimage kit not found."
        exit 22
    }
    fi
}
fi

#
echo "SOLUTION_NAME=${SOLUTION_NAME}"

#
echo "BUILD_PATH=${BUILD_PATH}"
echo "MAKE_CONF=${MAKE_CONF}"

#
echo "HOST_PLATFORM=${HOST_PLATFORM}"
echo "TARGET_PLATFORM=${TARGET_PLATFORM}"

#
echo "VERSION_MAJOR=${VERSION_MAJOR}"
echo "VERSION_MINOR=${VERSION_MINOR}"
echo "VERSION_DATETIME=${VERSION_DATETIME}"

#
echo "HAVE_OPENMP=${HAVE_OPENMP}"
echo "HAVE_UNIXODBC=${HAVE_UNIXODBC}"
echo "HAVE_SQLITE=${HAVE_SQLITE}"
echo "HAVE_OPENSSL=${HAVE_OPENSSL}"
echo "HAVE_FFMPEG=${HAVE_FFMPEG}"
echo "HAVE_FREEIMAGE=${HAVE_FREEIMAGE}"

#
echo "BUILD_TYPE=${BUILD_TYPE}"
echo "INSTALL_PREFIX=${INSTALL_PREFIX}"
echo "ROOT_PATH?=/"

#
echo "#" > ${MAKE_CONF}
echo "# ABCDK is abcdk!" >> ${MAKE_CONF}
echo "#" >> ${MAKE_CONF}
echo "" >> ${MAKE_CONF}

#
echo "SOLUTION_NAME = ${SOLUTION_NAME}" >> ${MAKE_CONF}
echo "BUILD_PATH = ${BUILD_PATH}" >> ${MAKE_CONF}
echo "HOST_PLATFORM = ${HOST_PLATFORM}" >> ${MAKE_CONF}
echo "TARGET_PLATFORM = ${TARGET_PLATFORM}" >> ${MAKE_CONF}
echo "VERSION_MAJOR = ${VERSION_MAJOR}" >> ${MAKE_CONF}
echo "VERSION_MINOR = ${VERSION_MINOR}" >> ${MAKE_CONF}
echo "VERSION_DATETIME = ${VERSION_DATETIME}" >> ${MAKE_CONF}
echo "DEPEND_FLAGS = ${DEPEND_FLAGS}" >> ${MAKE_CONF}
echo "DEPEND_LIBS = ${DEPEND_LIBS}" >> ${MAKE_CONF}
echo "BUILD_TYPE = ${BUILD_TYPE}" >> ${MAKE_CONF}
echo "INSTALL_PREFIX = ${INSTALL_PREFIX}" >> ${MAKE_CONF}
echo "ROOT_PATH ?= /" >> ${MAKE_CONF}