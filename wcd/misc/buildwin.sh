#!/bin/sh

set -e
set -x

cd ../win32
make mostlyclean all strip install dist UNINORM=1
make mostlyclean all strip install dist
make mostlyclean all strip install dist ENABLE_NLS=
cd ../win32psh
make mostlyclean all strip install dist UNINORM=1
make mostlyclean all strip install dist
make mostlyclean all strip install dist ENABLE_NLS=
cd ../win32zsh
make mostlyclean all strip install dist
cd ../win64
make mostlyclean all strip install dist UNINORM=1
make mostlyclean all strip install dist
cd ../win64psh
make mostlyclean all strip install dist UNINORM=1
make mostlyclean all strip install dist

