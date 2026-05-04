import bpy
import math
from bpy.props import BoolProperty
from bpy.types import Panel, Operator

# Global variable to store detected bone prefix
detected_bone_prefix = 'mixamorig:'

bl_info = {
    "name": "Mixamo Pose to Animation",
    "author": "Your Name",
    "version": (1, 0),
    "blender": (3, 0, 0),
    "location": "View3D > Sidebar > Mixamo Pose",
    "description": "Apply predefined pose to Mixamo armature with keyframe preservation",
    "category": "Animation",
}

def set_pose_rotation_safe(bone_name, x, y, z):
    """Set pose rotation in a way that preserves animation with better error handling"""
    obj = bpy.context.active_object
    if not obj or obj.type != 'ARMATURE':
        print(f'✗ No armature selected for {bone_name}')
        return False
    
    bones = obj.pose.bones
    
    # Get the detected prefix from global variable
    global detected_bone_prefix
    
    # Extract base name (remove any prefix)
    base_name = bone_name
    import re
    
    # Remove any mixamorig pattern (mixamorig, mixamorig1, mixamorig2, etc.)
    match = re.match(r'(mixamorig\d*|mixamo|mixamo_orig):(.+)', bone_name)
    if match:
        base_name = match.group(2)
    
    # Try different bone name variations
    bone_variations = [
        bone_name,  # Original name
        base_name,  # Without any prefix
        # Also try with the detected prefix
        f"{detected_bone_prefix}{base_name}",
    ]
    
    # Add variations for common mixamorig patterns
    common_mixamorig_patterns = ['mixamorig:', 'mixamorig1:', 'mixamorig2:', 'mixamorig3:', 'mixamorig4:', 'mixamorig5:', 'mixamo:', 'mixamo_orig:']
    for pattern in common_mixamorig_patterns:
        if pattern != detected_bone_prefix:
            bone_variations.append(f"{pattern}{base_name}")
    
    bone = None
    used_name = None
    
    for variation in bone_variations:
        if variation in bones:
            bone = bones[variation]
            used_name = variation
            break
    
    if bone is None:
        print(f'✗ Bone {bone_name} not found (tried variations: {bone_variations})')
        return False
    
    try:
        # Store original rotation mode and constraints
        original_mode = bone.rotation_mode
        original_constraints = []
        
        # Temporarily disable constraints that might interfere
        for constraint in bone.constraints:
            if constraint.type in ['COPY_ROTATION', 'LIMIT_ROTATION', 'TRANSFORM']:
                original_constraints.append((constraint, constraint.mute))
                constraint.mute = True
        
        # Set rotation mode to XYZ for consistent behavior
        bone.rotation_mode = 'XYZ'
        
        # Clear any existing rotation first and set the rotation
        bone.rotation_euler = (math.radians(x), math.radians(y), math.radians(z))
        
        # Restore original rotation mode
        bone.rotation_mode = original_mode
        
        # Restore constraints
        for constraint, was_muted in original_constraints:
            constraint.mute = was_muted
        
        print(f'✓ Set {used_name}: X={x:.1f}°, Y={y:.1f}°, Z={z:.1f}°')
        return True
        
    except Exception as e:
        print(f'✗ Error setting {bone_name}: {str(e)}')
        return False

def shift_keyframes_by_one(obj):
    """Shift all existing keyframes by 1 frame"""
    if obj.animation_data and obj.animation_data.action:
        action = obj.animation_data.action
        print(f"Found existing animation: {action.name}")
        
        # Count keyframes before shifting
        total_keyframes = 0
        for fcurve in action.fcurves:
            total_keyframes += len(fcurve.keyframe_points)
        
        print(f"Shifting {total_keyframes} keyframes by 1 frame...")
        
        # Shift all keyframes by 1 frame
        for fcurve in action.fcurves:
            for keyframe in fcurve.keyframe_points:
                keyframe.co[0] += 1  # Shift X coordinate (frame number) by 1
        
        print("✓ Successfully shifted all keyframes by 1 frame")
        return True
    else:
        print("No existing animation found")
        return False

