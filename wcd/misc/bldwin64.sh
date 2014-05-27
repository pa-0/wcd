#!/bin/sh

set -e
set -x

cd ../win64
make mostlyclean all strip install dist ENABLE_NLS=  UCS=
make mostlyclean all strip install dist ENABLE_NLS=1 UCS=
make mostlyclean all strip install dist ENABLE_NLS=  UNINORM=1
make mostlyclean all strip install dist ENABLE_NLS=1 UNINORM=1
cd ../win64psh
make mostlyclean all strip install dist ENABLE_NLS=  UCS=
make mostlyclean all strip install dist ENABLE_NLS=1 UCS=
make mostlyclean all strip install dist ENABLE_NLS=  UNINORM=1
make mostlyclean all strip install dist ENABLE_NLS=1 UNINORM=1
cd ../win64zsh
make mostlyclean all strip install dist ENABLE_NLS=  UCS=
make mostlyclean all strip install dist ENABLE_NLS=1 UCS=
make mostlyclean all strip install dist ENABLE_NLS=  UNINORM=1
make mostlyclean all strip install dist ENABLE_NLS=1 UNINORM=1

