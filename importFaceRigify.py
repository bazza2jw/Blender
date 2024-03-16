import bpy
#
# Viseme to rigify face rig action generator
#
# set the face bones to base position
def zeroViseme(rig):
    rig.pose.bones["lip_end.R.001"].location[0] = 0
    rig.pose.bones["lip_end.L.001"].location[0]= 0
    rig.pose.bones["lip_end.R.001"].location[1]= 0
    rig.pose.bones["lip_end.L.001"].location[1]= 0
    rig.pose.bones["lip.T.R.001"].location[1]= 0
    rig.pose.bones["lip.T"].location[1]= 0
    rig.pose.bones["lip.T.L.001"].location[1]= 0
    rig.pose.bones["lip.B.L.001"].location[1]= 0
    rig.pose.bones["lip.B.R.001"].location[1]= 0
    rig.pose.bones["lip.B"].location[1]= 0
    rig.pose.bones["jaw_master"].location[1]= 0
    rig.pose.bones["jaw_master"].rotation_quaternion[0]= 1
    rig.pose.bones["jaw_master"].rotation_quaternion[1]=0
    rig.pose.bones["jaw_master"].rotation_quaternion[2]=0
    rig.pose.bones["jaw_master"].rotation_quaternion[3]=0
    rig.pose.bones["teeth.B"].location[2] = 0
    rig.pose.bones["teeth.T"].location[2] = 0
    
    
def setBone(rig,bone,pts,scale,dx):
    try:
        lv = pts.split(" ");
        
        if(len(lv) > 1):
            if(dx):
                rig.pose.bones[bone].location[0] = float(lv[0]) * scale
            else:
                rig.pose.bones[bone].location[0] = 0
                
            rig.pose.bones[bone].location[1] = float(lv[y]) * scale
            rig.pose.bones[bone].keyframe_insert(data_path='location')
    except:
        print("set Exception")
        

def setViseme(rig, rec,scale):
    # rec is a string with comma separated points/vectors (X & Y)
    # split the string on commas to get points
    # split points on spaces to get values
    try:
    # put in rest state
        zeroViseme(rig)
        #
        # upack the record
        # 
        pointList = rec.split(",");
        setBone(rig,"chin",pointList[0],scale,0);
        setBone(rig,"nose.002",pointList[1],scale,0);
        setBone(rig,"nose.R.001",pointList[2],scale,1);
        setBone(rig,"nose.L.001",pointList[3],scale,1);
        setBone(rig,"cheek.B.R.001",pointList[0],scale,1);
        setBone(rig,"cheek.B.L.001",pointList[0],scale,1);
        setBone(rig,"lip.B",pointList[0],scale,0);
        setBone(rig,"lip.T",pointList[0],scale,0);
        setBone(rig,"lip.T.R.001",pointList[0],scale,0);
        setBone(rig,"lip.T.L.001",pointList[0],scale,0);
        setBone(rig,"lip.B.R.001",pointList[0],scale,0);
        setBone(rig,"lip.B.L.001",pointList[0],scale,0);
        setBone(rig,"lip_end.R.001",pointList[0],scale,1);
        setBone(rig,"lip_end.L.001",pointList[0],scale,1);
        setBone(rig,"lid.T.R.002",pointList[0],scale,0);
        setBone(rig,"lid.T.L.002",pointList[0],scale,0);
        setBone(rig,"lid.B.R.002",pointList[0],scale,0);
        setBone(rig,"lid.B.L.002",pointList[0],scale,0);
        setBone(rig,"brow.T.R.002",pointList[0],scale,1);
        setBone(rig,"brow.T.L.002",pointList[0],scale,1);
        setBone(rig,"brow.T.R.003",pointList[0],scale,1);
        setBone(rig,"brow.T.L.003",pointList[0],scale,1); 
    except:
        pass


  
def loadFaceData(context,rig,scale, filepath):
    context.view_layer.objects.active = rig
    bpy.ops.object.mode_set(mode='POSE')
    frame = 1
    with open(filepath, "r", encoding='utf-8') as fp:
        for line in fp:
            bpy.context.scene.frame_set(frame)
            setViseme(rig, line,scale)
            frame = frame + 1
  
class FaceDataLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Face Data Load'
    filter_glob: bpy.props.StringProperty(default = '*.fdt',options = {'HIDDEN'})
    filepath: bpy.props.StringProperty(subtype="FILE_PATH")
    def execute(self, context):
        print("filepath=", self.filepath)
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        scaleV = context.scene.scaleVisime
        loadFaceData(context,rig,scaleV,self.filepath)
        # load the moho file
        return {'FINISHED'}

    def invoke(self, context, event): # See comments at end  [1]        
        context.window_manager.fileselect_add(self)  
        #Open browser, take reference to 'self' read the path to selected 
        #file, put path in predetermined data structure self.filepath
        return {'RUNNING_MODAL'}  
        # Tells Blender to hang on for the slow user input
    
    
# Get the Armature    
class FaceLoadPanel(bpy.types.Panel):
    bl_label = 'Face Data Load'
    bl_idname = 'cc.mohoload'
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    
    def draw(self, context):
        layout = self.layout
        row = layout.row()
        row.prop_search(context.scene, "armature", context.scene,"objects", text="Rig")
        row = layout.row()
        row.prop(context.scene,"viseme")
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
    FaceDataLoadOperator,
    FaceLoadPanel,
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