def detect_bone_naming_issues(obj):
    """Detect potential bone naming issues and suggest fixes"""
    print("=== Bone Naming Analysis ===")
    
    bones = obj.pose.bones
    bone_names = [bone.name for bone in bones]
    
    # Check for common naming patterns dynamically
    import re
    
    # Find all mixamorig patterns (mixamorig, mixamorig1, mixamorig2, etc.)
    mixamorig_patterns = set()
    for name in bone_names:
        match = re.match(r'(mixamorig\d*):', name)
        if match:
            mixamorig_patterns.add(match.group(1) + ':')
    
    has_mixamorig_prefix = any(name.startswith('mixamorig:') for name in bone_names)
    has_mixamo_prefix = any(name.startswith('mixamo:') for name in bone_names)
    has_no_prefix = any(not re.match(r'(mixamorig\d*|mixamo|mixamo_orig):', name) for name in bone_names)
    
    print(f"Bone naming patterns found:")
    print(f"  - mixamorig patterns: {sorted(mixamorig_patterns)}")
    print(f"  - mixamo: prefix: {has_mixamo_prefix}")
    print(f"  - No prefix: {has_no_prefix}")
    
    # Determine the correct prefix to use
    if mixamorig_patterns:
        # Use the first mixamorig pattern found (they should all be the same)
        prefix = sorted(mixamorig_patterns)[0]
    elif has_mixamo_prefix:
        prefix = 'mixamo:'
    elif has_no_prefix:
        prefix = ''
    else:
        prefix = 'mixamorig:'  # default fallback
    
    print(f"Using prefix: '{prefix}'")
    
    # Check for specific bones we need
    required_bones = [
        f'{prefix}LeftUpLeg', f'{prefix}RightUpLeg',
        f'{prefix}LeftArm', f'{prefix}RightArm',
        f'{prefix}LeftShoulder', f'{prefix}RightShoulder'
    ]
    
    missing_bones = []
    found_bones = []
    
    for required_bone in required_bones:
        variations = [
            required_bone,
            required_bone.replace('mixamorig:', ''),
            required_bone.replace('mixamo:', ''),
            required_bone.replace('mixamo_orig:', ''),
            required_bone.replace('mixamorig:', 'mixamo:'),
            required_bone.replace('mixamorig:', 'mixamo_orig:')
        ]
        
        found = False
        for variation in variations:
            if variation in bone_names:
                found_bones.append(variation)
                found = True
                break
        
        if not found:
            missing_bones.append(required_bone)
    
    if missing_bones:
        print(f"⚠️  Missing critical bones: {missing_bones}")
        print("This might cause pose application to fail!")
    else:
        print("✓ All critical bones found")
    
    # Store the detected prefix globally for use in pose application
    global detected_bone_prefix
    detected_bone_prefix = prefix
    
    return len(missing_bones) == 0

def get_bone_name(base_name):
    """Get the correct bone name using the detected prefix"""
    global detected_bone_prefix
    return f"{detected_bone_prefix}{base_name}"

