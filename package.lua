includes("local_package.lua")

if is_plat("windows") then
    add_requires("opencv") 
    add_requires("opengl") 
    add_requires("libsdl") 
    add_requires("glew") 
    add_requires("glad")
    add_requires("glfw")
    add_requires("glm")
    add_requires("assimp")
    add_requires("stb") 
    add_requires("libigl") 
    add_requires("openscenegraph") 
end

if is_plat("linux") then
    add_requires("ncurses") -- 一个不支持 window 的 TUI 库
    add_requires("gnu-gsl") -- The GNU Scientific Library (GSL) is a numerical library for C and C++ programmers.
    add_requires("jemalloc") -- A general purpose malloc(3) implementation that emphasizes fragmentation avoidance and scalable concurrency support
end


-- 数据可视化
add_requires("matplotplusplus") -- A C++ Graphics Library for Data Visualization
add_requires("zeromq") -- High-performance, asynchronous messaging library
add_requires("poco")
add_requires("glog") --  
add_requires("benchmark") -- 谷歌性能评估框架
add_requires("protobuf-cpp")
add_requires("cryptopp") -- 一套密码学程序库，使用了大量泛型编程技术，与 Boost 类似，几乎囊括了所有已知的公开密码学算法的实现
add_requires("wxwidgets") -- Cross-Platform C++ GUI Library
add_requires("icu4c") -- C/C++ libraries for Unicode and globalization.
add_requires("libevent") -- 事件库
add_requires("boost_ut") --  UT: C++20 μ(micro)/Unit Testing Framework


-- 音视频
add_requires("ffmpeg")

-- 组件与工具
add_requires("fmt") -- 格式化输出库，部分功能已被标准库吸收
add_requires("tbox") -- c 语言的一个组件库
add_requires("snappy") -- A fast compressor/decompressor
add_requires("boost", {configs = {all = true}}) 
add_requires("range-v3")  -- 范围库
add_requires("abseil") -- 补充标准库没有的常用功能
-- MessagePack 高效易用的序列化和反序列化工具，可以跨语言交换数据
add_requires("msgpack-cxx") -- MessagePack implementation for C++
add_requires("msgpack-c") -- MessagePack implementation for C

-- 测试
add_requires("gtest") -- google test
add_requires("catch2") -- 谷歌性能评估框架
add_requires("benchmark") -- 谷歌性能评估框架

-- 反射与序列化
add_requires("magic_enum")  -- 枚举反射
add_requires("rttr") -- 反射库

-- 日志
add_requires("spdlog") -- 一个日志库, 基于 fmt
add_requires("easyloggingpp") 

-- 数学、数值计算与算法
add_requires("gsl") -- C 数学运算库
add_requires("eigen") -- 一个线性代数库
-- add_requires("libigl", {configs = {all = true}}) -- 各种图形学算法大合集
add_requires("cgal") --CGAL is a software project that provides easy access to efficient and reliable geometric algorithms in the form of a C++ library. 

-- 数据解析库
add_requires("tinyxml2") 
add_requires("yaml-cpp") 
add_requires("rapidjson") 
add_requires("jsoncpp") 
add_requires("pugixml") -- Light-weight, simple and fast XML parser for C++ with XPath support
add_requires("jsoncons") -- A C++, header-only library for constructing JSON and JSON-like data formats, with JSON Pointer, JSON Patch, JSONPath, JMESPath, CSV, MessagePack, CBOR, BSON, UBJSON

-- GUI 和 TUI
add_requires("sfml") -- 

-- 命令行解析库
add_requires("gflags") 
add_requires("cxxopts")

-- 其他库
add_requires("box2d") -- 
add_requires("dirent") -- 

-- 并行与 SIMD
add_requires("onetbb") -- 
add_requires("openmp") -- 



