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
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
skin = gr.material({1.0, 0.8, 0.6}, {0.1, 0.1, 0.1}, 5)
brown = gr.material({0.4, 0.2, 0}, {0.1, 0.1, 0.1}, 5)
lightGreen = gr.material({0, 0.8, 0}, {0.1, 0.1, 0.1}, 5)

torso = gr.mesh('Cylinder', 'torso')
rootnode:add_child(torso)
torso:scale(0.5,0.3,0.4);
torso:set_material(lightGreen)

-- upper body
shoulder = gr.mesh('sphere', 'shoulder')
torso:add_child(shoulder)
shoulder:scale(0.5, 0.4, 0.4)
shoulder:translate(0.0, 0.3, 0.0)
shoulder:set_material(lightGreen)

headJoint = gr.joint('headJoint', {-45, 0, 45}, {-90, 0, 90})
headJoint:translate(0.0, 0.3, 0.0)
shoulder:add_child(headJoint)

head = gr.mesh('sphere', 'head')
headJoint:add_child(head)
head:scale(0.5, 0.6, 0.4)
head:translate(0.0, 0.3, 0.0)
head:set_material(skin)

leftArmBodyConnection = gr.mesh('sphere', 'leftArmBodyConnection')
shoulder:add_child(leftArmBodyConnection)
leftArmBodyConnection:scale(0.05, 0.05, 0.05)
leftArmBodyConnection:translate(-0.5, 0, 0.0)
leftArmBodyConnection:set_material(lightGreen)


rightArmBodyConnection = gr.mesh('sphere', 'rightArmBodyConnection')
shoulder:add_child(rightArmBodyConnection)
rightArmBodyConnection:scale(0.05, 0.05, 0.05)
rightArmBodyConnection:translate(0.5, 0, 0.0)
rightArmBodyConnection:set_material(lightGreen)

