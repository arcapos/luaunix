package = "luaunix"
version = "scm-1"
source = {
   url = "git://github.com/mbalmer/luaunix"
}
description = {
   summary = "Unix bindings for Lua",
   homepage = "http://github.com/mbalmer/luaunix",
   license = "3-clause BSD",
}
dependencies = {
   "lua >= 5.1, < 5.3"
}
external_dependencies = {
   platforms = {
      linux = {
         BSD = {
            header = "bsd/bsd.h",
            library = "bsd"
         }
      }
   }
}
build = {
   type = "builtin",
   modules = {
      unix = {
         sources = {"select.c", "luaunix.c"}
      }
   },
   platforms = {
      linux = {
         modules = {
            unix = {
               libraries = {"bsd"},
               incdirs = {"$(BSD_INCDIR)"},
               libdirs = {"$(BSD_LIBDIR)"}
            }
         }
      }
   }
}
