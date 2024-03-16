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
    if is_plat("windows") then
        add_links("./libs/win/ComDlg32")
    end
    add_links("./libs/mono/coreclr.import.lib")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d", "yaml-cpp")
    add_deps("monado")
    -- set_rundir("$(projectdir)")
    after_build(function (target)
        if is_plat("windows") then
            os.runv("buildScript.bat", {target:targetdir() .. "/bin/"})
        end
        os.cp("$(projectdir)/asset", target:targetdir())
        os.cp("$(projectdir)/libs/**.dll", target:targetdir())
    end)

target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_deps("monado", "editor")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