class MIXAMO_OT_apply_pose(Operator):
    bl_idname = "mixamo.apply_pose"
    bl_label = "Apply Pose"
    bl_description = "Apply predefined pose to Mixamo armature"
    
    preserve_animation: BoolProperty(
        name="Preserve Animation",
        description="Shift existing keyframes to preserve animation",
        default=True
    )
    
    def execute(self, context):
        obj = context.active_object
        if not obj or obj.type != 'ARMATURE':
            self.report({'ERROR'}, "Please select a Mixamo armature first!")
            return {'CANCELLED'}
        
        print("=== Applying Pose (Addon Workflow) ===")
        
        # Analyze bone naming before proceeding
        bones_ok = detect_bone_naming_issues(obj)
        if not bones_ok:
            self.report({'WARNING'}, "Some critical bones may be missing. Pose application might be incomplete.")
        
        # Ensure we're in pose mode
        if obj.mode != 'POSE':
            print("Switching to pose mode...")
            bpy.ops.object.posemode_toggle()
        
        # Step 1: Select all bones
        print("Step 1: Selecting all bones...")
        bpy.ops.pose.select_all(action='SELECT')
        
        # Step 2: Clear rotation
        print("Step 2: Clearing rotation...")
        bpy.ops.pose.rot_clear()
        
        # Step 3: Shift keyframes and set frame to 1
        if self.preserve_animation:
            print("Step 3: Shifting existing keyframes by 1 frame...")
            shift_keyframes_by_one(obj)
        
        print("Step 3.5: Setting current frame to 1...")
        bpy.context.scene.frame_set(1)
        
        # Step 4: Select all bones and reset to rest pose
        print("Step 4: Selecting all bones and resetting to rest pose...")
        bpy.ops.pose.select_all(action='SELECT')
        bpy.ops.pose.rot_clear()
        bpy.ops.pose.loc_clear()
        
        # Step 5: Deselect all
        print("Step 5: Deselecting all...")
        bpy.ops.pose.select_all(action='DESELECT')
        
        # Step 6: Apply the pose rotations
        print("Step 6: Applying pose rotations...")
        
        successful_bones = 0
        total_bones = 0
        
        # Left side rotations
        print("\n--- LEFT SIDE ---")
        total_bones += 6
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftUpLeg'), 0.0, -0.1, 8.1)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftLeg'), -11.1, -20.5, 0.2)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftFoot'), 4.5, -1.5, -0.9)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftShoulder'), 14.7, 3.3, -4.2)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftArm'), 32.8, 4.7, 17.9)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftForeArm'), -13.2, -12.4, 26.9)
        
        # Left hand fingers
        print("\n--- LEFT HAND FINGERS ---")
        # Thumb
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandThumb1'), -5.8, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandThumb2'), 0.0, 0.0, -12.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandThumb3'), 0.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandThumb4'), 0.0, 0.0, 0.0)
        
        # Index finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandIndex1'), 15.0, 0.0, 1.3)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandIndex2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandIndex3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandIndex4'), 0.0, 0.0, 0.0)
        
        # Middle finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandMiddle1'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandMiddle2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandMiddle3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandMiddle4'), 0.0, 0.0, 0.0)
        
        # Ring finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandRing1'), 15.0, 0.0, -6.3)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandRing2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandRing3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandRing4'), 0.0, 0.0, 0.0)
        
        # Pinky finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandPinky1'), 15.0, 0.0, -6.9)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandPinky2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandPinky3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('LeftHandPinky4'), 15.0, 0.0, 0.0)
        
        # Right side rotations (mirrored)
        print("\n--- RIGHT SIDE (Mirrored) ---")
        total_bones += 6
        successful_bones += set_pose_rotation_safe(get_bone_name('RightUpLeg'), 0.0, 0.1, -8.1)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightLeg'), -11.1, 20.5, -0.2)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightFoot'), 4.5, 1.5, 0.9)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightShoulder'), 14.9, -2.1, -0.3)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightArm'), 32.8, -4.7, -17.9)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightForeArm'), -11.7, 13.8, -20.3)
        
        # Right hand fingers (mirrored)
        print("\n--- RIGHT HAND FINGERS (Mirrored) ---")
        # Thumb
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandThumb1'), -5.8, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandThumb2'), 0.0, 0.0, 12.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandThumb3'), 0.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandThumb4'), 0.0, 0.0, 0.0)
        
        # Index finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandIndex1'), 15.0, 0.0, -1.3)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandIndex2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandIndex3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandIndex4'), 0.0, 0.0, 0.0)
        
        # Middle finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandMiddle1'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandMiddle2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandMiddle3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandMiddle4'), 0.0, 0.0, 0.0)
        
        # Ring finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandRing1'), 15.0, 0.0, 6.3)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandRing2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandRing3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandRing4'), 0.0, 0.0, 0.0)
        
        # Pinky finger
        total_bones += 4
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandPinky1'), 15.0, 0.0, 6.9)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandPinky2'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandPinky3'), 15.0, 0.0, 0.0)
        successful_bones += set_pose_rotation_safe(get_bone_name('RightHandPinky4'), 15.0, 0.0, 0.0)
        
        # Step 7: Select all bones again
        print("Step 7: Selecting all bones...")
        bpy.ops.pose.select_all(action='SELECT')
        
        # Step 8: Keyframe insert
        print("Step 8: Keyframe inserting...")
        bpy.ops.anim.keyframe_insert()
        
        # Step 9: Toggle pose mode
        print("Step 9: Toggling pose mode...")
        bpy.ops.object.posemode_toggle()
        
        print("\n=== POSE APPLICATION SUMMARY ===")
        print(f"Successfully set {successful_bones} out of {total_bones} bones")
        success_rate = (successful_bones / total_bones * 100) if total_bones > 0 else 0
        print(f"Success rate: {success_rate:.1f}%")
        
        if success_rate < 100:
            print("⚠️  Some bones could not be set. This might be due to:")
            print("   - Different bone naming conventions")
            print("   - Missing finger bones in the rig")
            print("   - Constraints interfering with rotation")
            print("   - Rig not being a standard Mixamo rig")
        
        print("\n=== DONE ===")
        print("✓ Applied pose following addon workflow!")
        print("The pose should now be visible and keyframed!")
        
        if success_rate >= 80:
            self.report({'INFO'}, f"Pose applied successfully! ({success_rate:.0f}% bones set)")
        else:
            self.report({'WARNING'}, f"Pose partially applied. {success_rate:.0f}% bones set")
        
        return {'FINISHED'}

class MIXAMO_PT_panel(Panel):
    bl_label = "Mixamo Pose"
    bl_idname = "MIXAMO_PT_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = 'Mixamo Pose'
    
    def draw(self, context):
        layout = self.layout
        
        # Check if armature is selected
        obj = context.active_object
        if obj and obj.type == 'ARMATURE':
            layout.label(text=f"Selected: {obj.name}")
            
            # Add option to disable animation preservation
            layout.prop(context.scene, "mixamo_preserve_animation", text="Preserve Animation")
            
            # Add the apply pose button
            op = layout.operator("mixamo.apply_pose", text="Apply Pose")
            op.preserve_animation = context.scene.mixamo_preserve_animation
            
        else:
            layout.label(text="Select a Mixamo armature", icon='ERROR')

# Register classes
classes = [
    MIXAMO_OT_apply_pose,
    MIXAMO_PT_panel,
]

def register():
    # Add property to scene for UI
    bpy.types.Scene.mixamo_preserve_animation = BoolProperty(
        name="Preserve Animation",
        description="Shift existing keyframes to preserve animation",
        default=True
    )
    
    # Register all classes
    for cls in classes:
        bpy.utils.register_class(cls)

def unregister():
    # Unregister all classes
    for cls in reversed(classes):
        bpy.utils.unregister_class(cls)
    
    # Remove property
    del bpy.types.Scene.mixamo_preserve_animation

if __name__ == "__main__":
    register() 