# clang static analyzer
macx {
    static-check.target = clang-static-check
    static-check.commands = $$_PRO_FILE_PWD_/scripts/test/run-clang-analyzer.sh
    QMAKE_EXTRA_TARGETS += clang-static-check
}
