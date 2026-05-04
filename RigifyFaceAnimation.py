import bpy
import json
#
# Viseme to rigify face rig action generator
#
boneList = [
    "chin",
    "nose.002",
    "nose.R.001",
    "nose.L.001",
    "cheek.B.R.001",
    "cheek.B.L.001",
    "lip.B",
    "lip.T",
    "lip.T.R.001",
    "lip.T.L.001",
    "lip.B.R.001",
    "lip.B.L.001",
    "lip_end.R.001",
    "lip_end.L.001",
    "lid.T.R.002",
    "lid.T.L.002",
    "lid.B.R.002",
    "lid.B.L.002",
    "brow.T.R.002",
    "brow.T.L.002",
    "brow.T.R.003",
    "brow.T.L.003"
]

# set the face bones to base position
def zeroViseme(rig):
    for i in boneList:
        rig.pose.bones[i].location[0] = 0
        rig.pose.bones[i].location[1] = 0
#
# key a frame
def keyframeVisime(rig,f):
    try:
        for i in boneList:
            rig.pose.bones[i].keyframe_insert(data_path='location')
    except Exception as error:
        print("keyframeVisime Exception:",error)

  
def loadRigifyLoad(context,rig,scale, filepath):
    context.view_layer.objects.active = rig
    bpy.ops.object.mode_set(mode='POSE')
    with open(filepath,'r') as json_data:
        data = json.load(json_data)
        json_data.close()
        # number of records
        count = data["Count"]
        print(count)
        # array of data records
        rec = data["Data"] 
        frame = 2  
        # each record is a dictonary of z and x values
        for i in rec:
            bpy.context.scene.frame_set(frame)
            for j in boneList:
                # pair of values
                k = i[j]
                rig.pose.bones[j].location[0] = k[0] * scale
                rig.pose.bones[j].location[1] = k[1] * scale
                rig.pose.bones[j].keyframe_insert(data_path='location')
            frame = frame + 1

  
class RigifyLoadLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Animation Load'
    filter_glob: bpy.props.StringProperty(default = '*.rfy',options = {'HIDDEN'})
    filepath: bpy.props.StringProperty(subtype="FILE_PATH")
    def execute(self, context):
        print("filepath=", self.filepath)
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        scaleV = context.scene.scaleVisime
        loadRigifyLoad(context,rig,scaleV,self.filepath)
        # load the RigifyLoad file
        return {'FINISHED'}

    def invoke(self, context, event): # See comments at end  [1]        
        context.window_manager.fileselect_add(self)  
        #Open browser, take reference to 'self' read the path to selected 
        #file, put path in predetermined data structure self.filepath
        return {'RUNNING_MODAL'}  
        # Tells Blender to hang on for the slow user input
    
    
# Get the Armature    
class RigifyLoadLoadPanel(bpy.types.Panel):
    bl_label = 'Animation Load'
    bl_idname = 'cc.RigifyLoadload'
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    
    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.prop_search(context.scene, "armature", context.scene,"objects", text="Rig")
        row = layout.row()
        row.prop(context.scene,"scaleVisime")
        row = layout.row()
        row.operator('cc.control_load', text='Load') 
 
 
 
def add_properties():
    bpy.types.Scene.armature =  bpy.props.StringProperty(
        name="armature",
        description="Rig to add face animation to",
        default="Armature",
    )
  
    bpy.types.Scene.scaleVisime =  bpy.props.FloatProperty(
        name="scaleVisime",
        description="Action Scaling",
        default=0.01
    )
    
   
 
 
def remove_properties():
    del bpy.types.Scene.armature
    del bpy.types.Scene.scaleVisime

 
 
CLASSES = [
    RigifyLoadLoadOperator,
    RigifyLoadLoadPanel,
]

def register():
    for klass in CLASSES:
        bpy.utils.register_class(klass)
    add_properties()

def unregister():
    for klass in CLASSES:
        bpy.utils.unregister_class(klass)
    remove_properties()
        

if __name__ == '__main__':
    register()   
