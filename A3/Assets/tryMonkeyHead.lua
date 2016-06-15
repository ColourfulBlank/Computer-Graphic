-- puppet.lua
-- A simplified puppet without posable joints, but that
-- looks roughly humanoid.

rootnode = gr.node('root')
rootnode:rotate('y', -20.0)
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)



torso = gr.mesh('Cylinder', 'torso')
rootnode:add_child(torso)
torso:scale(0.5,0.3,0.4);
torso:set_material(red)

-- upper body
-- shoulder = gr.mesh('sphere', 'shoulder')
-- torso:add_child(shoulder)
-- shoulder:scale(0.5, 0.65 * 0.3, 0.4)
-- shoulder:translate(0.0, 0.3, 0.0)
-- shoulder:set_material(red)

headJoint = gr.joint('headJoint', {-90, 0, 90}, {-90, 0, 90})
headJoint:translate(0.0, 0.9, 0.0)
torso:add_child(headJoint)



head = gr.mesh('sphere', 'head')
headJoint:add_child(head)
head:scale(1.1, 1.3*1.1, 1.4 * 1.0)
head:translate(0.0, 1.5, 0.0)
head:set_material(red)

-- leftArmBodyConnection = gr.mesh('sphere', 'leftArmBodyConnection')
-- shoulder:add_child(leftArmBodyConnection)
-- leftArmBodyConnection:scale(0.1, 1.3*0.1, 1.4 * 0.1)
-- leftArmBodyConnection:translate(-1.0, 0, 0.0)
-- leftArmBodyConnection:set_material(green)

-- rightArmBodyConnection = gr.mesh('sphere', 'rightArmBodyConnection')
-- shoulder:add_child(rightArmBodyConnection)
-- rightArmBodyConnection:scale(0.1, 1.3*0.1, 1.4 * 0.1)
-- rightArmBodyConnection:translate(1.0, 0, 0.0)
-- rightArmBodyConnection:set_material(green)

-- leftUpperArm = gr.mesh('Cylinder', 'leftUpperArm')
-- leftArmBodyConnection:add_child(leftUpperArm)
-- leftUpperArm:scale(1, 2.5, 1)
-- leftUpperArm:rotate('z', -90)
-- leftUpperArm:translate(-2.5, 0, 0.0)
-- leftUpperArm:set_material(red)

-- rightUpperArm = gr.mesh('Cylinder', 'rightUpperArm')
-- rightArmBodyConnection:add_child(rightUpperArm)
-- rightUpperArm:scale(1, 2.5, 1)
-- rightUpperArm:rotate('z', 90)
-- rightUpperArm:translate(2.5, 0, 0.0)
-- rightUpperArm:set_material(red)

-- leftArmArmConnection = gr.mesh('sphere', 'leftArmArmConnection')
-- leftUpperArm:add_child(leftArmArmConnection)
-- leftArmArmConnection:scale(1, 1/2.5, 1)
-- leftArmArmConnection:translate(0.0, -1.0, 0.0)
-- leftArmArmConnection:set_material(green)

-- rightArmArmConnection = gr.mesh('sphere', 'rightArmArmConnection')
-- rightUpperArm:add_child(rightArmArmConnection)
-- rightArmArmConnection:scale(1, 1/2.5, 1)
-- rightArmArmConnection:translate(0.0, -1.0, 0.0)
-- rightArmArmConnection:set_material(green)

-- leftForeArm = gr.mesh('Cylinder', 'leftForeArm')
-- leftArmArmConnection:add_child(leftForeArm)
-- leftForeArm:scale(1, 2.5, 1)
-- leftForeArm:translate(0, -2.5, 0.0)
-- leftForeArm:set_material(red)

-- rightForeArm = gr.mesh('Cylinder', 'rightForeArm')
-- rightArmArmConnection:add_child(rightForeArm)
-- rightForeArm:scale(1, 2.5, 1)
-- rightForeArm:translate(0, -2.5, 0.0)
-- rightForeArm:set_material(red)

-- leftArmHandConnection = gr.mesh('sphere', 'leftArmHandConnection')
-- leftForeArm:add_child(leftArmHandConnection)
-- leftArmHandConnection:scale(1, 1/2.5, 1)
-- leftArmHandConnection:translate(0.0, -1.0, 0.0)
-- leftArmHandConnection:set_material(green)

-- rightArmHandConnection = gr.mesh('sphere', 'rightArmHandConnection')
-- rightForeArm:add_child(rightArmHandConnection)
-- rightArmHandConnection:scale(1, 1/2.5, 1)
-- rightArmHandConnection:translate(0.0, -1.0, 0.0)
-- rightArmHandConnection:set_material(green)

-- leftHand = gr.mesh('sphere', 'leftHand')
-- leftArmHandConnection:add_child(leftHand)
-- leftHand:scale(1, 1.5, 1)
-- leftHand:translate(0, -2.5, 0.0)
-- leftHand:set_material(blue)

-- rightHand = gr.mesh('sphere', 'rightHand')
-- rightArmHandConnection:add_child(rightHand)
-- rightHand:scale(1, 1.5, 0.5)
-- rightHand:translate(0, -2.0, 0.0)
-- rightHand:set_material(blue)

-- -- lower body
-- leftHip = gr.mesh('sphere', 'leftHip')
-- torso:add_child(leftHip)
-- leftHip:scale(1.0, 1.3, 1.0)
-- leftHip:translate(0.0, -1.0, 0.0)
-- leftHip:set_material(red)






return rootnode
