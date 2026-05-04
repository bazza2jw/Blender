import bpy
from bpy_extras import anim_utils
from typing import Optional, List, Union, Iterable


xferList = [
    ["lip_end.R.001","cgt_face_vertex_291",'LOCAL','LOCAL'],
    ["lip.T","cgt_face_vertex_11",'LOCAL','LOCAL'],    
    ["lip.B","cgt_face_vertex_14",'LOCAL','LOCAL'],
    ["lip_end.L.001","cgt_face_vertex_61",'LOCAL','LOCAL'],
    ["lip.T.R.001","cgt_face_vertex_303",'LOCAL','LOCAL'],
    ["lip.T.L.001","cgt_face_vertex_73",'LOCAL','LOCAL'],
    ["lip.B.R.001","cgt_face_vertex_402",'LOCAL','LOCAL'],
    ["lip.B.L.001","cgt_face_vertex_179",'LOCAL','LOCAL'],
    ["chin","cgt_face_vertex_199",'LOCAL','LOCAL'],
    ["nose.002","cgt_face_vertex_4",'LOCAL','LOCAL'],
    ["nose.R.001","cgt_face_vertex_455",'LOCAL','LOCAL'],
    ["nose.L.001","cgt_face_vertex_235",'LOCAL','LOCAL'],
    ["cheek.B.R.001","cgt_face_vertex_433",'LOCAL','LOCAL'],
    ["cheek.B.L.001","cgt_face_vertex_213",'LOCAL','LOCAL'],

    ["lip.T.R.001","cgt_face_vertex_303",'LOCAL','LOCAL'],
    ["lip.T.L.001","cgt_face_vertex_73",'LOCAL','LOCAL'],
    ["lid.T.R.002","cgt_face_vertex_386",'LOCAL','LOCAL'],
    ["lid.T.L.002","cgt_face_vertex_160",'LOCAL','LOCAL'],
    ["lid.B.R.002","cgt_face_vertex_254",'LOCAL','LOCAL'],
    ["lid.B.L.002","cgt_face_vertex_88",'LOCAL','LOCAL'],
    ["brow.T.R.002","cgt_face_vertex_286",'LOCAL','LOCAL'],
    ["brow.T.L.002","cgt_face_vertex_53",'LOCAL','LOCAL'],
    ["brow.T.R.003","cgt_face_vertex_285",'LOCAL','LOCAL'],
    ["brow.T.L.003","cgt_face_vertex_55",'LOCAL','LOCAL'],
    ["brow.T.R.001","cgt_face_vertex_286",'LOCAL','LOCAL'],
    ["brow.T.L.001","cgt_face_vertex_162",'LOCAL','LOCAL'],
    ["brow.T.R","cgt_face_vertex_368",'LOCAL','LOCAL'],
    ["brow.T.L","cgt_face_vertex_34",'LOCAL','LOCAL']
]



def get_default_channelbag(
    obj: bpy.types.Object,
    create: bool = False
) -> Optional[bpy.types.ActionChannelbag]:
    """
    Get the most commonly used channelbag:
    - First animation layer
    - First strip in that layer
    - First/default channelbag in that strip

    Returns None if no animation data / action exists (unless create=True).
    """
    ad = obj.animation_data
    if not ad:
        if not create:
            return None
        ad = obj.animation_data_create()

    action = ad.action
    if not action:
        if not create:
            return None
        # Minimal creation — real add-ons often name it better
        action = bpy.data.actions.new(name=f"{obj.name}_Action")
        ad.action = action

    if not action.layers:
        if not create:
            return None
        layer = action.layers.new(name="Layer")  # or use a better name
    else:
        layer = action.layers[0]  # most scripts use first layer

    if not layer.strips:
        if not create:
            return None
        strip = layer.strips.new(name="Strip", type='REGULAR')  # adjust type if needed
    else:
        strip = layer.strips[0]  # most common: first strip

    # In many cases there's only one channelbag per strip
    if not strip.channelbags:
        if not create:
            return None
        # Create one (often implicitly tied to the "default" / main slot)
        # For single-object actions this is usually what you want
        channelbag = strip.channelbags.new()  # or .ensure(...) if available in future
    else:
        channelbag = strip.channelbags[0]

    return channelbag


