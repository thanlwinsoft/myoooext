PKG_NAME=openoffice.org3-dict-my
DEB_DIR=${PKG_NAME}`uname -m`/
ARCHIVE_DIR=${PKG_NAME}
PKG_VER=`tail -1 .hgtags | sed "s/.* //"`
SRC_DIR=`pwd`
export QUILT_PATCHES=debian/patches
mkdir -p ${ARCHIVE_DIR}
mkdir -p ${DEB_DIR}
rm -rf ${DEB_DIR}*
if test -f ${ARCHIVE_DIR}/${PKG_NAME}_${PKG_VER}.orig.tar.gz;
then
    cp ${ARCHIVE_DIR}/${PKG_NAME}_${PKG_VER}.orig.tar.gz ${DEB_DIR}
else
    hg archive -X debian -X .hgtags -p ${PKG_NAME}_${PKG_VER} -ttgz ${ARCHIVE_DIR}/${PKG_NAME}_${PKG_VER}.orig.tar.gz
    cp ${ARCHIVE_DIR}/${PKG_NAME}_${PKG_VER}.orig.tar.gz ${DEB_DIR}
fi
cd ${DEB_DIR}
tar -zxf ${PKG_NAME}_${PKG_VER}.orig.tar.gz
cd ${PKG_NAME}_${PKG_VER}
cp -R ${SRC_DIR}/debian debian
while quilt push; do echo; done
debuild
cd ..
lintian --pedantic -i ${PKG_NAME}_${PKG_VER}*.changes

