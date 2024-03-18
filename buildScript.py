import os
import subprocess

def process_directory(directory):
    file_list = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".cs"):
                file_list.append(os.path.join(root, file))
    return file_list

def compile_dll(file_list, output_file, extra_args=None):
    command = ["csc", "/debug","/target:library", "/out:" + output_file]
    if extra_args:
        command += extra_args
    command += file_list
    return subprocess.run(command).returncode

scriptCore_files = process_directory("scriptCore")
sandboxProject_files = process_directory("editor/sandboxProject/scripts")

if not scriptCore_files:
    print("No .cs files found in scriptCore directory.")
    exit()

if not sandboxProject_files:
    print("No .cs files found in editor/sandboxProject/scripts directory.")
    exit()

output_dir = "build/windows/x64/debug/bin"
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# Compile MonadoScriptCore.dll
monado_output_file = os.path.join(output_dir, "MonadoScriptCore.dll")
if compile_dll(scriptCore_files, monado_output_file) == 0:
    print(f"Compilation of MonadoScriptCore.dll succeeded. Output: {monado_output_file}")
else:
    print("Compilation of MonadoScriptCore.dll failed.")
    exit()

# Compile Sandbox.dll
sandbox_output_file = os.path.join(output_dir, "Sandbox.dll")
if compile_dll(sandboxProject_files, sandbox_output_file, ["/r:" + monado_output_file]) == 0:
    print(f"Compilation of Sandbox.dll succeeded. Output: {sandbox_output_file}")
else:
    print("Compilation of Sandbox.dll failed.")
