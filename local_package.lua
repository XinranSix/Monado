-- rttr 
package("rttr")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "extern" .. pathSeparator .. "rttr"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_UNIT_TESTS=OFF")
        table.insert(configs, "-DBUILD_BENCHMARKS=OFF")
        table.insert(configs, "-DBUILD_BENCHMARKS=OFF")
        table.insert(configs, "-DBUILD_EXAMPLES=OFF")
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

-- fast_io 
package("fast_io")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "extern" .. pathSeparator .. "fast_io"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

add_requires("rttr") 
add_requires("fast_io") 
