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
add_includedirs("extern/msdf-atlas-gen/msdf-atlas-gen")
add_includedirs("extern/msdf-atlas-gen/msdfgen")

-- msdf-atlas-gen 
package("msdf-atlas-gen")
    add_deps("cmake")   
    set_sourcedir(path.join(os.scriptdir(), "extern/msdf-atlas-gen"))
    on_install(function (package)   
        local configs = {}   
        -- table.insert(configs, "-DBUILD_SHARED_LIBS=OFF")
        -- table.insert(configs, "-DMSDF_ATLAS_USE_VCPKG=OFF")
        -- table.insert(configs, "-DMSDF_ATLAS_USE_SKIA=OFF")  
        -- table.insert(configs, "-DMSDFGEN_INSTALL=ON")                           

        -- option(MSDF_ATLAS_BUILD_STANDALONE "Build the msdf-atlas-gen standalone executable" ON)
        -- option(MSDF_ATLAS_USE_VCPKG "Use vcpkg package manager to link project dependencies" ON)
        -- option(MSDF_ATLAS_USE_SKIA "Build with the Skia library" ON)
        -- option(MSDF_ATLAS_NO_ARTERY_FONT "Disable Artery Font export and do not require its submodule" OFF)
        -- option(MSDF_ATLAS_MSDFGEN_EXTERNAL "Do not build the msdfgen submodule but find it as an external package" OFF)
        -- option(MSDF_ATLAS_INSTALL "Generate installation target" OFF)   
        -- option(MSDF_ATLAS_DYNAMIC_RUNTIME "Link dynamic runtime library instead of static" OFF)
        -- option(BUILD_SHARED_LIBS "Generate dynamic library files instead of static" OFF)
        import("package.tools.cmake").install(package, configs)
    end)   
package_end()

add_requires("opengl") 
add_requires("glad")
add_requires("glfw")
add_requires("glm")
add_requires("stb") 
add_requires("spdlog") 
add_requires("entt") 
add_requires("box2d") 
add_requires("yaml-cpp") 
add_requires("msdf-atlas-gen") 

target("imgui")
    add_files("./imgui/**.cpp") 
    set_kind("static")
    add_packages("glfw", "glad", "stb")

target("monado")
    set_kind("static")
    add_files("src/**.cpp")
    add_deps("imgui")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d", "yaml-cpp", "msdf-atlas-gen")

target("editor")
    set_kind("binary")
    add_files("editor/**.cpp")
    set_runargs("Sandbox.mproj")
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
        os.cp("$(projectdir)/asset/**.mproj", target:targetdir())
    end)

target("sandbox")
    set_kind("binary")
    add_files("sandbox/**.cpp")
    add_deps("monado", "editor")
    add_packages("opengl", "glfw", "glad", "stb", "glm", "stb", "spdlog", "entt", "box2d")
    after_build(function (target)
        os.cp("$(projectdir)/asset", target:targetdir())
    end)
