import os
import json
import shutil
import subprocess

def run_command(command, cwd=None):
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, cwd=cwd)
    stdout, stderr = process.communicate()
    if process.returncode != 0:
        print(f"Error executing command: {command}")
        print(stderr.decode())
        exit(1)
    return stdout.decode()

# 1. Read target-locked.json and clone dependencies
with open('target-locked.json') as f:
    config = json.load(f)

# Create a libraries directory
if not os.path.exists('libraries'):
    os.makedirs('libraries')

# Clone dependencies
for lib in config['libraries']:
    lib_name = lib['name']
    lib_url = lib['url']
    lib_branch = lib['branch']
    lib_path = f"libraries/{lib_name}"
    if not os.path.exists(lib_path):
        run_command(f"git clone {lib_url} {lib_path}")
        run_command(f"git checkout {lib_branch}", cwd=lib_path)

# Create a CMakeLists.txt in the libraries directory
with open('libraries/CMakeLists.txt', 'w') as f:
    for lib in config['libraries']:
        f.write(f"add_subdirectory({lib['name']})\n")

# 2. Create a build directory
if os.path.exists('build'):
    shutil.rmtree('build')
os.makedirs('build')

# 3. Run CMake
os.chdir('build')
run_command("cmake ..")

# 4. Build the project
run_command("make")

# 5. Copy the hex file to the root
os.chdir('..')
shutil.copyfile('build/MICROBIT.hex', 'MICROBIT.hex')

print("Build successful! MICROBIT.hex is in the root directory.")