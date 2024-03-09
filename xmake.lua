set_project("Monado")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

set_languages("c++20")

if is_plat("windows") then
    add_cxflags("/utf-8")
    add_defines("MND_PLATFORM_WINDOWS")
end

if is_mode("debug") then
    -- add_defines("MND_DEBUG", "MND_PROFILE")
    add_defines("MND_DEBUG")
end

add_defines("STB_IMAGE_IMPLEMENTATION")

add_includedirs("include")

package("yaml-cpp")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "extern/yaml-cpp"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        -- table.insert(configs, "-DBUILD_SHARED_LIBS=OFF" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=ON")
        -- table.insert(configs, "-DYAML_CPP_MAIN_PROJECT=OFF")
        -- table.insert(configs, "-DYAML_BUILD_SHARED_LIBS=OFF")
        import("package.tools.cmake").install(package, configs)
    end)
package_end()

-- rttr 
package("rttr")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "extern/rttr"))
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

add_requires("opengl") 
add_requires("glad")
add_requires("glfw")
add_requires("glm")
add_requires("assimp")
add_requires("stb") 
add_requires("dirent") 
add_requires("spdlog") 
add_requires("entt") 
add_requires("box2d") 
add_requires("yaml-cpp") 
add_requires("rttr") 


target("imgui")
    add_files("./imgui/**.cpp")
    set_kind("static")
    add_packages("glfw", "glad", "stb", "dirent")

target("monado")
    set_kind("static")
    add_files("src/**.cpp")
    add_deps("imgui")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d", "yaml-cpp")

target("editor")
    set_kind("binary")
    add_files("editor/**.cpp")
    add_deps("monado")
    add_links("ComDlg32")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d", "yaml-cpp")
    set_rundir("$(projectdir)")

target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_deps("monado", "editor")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    set_rundir("$(projectdir)")
