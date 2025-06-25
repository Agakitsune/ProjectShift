
# iter over dir
import os
import subprocess
def compile_shaders(shader_dir):
    # Get all files in the directory
    files = os.listdir(shader_dir)
    
    # Filter for .glsl files
    vert_files = [f for f in files if f.endswith('.vert')]
    
    # Compile each shader file
    for shader_file in vert_files:
        shader_path = os.path.join(shader_dir, shader_file)
        file_name = os.path.splitext(shader_file)[0]
        print(f"Compiling {shader_file}...")
        subprocess.run(['glslc', shader_path, '-o', file_name + '.vert.spv'], check=True)
        # move the compiled file to the same directory
        subprocess.run(['mv', file_name + '.vert.spv', shader_dir], check=True)
        print(f"Compiled {shader_file} to {shader_file}.spv")
    

    frag_files = [f for f in files if f.endswith('.frag')]
    for shader_file in frag_files:
        shader_path = os.path.join(shader_dir, shader_file)
        file_name = os.path.splitext(shader_file)[0]
        print(f"Compiling {shader_file}...")
        subprocess.run(['glslc', shader_path, '-o', file_name + '.frag.spv'], check=True)
        # move the compiled file to the same directory
        subprocess.run(['mv', file_name + '.frag.spv', shader_dir], check=True)
        print(f"Compiled {shader_file} to {shader_file}.spv")

if __name__ == "__main__":
    shader_directory = 'assets/shaders/'
    compile_shaders(shader_directory)
    print("All shaders compiled successfully.")
