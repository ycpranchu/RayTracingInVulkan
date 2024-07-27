# pip3 install trimesh
import trimesh

def ply_to_obj(ply_filename, obj_filename):
    # Load the .ply file using trimesh
    mesh = trimesh.load(ply_filename)

    # Export the mesh to .obj format
    mesh.export(obj_filename)
    
    print(f"Converted {ply_filename} to {obj_filename}")

# Example usage
ply_filename = 'path/to/your/file.ply'
obj_filename = 'path/to/your/file.obj'
ply_to_obj(ply_filename, obj_filename)