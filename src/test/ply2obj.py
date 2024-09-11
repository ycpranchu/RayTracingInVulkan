import aspose.threed as a3d

scene = a3d.Scene.from_file("kitchen.ply")
scene.save("kitchen.obj")