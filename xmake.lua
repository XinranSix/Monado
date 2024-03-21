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

add_includedirs("imgui/include")
add_includedirs("monado/include")
add_includedirs("alvis/include")
add_includedirs("extern/msdf-atlas-gen/msdf-atlas-gen")
add_includedirs("extern/msdf-atlas-gen/msdfgen")

-- msdf-atlas-gen 
-- package("msdf-atlas-gen")
--     add_deps("cmake")   
--     set_sourcedir(path.join(os.scriptdir(), "extern/msdf-atlas-gen"))
--     on_install(function (package)   
--         local configs = {}   
--         import("package.tools.cmake").install(package, configs)
--     end)   
-- package_end()

add_requires("opengl") 
add_requires("glad")
add_requires("glfw")
add_requires("glm")
add_requires("stb") 
add_requires("spdlog") 
add_requires("entt") 
add_requires("box2d") 
add_requires("yaml-cpp") 
-- add_requires("msdf-atlas-gen") 
add_requires("assimp")

target("imgui")
    add_files("./imgui/src/**.cpp") 
    set_kind("static")
    add_packages("glfw", "glad", "stb")

target("monado")
    set_kind("static")
    add_files("monado/src/**.cpp")
    add_deps("imgui")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d", "yaml-cpp", "assimp")

target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_links("libs/win/ComDlg32")
    add_deps("monado")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    after_build(function (target)
        os.cp("sandbox/assets", target:targetdir() .. "/sandbox")
    end)

target("alvis")
    set_kind("binary")
    add_files("alvis/**.cpp")
    add_links("libs/win/ComDlg32")
    add_deps("monado")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    after_build(function (target)
        os.cp("alvis/assets", target:targetdir() .. "/alvis")
    end)

