set_xmakever("2.9.3")

ProjectName = "D3D12Engine"

set_project(ProjectName)

add_includedirs("Include")

add_rules("plugin.vsxmake.autoupdate")

if is_mode("release") then
  set_fpmodels("fast")
  set_optimize("fastest")
  set_symbols("hidden")
else
  add_defines("DE_DEBUG")
  set_symbols("debug")
end

set_encodings("utf-8")
set_exceptions("cxx")
set_languages("cxx23")
set_rundir("./bin/$(plat)_$(arch)_$(mode)")
set_targetdir("./bin/$(plat)_$(arch)_$(mode)")
set_warnings("allextra")
set_allowedplats("windows", "mingw")
add_cxflags("-Wno-missing-field-initializers -Werror=vla", {tools = {"clang", "gcc"}})

option("override_runtime", {description = "Override VS runtime to MD in release and MDd in debug.", default = true})

if is_plat("windows") then
  if has_config("override_runtime") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
  end
end

add_requires("directx-headers", "directxtk12", "directxtex")

rule("cp-resources")
  after_build(function (target) 
    os.cp("Resources", "./bin/$(plat)_$(arch)_$(mode)")
  end)

target(ProjectName) 
  set_kind("binary")
  add_rules("cp-resources")
  
  add_files("Source/**.cpp")
  
  for _, ext in ipairs({".hpp", ".inl"}) do
    add_headerfiles("Include/**" .. ext)
  end
  
  add_rpathdirs("$ORIGIN")

  add_packages("directx-headers", "directxtk12", "directxtex")
  add_syslinks("d3d12", "dxgi", "D3DCompiler", "user32", "kernel32", "shell32")
  add_defines("UNICODE", "_UNICODE")

includes("xmake/**.lua")