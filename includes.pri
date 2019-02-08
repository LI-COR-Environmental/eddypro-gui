# include paths
unix{
    INCLUDEPATH += $$QUAZIP_INC
} else {
    INCLUDEPATH += $$_PRO_FILE_PWD_/libs/quazip-0.7.1/quazip
    INCLUDEPATH += $$_PRO_FILE_PWD_/../../../libs/c++/boost_1_61_0
}
INCLUDEPATH += $$_PRO_FILE_PWD_/src/lisp_parser