leftArmBodyConnectionJoint = gr.joint('leftArmBodyConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
leftArmBodyConnectionJoint:translate(0.0, 0.0, 0.0)
leftArmBodyConnection:add_child(leftArmBodyConnectionJoint)

rightArmBodyConnectionJoint = gr.joint('rightArmBodyConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
rightArmBodyConnectionJoint:translate(0.0, 0, 0.0)
rightArmBodyConnection:add_child(rightArmBodyConnectionJoint)

leftUpperArm = gr.mesh('Cylinder', 'leftUpperArm')
leftArmBodyConnectionJoint:add_child(leftUpperArm)
leftUpperArm:scale(0.09, 0.1, 0.09)
leftUpperArm:rotate('z', -90)
leftUpperArm:translate(-0.1, 0, 0.0)
leftUpperArm:set_material(lightGreen)

rightUpperArm = gr.mesh('Cylinder', 'rightUpperArm')
rightArmBodyConnectionJoint:add_child(rightUpperArm)
rightUpperArm:scale(0.09, 0.1, 0.09)
rightUpperArm:rotate('z', 90)
rightUpperArm:translate(0.1, 0, 0.0)
rightUpperArm:set_material(lightGreen)

leftArmArmConnection = gr.mesh('sphere', 'leftArmArmConnection')
leftUpperArm:add_child(leftArmArmConnection)
leftArmArmConnection:scale(0.05, 0.05, 0.05)
leftArmArmConnection:translate(-0.1, 0.0, 0.0)
leftArmArmConnection:set_material(lightGreen)

rightArmArmConnection = gr.mesh('sphere', 'rightArmArmConnection')
rightUpperArm:add_child(rightArmArmConnection)
rightArmArmConnection:scale(0.05, 0.05, 0.05)
rightArmArmConnection:translate(0.1, 0.0, 0.0)
rightArmArmConnection:set_material(lightGreen)

leftArmArmConnectionJoint = gr.joint('leftArmArmConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
leftArmArmConnectionJoint:translate(0.0, 0.0, 0.0)
leftArmArmConnection:add_child(leftArmArmConnectionJoint)

rightArmArmConnectionJoint = gr.joint('rightArmArmConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
rightArmArmConnectionJoint:translate(0.0, 0.0, 0.0)
rightArmArmConnection:add_child(rightArmArmConnectionJoint)

leftForeArm = gr.mesh('Cylinder', 'leftForeArm')
leftArmArmConnectionJoint:add_child(leftForeArm)
leftForeArm:scale(0.09, 0.1, 0.09)
leftForeArm:rotate('z', -90)
leftForeArm:translate(-0.1, 0.0, 0.0)
leftForeArm:set_material(lightGreen)

rightForeArm = gr.mesh('Cylinder', 'rightForeArm')
rightArmArmConnectionJoint:add_child(rightForeArm)
rightForeArm:scale(0.09, 0.1, 0.09)
rightForeArm:rotate('z', 90)
rightForeArm:translate(0.1, 0.0, 0.0)
rightForeArm:set_material(lightGreen)


leftArmHandConnection = gr.mesh('sphere', 'leftArmHandConnection')
leftForeArm:add_child(leftArmHandConnection)
leftArmHandConnection:scale(0.05, 0.05, 0.05)
leftArmHandConnection:translate(-0.1, 0.0, 0.0)
leftArmHandConnection:set_material(skin)

rightArmHandConnection = gr.mesh('sphere', 'rightArmHandConnection')
rightForeArm:add_child(rightArmHandConnection)
rightArmHandConnection:scale(0.05, 0.05, 0.05)
rightArmHandConnection:translate(0.1, 0.0, 0.0)
rightArmHandConnection:set_material(skin)

leftArmHandConnectionJoint = gr.joint('leftArmHandConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
leftArmHandConnectionJoint:translate(0.0, 0.0, 0.0)
leftArmHandConnection:add_child(leftArmHandConnectionJoint)

rightArmHandConnectionJoint = gr.joint('rightArmHandConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
rightArmHandConnectionJoint:translate(0.0, 0.0, 0.0)
rightArmHandConnection:add_child(rightArmHandConnectionJoint)


leftHand = gr.mesh('sphere', 'leftHand')
leftArmHandConnectionJoint:add_child(leftHand)
leftHand:scale(0.15, 0.1, 0.05)
leftHand:translate(-0.1, 0, 0.0)
leftHand:set_material(skin)

rightHand = gr.mesh('sphere', 'rightHand')
rightArmHandConnectionJoint:add_child(rightHand)
rightHand:scale(0.15, 0.1, 0.05)
rightHand:translate(0.1, 0, 0.0)
rightHand:set_material(skin)


-- lower body
leftHipTorsoConnection = gr.mesh('sphere', 'leftHipTorsoConnection')
torso:add_child(leftHipTorsoConnection)
leftHipTorsoConnection:scale(0.05, 0.05, 0.05)
leftHipTorsoConnection:translate(-0.25, -0.3, 0.0)
leftHipTorsoConnection:set_material(skin)

rightHipTorsoConnection = gr.mesh('sphere', 'rightHipTorsoConnection')
torso:add_child(rightHipTorsoConnection)
rightHipTorsoConnection:scale(0.05, 0.05, 0.05)
rightHipTorsoConnection:translate(0.25, -0.3, 0.0)
rightHipTorsoConnection:set_material(skin)

leftHipTorsoConnectionJoint = gr.joint('leftHipTorsoConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
leftHipTorsoConnectionJoint:translate(0.0, 0.0, 0.0)
leftHipTorsoConnection:add_child(leftHipTorsoConnectionJoint)

rightHipTorsoConnectionJoint = gr.joint('rightHipTorsoConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
rightHipTorsoConnectionJoint:translate(0.0, 0.0, 0.0)
rightHipTorsoConnection:add_child(rightHipTorsoConnectionJoint)

leftHip = gr.mesh('sphere', 'leftHip')
leftHipTorsoConnectionJoint:add_child(leftHip)
leftHip:scale(0.24, 0.3, 0.3)
leftHip:translate(0, 0, 0.0)
leftHip:set_material(brown)

rightHip = gr.mesh('sphere', 'rightHip')
rightHipTorsoConnectionJoint:add_child(rightHip)
rightHip:scale(0.24, 0.3, 0.3)
rightHip:translate(0.0, 0.0, 0.0)
rightHip:set_material(brown)

leftHipLegConnection = gr.mesh('sphere', 'leftHipLegConnection')
leftHip:add_child(leftHipLegConnection)
leftHipLegConnection:scale(0.05, 0.05, 0.05)
leftHipLegConnection:translate(0, -0.3, 0.0)
leftHipLegConnection:set_material(brown)

rightHipLegConnection = gr.mesh('sphere', 'rightHipLegConnection')
rightHip:add_child(rightHipLegConnection)
rightHipLegConnection:scale(0.05, 0.05, 0.05)
rightHipLegConnection:translate(0, -0.3, 0.0)
rightHipLegConnection:set_material(brown)

leftHipLegConnectionJoint = gr.joint('leftHipLegConnectionJoint', {-20, 0, 20}, {-10, 0, 10})
leftHipLegConnectionJoint:translate(0.0, 0.0, 0.0)
leftHipLegConnection:add_child(leftHipLegConnectionJoint)

rightHipLegConnectionJoint = gr.joint('rightHipLegConnectionJoint', {-20, 0, 20}, {-10, 0, 10})
rightHipLegConnectionJoint:translate(0.0, 0.0, 0.0)
rightHipLegConnection:add_child(rightHipLegConnectionJoint)

leftUpperLeg = gr.mesh('Cylinder', 'leftUpperLeg')
leftHipLegConnectionJoint:add_child(leftUpperLeg)
leftUpperLeg:scale(0.12, 0.075, 0.12)
leftUpperLeg:rotate('x', -15)
leftUpperLeg:translate(0.0, -0.025, 0.0)
leftUpperLeg:set_material(brown)

rightUpperLeg = gr.mesh('Cylinder', 'rightUpperLeg')
rightHipLegConnectionJoint:add_child(rightUpperLeg)
rightUpperLeg:scale(0.12, 0.075, 0.12)
rightUpperLeg:rotate('x', -15)
rightUpperLeg:translate(0.0, -0.025, 0.0)
rightUpperLeg:set_material(brown)

leftLegFootConnection = gr.mesh('sphere', 'leftLegFootConnection')
leftUpperLeg:add_child(leftLegFootConnection)
leftLegFootConnection:scale(0.01, 0.01, 0.01)
leftLegFootConnection:translate(0, -0.05, 0.0)
leftLegFootConnection:set_material(blue)

rightLegFootConnection = gr.mesh('sphere', 'rightLegFootConnection')
rightUpperLeg:add_child(rightLegFootConnection)
rightLegFootConnection:scale(0.01, 0.01, 0.01)
rightLegFootConnection:translate(0, -0.05, 0.0)
rightLegFootConnection:set_material(blue)

leftLegFootConnectionJoint = gr.joint('leftLegFootConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
leftLegFootConnectionJoint:translate(0.0, 0.0, 0.0)
leftLegFootConnection:add_child(leftLegFootConnectionJoint)

rightLegFootConnectionJoint = gr.joint('rightLegFootConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
rightLegFootConnectionJoint:translate(0.0, 0.0, 0.0)
rightLegFootConnection:add_child(rightLegFootConnectionJoint)


leftFoot = gr.mesh('Cylinder', 'leftFoot')
leftLegFootConnectionJoint:add_child(leftFoot)
leftFoot:scale(0.1, 0.075, 0.1)
leftFoot:rotate('x', 15)
leftFoot:translate(0.0, -0.025, 0.0)
leftFoot:set_material(black)

rightFoot = gr.mesh('Cylinder', 'rightFoot')
rightLegFootConnectionJoint:add_child(rightFoot)
rightFoot:scale(0.1, 0.075, 0.1)
rightFoot:rotate('x', 15)
rightFoot:translate(0.0, -0.025, 0.0)
rightFoot:set_material(black)

-- Face

leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.07, 0.1, 0.02)
leftEye:translate(0, 0.0, 0.4)
leftEye:translate(-0.2, 0.0, 0)
leftEye:set_material(black)

leftEyeMeiConnection = gr.mesh('sphere', 'leftEyeMeiConnection')
leftEye:add_child(leftEyeMeiConnection)
leftEyeMeiConnection:scale(0.01, 0.01, 0.01)
leftEyeMeiConnection:translate(0, 0.15, 0.0)
leftEyeMeiConnection:set_material(green)

leftEyeMeiConnectionJoint = gr.joint('leftEyeMeiConnectionJoint', {-45, 0, 45}, {-90, 0, 90})
leftEyeMeiConnectionJoint:translate(0.0, 0.0, 0.0)
leftEyeMeiConnection:add_child(leftEyeMeiConnectionJoint)

leftMei = gr.mesh('cube', 'leftMei')
leftEyeMeiConnectionJoint:add_child(leftMei)
leftMei:scale(0.16, 0.05, 0.02)
leftMei:rotate('x', 90)
-- leftMei:translate(0, 0.0, 0.4)
-- leftMei:translate(-0.2, 0.0, 0)
leftMei:set_material(black)

rightEye = gr.mesh('sphere', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.07, 0.1, 0.02)
rightEye:translate(0, 0.0, 0.4)
rightEye:translate(0.2, 0.0, 0)
rightEye:set_material(black)

nose = gr.mesh('pin', 'nose')
head:add_child(nose)
nose:scale(0.07, 0.1, 0.07)
nose:translate(0, 0.0, 0.4)
nose:translate(-0.06, -0.2, 0)
nose:set_material(red)

hairOne = gr.mesh('sphere', 'hairOne')
head:add_child(hairOne)
hairOne:scale(0.1, 0.5, 0.5)
hairOne:translate(0, 0.0, -0.1)
hairOne:translate(0, 0.2, 0)
hairOne:set_material(black)

hairTwo = gr.mesh('sphere', 'hairTwo')
head:add_child(hairTwo)
hairTwo:scale(0.1, 0.5, 0.5)
hairTwo:rotate('z', 30)
hairTwo:translate(-0.05, 0.0, -0.1)
hairTwo:translate(0, 0.2, 0)
hairTwo:set_material(black)

hairThree = gr.mesh('sphere', 'hairThree')
head:add_child(hairThree)
hairThree:scale(0.1, 0.5, 0.5)
hairThree:rotate('z', -30)
hairThree:translate(0.05, 0.0, -0.1)
hairThree:translate(0, 0.2, 0)
hairThree:set_material(black)

ear = gr.mesh('cube', 'ear')
head:add_child(ear)
ear:scale(1, 0.1, 0.1)
ear:set_material(skin)



return rootnode
