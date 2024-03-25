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
elseif is_mode("release") then
    add_defines("MND_RELEASE")
end

add_defines("STB_IMAGE_IMPLEMENTATION", "PX_PHYSX_STATIC_LIB", "NDEBUG")

add_includedirs("imgui/include")
add_includedirs("monado/include")
add_includedirs("alvis/include")
add_includedirs("extern/FastNoise")
add_includedirs("extern/PhysX/include")
add_includedirs("extern/PxShared/include")

-- add_includedirs("extern/msdf-atlas-gen/msdf-atlas-gen")
-- add_includedirs("extern/msdf-atlas-gen/msdfgen")

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

target("FastNoise")
    add_files("./extern/FastNoise/FastNoise.cpp") 
    set_kind("static")

target("imgui")
    add_files("./imgui/src/**.cpp") 
    set_kind("static")
    add_packages("glfw", "glad", "stb")

target("monado")
    set_kind("static")
    add_files("monado/src/**.cpp")
    add_deps("imgui", "FastNoise")
    if is_mode("debug") then 
        add_links("./libs/mono/lib/Debug/mono-2.0-sgen.lib")
    else
        add_links("./libs/mono/lib/Release/mono-2.0-sgen.lib")
    end
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysX_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXCharacterKinematic_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXCommon_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXCooking_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXExtensions_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXFoundation_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXPvdSDK_static_64.lib")
    add_links("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PhysXVehicle_static_64.lib")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d", "yaml-cpp", "assimp")
    after_build(function (target)
        os.cp("monado/assets", target:targetdir() .. "/monado/")
    end)

target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_links("libs/win/ComDlg32")
    add_deps("monado")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    after_build(function (target)
        os.cp("sandbox/assets", target:targetdir() .. "/sandbox/")
    end)

target("alvis")
    set_kind("binary")
    add_files("alvis/**.cpp")
    add_deps("monado") 
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    add_links("libs/win/ComDlg32")
    after_build(function (target)
        os.cp("alvis/assets", target:targetdir() .. "/alvis/")
        -- os.cp("./extern/PhysX/bin/win.x86_64.vc143.mt/checked/PVDRuntime_64.dll", target:targetdir())
        if is_mode("debug") then
            os.cp("./libs/mono/bin/Debug/**.*", target:targetdir())
        else
            os.cp("./libs/mono/bin/Release/**.*", target:targetdir())
        end
        os.runv("buildScript.bat", {target:targetdir()})
    end)

