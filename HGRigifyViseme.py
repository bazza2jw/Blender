import bpy
#
# Viseme to rigify face rig action generator for Human Generator 
#
# set the face bones to base position
def zeroViseme(rig):
    rig.pose.bones["mouth_dimple_R"].location[0] = 0
    rig.pose.bones["mouth_dimple_L"].location[0] = 0
    rig.pose.bones["mouth_upper_up_R"].location[2] = 0
    rig.pose.bones["mouth_upper_up_L"].location[2] = 0
    rig.pose.bones["mouth_shrug_roll_upper"].location[2] = 0
    rig.pose.bones["mouth_lower_down_R"].location[2] = 0
    rig.pose.bones["mouth_lower_down_L"].location[2] = 0
    rig.pose.bones["mouth_roll_lower"].location[2] = 0
    rig.pose.bones["jaw_dwn_mouth_clsd"].location[2] = 0
    rig.pose.bones["jaw_open_lt_rt_frwd"].location[2] = 0
    rig.pose.bones["mouth_smile_frown_R"].location[2] = 0
    rig.pose.bones["mouth_smile_frown_L"].location[2] = 0
    rig.pose.bones["pucker_cheekPuf"].location[1] = 0
    

def setViseme(rig, vis,scale):
    try:
        # put in rest state
        zeroViseme(rig)
        if vis == "rest":
    #            ["lips_part", 0.6]
            rig.pose.bones["mouth_lower_down_R"].location[2] = -0.04 * scale
            rig.pose.bones["mouth_lower_down_L"].location[2] = -0.04 * scale
    
        elif vis == "etc":   
    #            ["lips_part", 0.4]
            rig.pose.bones["mouth_lower_down_R"].location[2] = -0.05 * scale
            rig.pose.bones["mouth_lower_down_L"].location[2] = -0.05 * scale
         
        elif vis == "AI":
    #            ["lips_mid_upper_up_left", 0.4],
    #            ["lips_mid_upper_up_right", 0.4],
    #            ["mouth_open", 0.7]
            rig.pose.bones["jaw_open_lt_rt_frwd"].location[2] = 0.03 * scale
            rig.pose.bones["mouth_lower_down_R"].location[2] = -1.0 * scale
            rig.pose.bones["mouth_lower_down_L"].location[2] = -1.0 * scale

        
        elif vis == "O":
    #            ["mouth_narrow_left", 0.9],
    #            ["mouth_narrow_right", 0.9],
    #            ["mouth_open", 0.8]
            rig.pose.bones["jaw_open_lt_rt_frwd"].location[2] = -0.02 * scale
            rig.pose.bones["pucker_cheekPuf"].location[1] = 0.8 * scale


          
        elif vis == "U":
    #            ["mouth_narrow_left", 1.0],
    #            ["mouth_narrow_right", 1.0],
    #            ["mouth_open", 0.4]
            rig.pose.bones["jaw_open_lt_rt_frwd"].location[2] = -0.01 * scale
            rig.pose.bones["pucker_cheekPuf"].location[1] = 1.0 * scale
        
        elif vis == "WQ":
    #         ["lips_mid_upper_up_left", 0.4],
    #         ["lips_mid_upper_up_right", 0.4],
            rig.pose.bones["mouth_lower_down_R"].location[2] = -0.05 * scale
            rig.pose.bones["mouth_lower_down_L"].location[2] = -0.05 * scale
       
        elif vis == "L": 
    #            ["lips_mid_upper_up_left", 0.5],
    #            ["lips_mid_upper_up_right", 0.5],
    #            ["lips_mid_lower_down_left", 0.5],
    #            ["lips_mid_lower_down_right", 0.5],
    #            ["mouth_open", 0.2],
    #            ["tongue_back_up", 1.0],
    #            ["tongue_up", 1.0]
            rig.pose.bones["jaw_open_lt_rt_frwd"].location[2] = -0.02 * scale
            rig.pose.bones["mouth_dimple_R"].location[0] = 0.2 * scale
            rig.pose.bones["mouth_dimple_L"].location[0] = -0.2 * scale

         
        elif vis == "E": 
    #            ["lips_mid_upper_up_left", 0.5],
    #            ["lips_mid_upper_up_right", 0.5],
    #            ["lips_mid_lower_down_left", 0.6],
    #            ["lips_mid_lower_down_right", 0.6],
    #            ["mouth_open", 0.25]
            rig.pose.bones["jaw_open_lt_rt_frwd"].location[2] = -0.02 * scale
            rig.pose.bones["mouth_dimple_R"].location[0] = 0.2 * scale
            rig.pose.bones["mouth_dimple_L"].location[0] = -0.2 * scale
            rig.pose.bones["mouth_smile_frown_R"].location[2] = 0.06 * scale
            rig.pose.bones["mouth_smile_frown_L"].location[2] = 0.06 * scale

            
        elif vis == "MBP":
            # reset to zero
            rig.pose.bones["mouth_dimple_R"].location[0] = 0.02 * scale
            rig.pose.bones["mouth_dimple_L"].location[0] = -0.02 * scale
        elif vis == "FV":
    #           ["mouth_wide_left", 0.2],
    #            ["mouth_wide_right", 0.2],
    #            ["lips_part", 1.0],
    #            ["lips_mid_lower_up_left", 0.3],
    #            ["lips_mid_lower_up_right", 0.3],
    #            ["lips_lower_in", 0.6]
            rig.pose.bones["mouth_dimple_R"].location[0] = 0.02 * scale
            rig.pose.bones["mouth_dimple_L"].location[0] = -0.02 * scale
            rig.pose.bones["mouth_roll_lower"].location[2] = 0.02 * scale

        else:
            # ignore
            pass
        
    except Exception as error:
        print("setViseme Exception:",error)

