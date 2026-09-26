(declare-project
    :name "stbimage"
    :description "Janet binding for stb_image"
    :author "virtualgrub39"
    :license "BSD-2-Clause"
    :url "https://github.com/virtualgrub39/janet-stbimage"
    :repo "git+https://github.com/virtualgrub39/janet-stbimage.git"
    :version "0.0.1")

(declare-native
    :name "_stbimage"
    :source @["src/c/core.c"])

(declare-source
    :source @["src/stbimage"])