def get_channelbag_for_slot(
    action: bpy.types.Action,
    slot_identifier: Union[str, bpy.types.ActionSlot],
    layer_index: int = 0,
    strip_index: int = 0,
    create: bool = False
) -> Optional[bpy.types.ActionChannelbag]:
    """
    More explicit: get channelbag for a specific ActionSlot.
    slot_identifier can be:
      - ActionSlot instance
      - string like "OBcube" or "POArmature:Hand.L" (slot name convention)
    """
    if not action.layers or layer_index >= len(action.layers):
        return None
    layer = action.layers[layer_index]

    if not layer.strips or strip_index >= len(layer.strips):
        return None
    strip = layer.strips[strip_index]

    if isinstance(slot_identifier, bpy.types.ActionSlot):
        return strip.channelbag(slot=slot_identifier, ensure=create)

    # String lookup — Blender uses special naming like "OB<name>" or "PO<arm>:<bone>"
    # This is a simple fallback — real code often loops or uses known prefixes
    for cb in strip.channelbags:
        if cb.slot and cb.slot.name == slot_identifier:
            return cb

    if create:
        # Requires knowing/creating the slot first — advanced usage
        # For simple scripts → prefer get_default_channelbag()
        print("Warning: creating channelbag with unknown slot not implemented here")
    return None


def get_fcurve(
    channelbag: bpy.types.ActionChannelbag,
    data_path: str,
    index: int = -1,
    group_name: str = "",
    create: bool = False
) -> Optional[bpy.types.FCurve]:
    """
    Get (or create) an F-Curve inside a specific channelbag.
    """
    if not channelbag:
        return None

    fc = channelbag.fcurves.find(data_path=data_path, index=index)

    if fc is None and create:
        fc = channelbag.fcurves.new(
            data_path=data_path,
            index=index,
            group_name=group_name
        )

    return fc


def find_fcurves(
    channelbag: bpy.types.ActionChannelbag,
    data_path_start: str = "",
    exact: bool = False
) -> List[bpy.types.FCurve]:
    """
    Find F-Curves in a channelbag by prefix or exact match.
    """
    if not channelbag:
        return []

    if exact:
        return [fc for fc in channelbag.fcurves if fc.data_path == data_path_start]

    return [fc for fc in channelbag.fcurves if fc.data_path.startswith(data_path_start)]


def sample_fcurve(
    fcurve: Optional[bpy.types.FCurve],
    frame: float,
    fallback: float = 0.0
) -> float:
    """Safe evaluation of an F-Curve at any frame time."""
    if fcurve is None or not fcurve.keyframe_points:
        return fallback
    return fcurve.evaluate(frame)


def get_action(obj: bpy.types.Object) -> Optional[bpy.types.Action]:
    """Quick legacy-compatible access to the action."""
    ad = obj.animation_data
    return ad.action if ad else None


# ──────────────────────────────────────────────
#          Convenience wrappers for common cases
# ──────────────────────────────────────────────

def get_fcurve_simple(
    obj: bpy.types.Object,
    data_path: str,
    index: int = -1,
    group_name: str = "",
    create: bool = False
) -> Optional[bpy.types.FCurve]:
    """Most scripts want this: default channelbag + get/create F-Curve."""
    cb = get_default_channelbag(obj, create=create)
    if not cb:
        return None
    return get_fcurve(cb, data_path, index=index, group_name=group_name, create=create)


def sample_at_frame(
    obj: bpy.types.Object,
    data_path: str,
    index: int = -1,
    frame: float = 0.0,
    fallback: float = 0.0
) -> float:
    """One-liner: sample value from default channelbag."""
    fc = get_fcurve_simple(obj, data_path, index=index)
    return sample_fcurve(fc, frame, fallback)





def get_frame_range_from_empty(empty_obj):
    """Return (start, end) frame range to bake from empty's action or scene."""
    ad = empty_obj.animation_data
    if ad and ad.action:
        ar = ad.action.frame_range
        start, end = int(ar[0]), int(ar[1])
        return start, end
    # fallback to NLA tracks (if empty uses NLA)
    if ad and ad.nla_tracks:
        min_frame = None
        max_frame = None
        for t in ad.nla_tracks:
            for s in t.strips:
                if min_frame is None or s.frame_start < min_frame:
                    min_frame = int(s.frame_start)
                if max_frame is None or s.frame_end > max_frame:
                    max_frame = int(s.frame_end)
        if min_frame is not None:
            return min_frame, max_frame
    # fallback to scene range
    scene = bpy.context.scene
    return int(scene.frame_start), int(scene.frame_end)