# key a frame
def keyframeVisime(rig,f):
    try:
       rig.pose.bones["mouth_dimple_R"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_dimple_L"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_upper_up_R"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_upper_up_L"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_shrug_roll_upper"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_lower_down_R"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_lower_down_L"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_roll_lower"].keyframe_insert(data_path='location')
       rig.pose.bones["jaw_dwn_mouth_clsd"].keyframe_insert(data_path='location')
       rig.pose.bones["jaw_open_lt_rt_frwd"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_smile_frown_R"].keyframe_insert(data_path='location')
       rig.pose.bones["mouth_smile_frown_L"].keyframe_insert(data_path='location')
       rig.pose.bones["pucker_cheekPuf"].keyframe_insert(data_path='location')
    except Exception as error:
        print("keyframeVisime Exception:",error)

  
def loadMoho(context,rig,scale, filepath):
    context.view_layer.objects.active = rig
    bpy.ops.object.mode_set(mode='POSE')
    with open(filepath, "r", encoding='utf-8') as fp:
        for line in fp:
            words= line.split()
            if len(words) < 2:
                pass
            else:
                frame = int(words[0]) 
                if frame > 0:
                    bpy.context.scene.frame_set(frame)
                    try:
                        vis = words[1]
                        setViseme(rig, vis, scale)
                        keyframeVisime(rig,frame)
                    except Exception as error:
                        print("Exception:",error)
                        pass    

  
class MohoLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Moho Load'
    filter_glob: bpy.props.StringProperty(default = '*.dat',options = {'HIDDEN'})
    filepath: bpy.props.StringProperty(subtype="FILE_PATH")
    def execute(self, context):
        print("filepath=", self.filepath)
        rigName = context.scene.armature
        rig = bpy.data.objects[rigName] 
        scaleV = context.scene.scaleVisime
        loadMoho(context,rig,scaleV,self.filepath)
        # load the moho file
        return {'FINISHED'}

    def invoke(self, context, event): # See comments at end  [1]        
        context.window_manager.fileselect_add(self)  
        #Open browser, take reference to 'self' read the path to selected 
        #file, put path in predetermined data structure self.filepath
        return {'RUNNING_MODAL'}  
        # Tells Blender to hang on for the slow user input
    
    
# Get the Armature    
class MohoLoadPanel(bpy.types.Panel):
    bl_label = 'Moho Load'
    bl_idname = 'cc.mohoload'
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
    MohoLoadOperator,
    MohoLoadPanel,
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
