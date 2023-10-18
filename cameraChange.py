import bpy
#
# This script allows renders to be triggered only when the camera changes
# this means a change in camera id, the camera location, rotation or focal length
# This automates the enabling and disabling of layers so they are only rendered when required
# No need to keyframe and it adapts to changes in camera use. 
#
# How to use. There is a small amount of preparation. Overall this should make life that little 
# bit easier in some cases. The greater the number of cameras the more useful this is. If you have one 
# camera this is not much use unless you are driving the virtual camera  from a "real" scene camera.
#
# Create an empty called RenderControl - this is used to drive the "Use for Rendering" property
# Add a driver to each "Use for Rendering" property for each layer to be controlled by the x-rotation
# of the RenderControl. Set the expression to "var > 0.1"
# Set:
# Object to RenderControl
# Type to X Rotation
# Mode XYZ Euler
# Space World Space
#
# Run the script from Scripting. A panel will be available on the left hand side tool bar
# Click Setup. This hooks in the frame change filter
# Click Initialise. This sets the variables to initial values
# Click on the Timeline goto start. The selected render layers should all be checked
# Before rendering Save.
#
def frameCameraChangeHandler(scene):
    print("Frame Change", scene.frame_current, bpy.context.scene.camera.name, bpy.types.Scene.lastCameraName)
    cameraChanged = False
    if bpy.context.scene.camera.name != bpy.types.Scene.lastCameraName:
        cameraChanged = True
    else:
        if bpy.context.scene.camera.location[0] != bpy.types.Scene.lastLocationX:
            cameraChanged = True
            bpy.types.Scene.lastLocationX = bpy.context.scene.camera.location[0]
        elif bpy.context.scene.camera.location[1] != bpy.types.Scene.lastLocationY:
            cameraChanged = True
        elif bpy.context.scene.camera.location[2] != bpy.types.Scene.lastLocationZ:
            cameraChanged = True
        elif bpy.context.scene.camera.rotation_euler[0] != bpy.types.Scene.lastRotationX:
            cameraChanged = True
        elif bpy.context.scene.camera.rotation_euler[1] != bpy.types.Scene.lastRotationY:
            cameraChanged = True
        elif bpy.context.scene.camera.rotation_euler[2] != bpy.types.Scene.lastRotationZ:
            cameraChanged = True
        elif bpy.data.cameras[bpy.context.scene.camera.name].lens != bpy.types.Scene.lastFocalLength:
            cameraChanged = True
# Update the track            
    if cameraChanged == True:   
        print("Camera Changed")  
        bpy.types.Scene.lastCameraName  =  bpy.context.scene.camera.name     
        bpy.types.Scene.lastLocationX = bpy.context.scene.camera.location[0]
        bpy.types.Scene.lastLocationY = bpy.context.scene.camera.location[1]
        bpy.types.Scene.lastLocationZ = bpy.context.scene.camera.location[2]
        bpy.types.Scene.lastRotationX = bpy.context.scene.camera.rotation_euler[0]
        bpy.types.Scene.lastRotationY = bpy.context.scene.camera.rotation_euler[1]
        bpy.types.Scene.lastRotationZ = bpy.context.scene.camera.rotation_euler[2]
        bpy.types.Scene.lastFocalLength = bpy.data.cameras[bpy.context.scene.camera.name].lens
    if cameraChanged:    
        bpy.data.objects["RenderControl"].rotation_euler[0] = 1.0
        bpy.context.scene.camerachanged = True
    else:
        bpy.data.objects["RenderControl"].rotation_euler[0] = 0.0
        bpy.context.scene.camerachanged = False
        
              
        
                    
# has the camera change name
# if not has the location, rotation or lens changed

class CameraChangePanel(bpy.types.Panel):
    bl_label = "Render On Camera Change Panel"
    bl_idname = "OBJECT_PT_RenderOnCameraChange"
    bl_space_type = "VIEW_3D"
    bl_region_type = "TOOLS"


    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.operator('cc.initialise', text = "Initialise")
        row = layout.row()
        row.operator('cc.setup', text = "Setup")
        row = layout.row()
        row.prop(context.scene,"camerachanged")
        row = layout.row()

    

class RenderOnCameraChangeInitialise(bpy.types.Operator):
    bl_idname = "cc.initialise"
    bl_label = "Initialise"
    
    def execute(self, context):
        bpy.types.Scene.lastCameraName = ""
        bpy.types.Scene.lastLocationX = -1.0
        bpy.types.Scene.lastLocationY = -1.0
        bpy.types.Scene.lastLocationZ = -1.0
        bpy.types.Scene.lastRotationX = -1.0
        bpy.types.Scene.lastRotationY = -1.0
        bpy.types.Scene.lastRotationZ = -1.0
        bpy.types.Scene.lastFocalLength = -1.0
        bpy.data.objects["RenderControl"].rotation_euler[0] = 1.0
        bpy.context.scene.camerachanged = True
        print("Initialised")
        return {'FINISHED'}


class RenderOnCameraChangeSetup(bpy.types.Operator):
    bl_idname = "cc.setup"
    bl_label = "Setup"
    
    def execute(self, context):
# install the handler - changes after each file load        
        print("Installed Frame Handler")
        bpy.app.handlers.frame_change_pre.append(frameCameraChangeHandler)
        return {'FINISHED'}


CLASSES = [CameraChangePanel,RenderOnCameraChangeInitialise,RenderOnCameraChangeSetup]


def register():
    
    bpy.types.Scene.camerachanged =  bpy.props.BoolProperty(
        name="camerachanged",
        description="1.0 if the camera has changed since the last frame",
        default=False)
    for klass in CLASSES:
        bpy.utils.register_class(klass)

def unregister():
    for klass in CLASSES:
        bpy.utils.unregister_class(klass)
    del bpy.types.Scene.camerachanged    
        

if __name__ == '__main__':
    register()
 