def ensure_pose_mode(arm_obj):
    # Make armature active and switch to pose mode
    bpy.context.view_layer.objects.active = arm_obj
    if bpy.context.mode != 'POSE':
        bpy.ops.object.mode_set(mode='POSE')


def add_transformation_constraint(
    armature_obj,
    bone_name,
    target_obj,
    target_bone_name=None,
    map_from='LOCATION',
    map_to='LOCATION',
    from_min=(0.0, 0.0, 0.0),
    from_max=(1.0, 1.0, 1.0),
    to_min=(0.0, 0.0, 0.0),
    to_max=(1.0, 1.0, 1.0),
    owner_space='WORLD',
    target_space='WORLD'
):
    """
    Adds a Transformation constraint to a pose bone.
    """

    # Ensure we're in POSE mode
    bpy.context.view_layer.objects.active = armature_obj
    bpy.ops.object.mode_set(mode='POSE')

    pose_bone = armature_obj.pose.bones.get(bone_name)
    if not pose_bone:
        raise ValueError(f"Bone '{bone_name}' not found")

    constraint = pose_bone.constraints.new(type='TRANSFORM')

    constraint.target = target_obj
    if target_bone_name:
        constraint.subtarget = target_bone_name

    constraint.map_from = map_from
    constraint.map_to = map_to

    constraint.from_min_x, constraint.from_min_y, constraint.from_min_z = from_min
    constraint.from_max_x, constraint.from_max_y, constraint.from_max_z = from_max

    constraint.to_min_x, constraint.to_min_y, constraint.to_min_z = to_min
    constraint.to_max_x, constraint.to_max_y, constraint.to_max_z = to_max

    constraint.owner_space = owner_space
    constraint.target_space = target_space

    return constraint



# inputs are names
def mapEmptyToBone(rig, empty, bone, scale,owner,target):
    empty_obj = bpy.data.objects.get(empty)
    scene = bpy.context.scene
    if empty_obj is None:
        raise RuntimeError(f"Empty named '{empty}' not found.")
    if  rig:
        arm_obj = bpy.data.objects.get(rig)
        if arm_obj is None:
            raise RuntimeError(f"Armature named '{rig}' not found.")
        if arm_obj.type != 'ARMATURE':
            raise RuntimeError(f"Object '{rig}' is not an Armature.")
    # find the bone
        if bone:
            pose_bone = arm_obj.pose.bones.get(bone)
            if pose_bone is None:
                raise RuntimeError(f"Bone '{bone}' not found in armature '{arm_obj.name}'.")
            # we now have the empty and the bone
            # get the minx, miny and max x and maxy from the empty action
            # Ensure animation_data exists on armature
            #
            ensure_pose_mode(arm_obj)
            #
            if arm_obj.animation_data is None:
                arm_obj.animation_data_create()
            # get the animation data    
            ad = arm_obj.animation_data 
            #ar = ad.action.frame_range
            #
            x = sample_at_frame(empty_obj,"location",index = 0, frame = 1)
            y = sample_at_frame(empty_obj,"location",index = 1, frame = 1)
            z = sample_at_frame(empty_obj,"location",index = 2, frame = 1)
            print(f"range {x} {y}  ")
            # Get the range of the controlling empty / empties
            add_transformation_constraint(arm_obj, bone, empty_obj,None,'LOCATION','LOCATION',
            (x - 0.01, y - 0.01, z - 0.01),(x + 0.01,y + 0.01,z + 0.01),
            (-1.0 * scale, -1.0 * scale, 0.0),(1.0 * scale, 1.0 * scale, 0),owner,target)
#
def loadTransfer(context,rig,scaleV):
    # walk the bone list and the empty list and do the transfer
    for i in xferList:
        mapEmptyToBone(rig, i[1], i[0], scaleV,i[3],i[2])        


class TransferLoadOperator(bpy.types.Operator):
    
    bl_idname = 'cc.control_load'
    bl_label = 'Transfer Load'
    def execute(self, context):
        rig = bpy.data.objects[context.scene.armature] 
        context.view_layer.objects.active = rig
        bpy.ops.object.mode_set(mode='POSE')
        scaleV = context.scene.scaleVisime
        loadTransfer(context,context.scene.armature,scaleV)
        # load the moho file
        return {'FINISHED'}

    
    
# Get the Armature    
class TransferLoadPanel(bpy.types.Panel):
    bl_label = 'Transfer Load'
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
    TransferLoadOperator,
    TransferLoadPanel,
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