set_project("Monado")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

set_languages("c++20")

if is_plat("windows") then
    add_cxflags("/utf-8")
    add_defines("MND_PLATFORM_WINDOWS")
end

if is_mode("debug") then
    add_defines("MND_DEBUG", "MND_PROFILE")
end

add_includedirs("include")

add_requires("opengl") 
add_requires("glad")
add_requires("glfw")
add_requires("glm")
add_requires("assimp")
add_requires("stb") 
add_requires("dirent") 
add_requires("spdlog") 

target("imgui")
    add_files("./imgui/**.cpp")
    set_kind("static")
    add_packages("glfw", "glad", "stb", "dirent")

target("Monado")
    set_kind("static")
    add_files("src/**.cpp")
    add_deps("imgui")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog")

target("sandbox")
    set_kind("binary")
    add_files("test/**.cpp")
    add_deps("Monado")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog")
    set_rundir("$(projectdir)")
