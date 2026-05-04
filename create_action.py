import bpy

def create_location_action(obj, action_name="MyAction"):
    # Create a new action
    action = bpy.data.actions.new(name=action_name)

    # Ensure the object has animation data and assign the action
    obj.animation_data_create()
    obj.animation_data.action = action

    # Example keyframes for each axis (frame, value)
    frames = [
        (1,  obj.location.x),
        (20, obj.location.x + 2.0),
        (40, obj.location.x)
    ]

    # Create 3 f-curves for location.x/y/z and populate keyframes
    for idx in range(3):
        fcu = action.fcurves.new(data_path="location", index=idx, action_group="Location")

        # Add the same example frames for each axis, but offset Y/Z slightly for demo
        if idx == 0:
            pts = frames
        elif idx == 1:
            pts = [(f, v + 0.5) for f, v in frames]  # Y offset
        else:
            pts = [(f, v - 0.5) for f, v in frames]  # Z offset

        # Add keyframe points in bulk (fcu.keyframe_points starts empty)
        fcu.keyframe_points.add(len(pts))

        # Set each keyframe point coordinate and interpolation/handles
        for i, (frame, value) in enumerate(pts):
            kp = fcu.keyframe_points[i]
            kp.co = (frame, value)
            kp.interpolation = 'BEZIER'          # 'CONSTANT', 'LINEAR', 'BEZIER', etc.
            kp.handle_left_type = 'AUTO'
            kp.handle_right_type = 'AUTO'

        # Recommended to update after edits
        fcu.update()

        # Optional: add a noise modifier to demonstrate modifiers usage
        mod = fcu.modifiers.new(type='NOISE')
        mod.scale = 10.0
        mod.strength = 0.1
        mod.phase = 0.0

    return action

# Usage: pick the active object (or replace with bpy.data.objects['Cube'] etc.)
obj = bpy.context.active_object
if not obj:
    raise RuntimeError("Select an object first.")

action = create_location_action(obj, "Example_Loc_Action")
print("Created action:", action.name)